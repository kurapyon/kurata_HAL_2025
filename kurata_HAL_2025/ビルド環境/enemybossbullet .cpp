//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "model.h"
#include "renderer.h"
#include "shadow.h"
#include "collision.h"
#include "enemybossbullet .h"
#include "fade.h"
#include "debugproc.h"
#include "camera.h"
#include "light.h"
#include "player.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(100/2)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数
#define	ENEMYBULLETBULLET_WIDTH		(10.0f)			// 頂点サイズ
#define	ENEMYBULLETBULLET_HEIGHT	(10.0f)			// 頂点サイズ

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値
#define ENEMYBOSSBULLET_SCL			(5.0f)	// サイズ
#define ENEMYBOSSBULLET_HP			(240.0f)// ライフ
#define ENEMYBOSSBULLET_START		(1.0f)// 弾のスタート位置

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexEnemyBossBullet(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static float					g_fWidthBase = 5.0f;			// 基準の幅
static float					g_fHeightBase = 10.0f;			// 基準の高さ

static char *g_TexturName[] = {
	"data/TEXTURE/blt.png",
};

static BOOL				g_Load = FALSE;		// 初期化を行ったかのフラグ
static ENEMYBOSSBULLET	g_EnemyBossBullet[ENEMYBOSSBULLET_MAX];	// バレット構造体
static BOOL				g_alpaTest;		// アルファテストON/OFF


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemyBossBullet(void)
{
	ID3D11Device *pDevice = GetDevice();
	MakeVertexEnemyBossBullet();
	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// バレット構造体の初期化
	for (int i = 0; i < ENEMYBOSSBULLET_MAX; i++)
	{
		g_EnemyBossBullet[i].use   = FALSE;			// 未使用（発射されていない弾）
		g_EnemyBossBullet[i].w     = TEXTURE_WIDTH;
		g_EnemyBossBullet[i].h     = TEXTURE_HEIGHT;
		g_EnemyBossBullet[i].pos   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyBossBullet[i].localpos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyBossBullet[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyBossBullet[i].texNo = 0;

		g_EnemyBossBullet[i].countAnim = 0;
		g_EnemyBossBullet[i].patternAnim = 0;

		g_EnemyBossBullet[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 移動量を初期化
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemyBossBullet(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemyBossBullet(void)
{

	ENEMYBOSS* enemyboss = GetEnemyBoss();
	XMFLOAT3 enemybosspos = enemyboss->pos;

	int bulletCount = 0;				// 処理したバレットの数

	for (int i = 0; i < ENEMYBOSSBULLET_MAX; i++)
	{
		if (g_EnemyBossBullet[i].use == TRUE)	// このバレットが使われている？
		{										// Yes
				if (g_EnemyBossBullet[i].use)
				{
					// 弾の移動処理
					g_EnemyBossBullet[i].localpos.x += g_EnemyBossBullet[i].move.x;
					g_EnemyBossBullet[i].localpos.y += g_EnemyBossBullet[i].move.y;
					g_EnemyBossBullet[i].localpos.z += g_EnemyBossBullet[i].move.z;

					g_EnemyBossBullet[i].pos.x = g_EnemyBossBullet[i].localpos.x + enemybosspos.x;
					g_EnemyBossBullet[i].pos.y = g_EnemyBossBullet[i].localpos.y + enemybosspos.y;
					g_EnemyBossBullet[i].pos.z = g_EnemyBossBullet[i].localpos.z + enemybosspos.z;


					// 影の位置設定
					SetPositionShadow(g_EnemyBossBullet[i].shadowIdx, XMFLOAT3(g_EnemyBossBullet[i].pos.x, 0.1f, g_EnemyBossBullet[i].pos.z));

					// 時間経過で消す
					if (g_EnemyBossBullet[i].hp >= ENEMYBOSSBULLET_HP)
					{
						g_EnemyBossBullet[i].use = FALSE;
						g_EnemyBossBullet[i].hp = 0.0f;
						ReleaseShadow(g_EnemyBossBullet[i].shadowIdx);

					}
				}
			}

			// 画面外まで進んだ？
			if (g_EnemyBossBullet[i].pos.x < (SCREEN_WIDTH -g_EnemyBossBullet[i].h/2))		// 自分の大きさを考慮して画面外か判定している
			{
				g_EnemyBossBullet[i].use = false;
			}
			if (g_EnemyBossBullet[i].pos.y > (SCREEN_HEIGHT+ g_EnemyBossBullet[i].h/2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_EnemyBossBullet[i].use = false;
			}

			bulletCount++;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemyBossBullet(void)
{
	// αテスト設定
	if (g_alpaTest == TRUE)
	{
		// αテストを有効に
		SetAlphaTestEnable(TRUE);
	}

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// ライティングを無効
	SetLightEnable(FALSE);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較無し
	SetDepthEnable(FALSE);

	// フォグ無効
	SetFogEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	for (int i = 0; i < ENEMYBOSSBULLET_MAX; i++)
	{
		if (g_EnemyBossBullet[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			// 処理が速いしお勧め
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// スケールを反映
			mtxScl = XMMatrixScaling(g_EnemyBossBullet[i].scl.x * ENEMYBOSSBULLET_SCL, g_EnemyBossBullet[i].scl.y * ENEMYBOSSBULLET_SCL, g_EnemyBossBullet[i].scl.z * ENEMYBOSSBULLET_SCL);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_EnemyBossBullet[i].pos.x, g_EnemyBossBullet[i].pos.y, g_EnemyBossBullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_EnemyBossBullet[i].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
	// ライティングを有効に
	SetLightEnable(TRUE);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);

	// フォグ有効
	SetFogEnable(GetFogEnable());

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexEnemyBossBullet(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = ENEMYBULLETBULLET_WIDTH;
	float fHeight = ENEMYBULLETBULLET_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = { -fWidth / 2.0f,  fHeight / 2.0f , 0.0f };
	vertex[1].Position = { fWidth / 2.0f,  fHeight / 2.0f , 0.0f };
	vertex[2].Position = { -fWidth / 2.0f, -fHeight / 2.0f , 0.0f };
	vertex[3].Position = { fWidth / 2.0f, -fHeight / 2.0f , 0.0f };

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 拡散光の設定
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// テクスチャ座標の設定
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
ENEMYBOSSBULLET* GetEnemyBossBullet(void)
{
	return &g_EnemyBossBullet[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetEneyBossBullet(XMFLOAT3 pos, XMFLOAT3 move, float hp, float spd)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < ENEMYBOSSBULLET_MAX; i++)
	{
		if (g_EnemyBossBullet[i].use == FALSE)	// 未使用状態のバレットを見つける
		{
			g_EnemyBossBullet[i].use = TRUE;			// 使用状態へ変更する
			g_EnemyBossBullet[i].localpos = pos;		// 座標をセット
			g_EnemyBossBullet[i].move = move;			// 移動量をセット
			g_EnemyBossBullet[i].hp = hp;
			g_EnemyBossBullet[i].spd = spd;
			return;								// 1発セットしたので終了する
		}
	}
}

//=============================================================================
// 弾のパラメータをセット
//=============================================================================
int SetBossAttackBullet(void)
{
	ENEMYBOSS* enemyboss = GetEnemyBoss();
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < ENEMYBOSSBULLET_MAX; nCntBullet++)
	{
		if (!g_EnemyBossBullet[nCntBullet].use)
		{

			XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);		// パーティクルの回転(初期化)
			XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// パーティクルの回転(初期化)
			XMFLOAT3		move;
			float			fAngleX, fAngleY, fAngleZ, fLength;
			float			nLife;
			float			fSize;


			XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// アフィン変換
			// 移動を反映
			mtxTranslate = XMMatrixTranslation(0, 0, 1);//forward vector
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// 回転を反映
			fAngleX = (float)(rand() % 628 - 314) / g_fHeightBase;
			fAngleY = (float)(rand() % 628 - 314) / g_fHeightBase;
			fAngleZ = (float)(rand() % 628 - 314) / g_fHeightBase;

			mtxRot = XMMatrixRotationRollPitchYaw(fAngleX, fAngleY, fAngleZ);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
			XMFLOAT4X4 result;
			XMStoreFloat4x4(&result, mtxWorld);

			// ベクトル
			XMFLOAT3 vector;
			vector.x = result._41;
			vector.y = result._42;
			vector.z = result._43;

			move = XMFLOAT3(0, 0, 0);
			move.x = vector.x * 1.0f;
			move.y = vector.y * 1.0f;
			move.z = vector.z * 1.0f;

			// 座標
			pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			pos.x += vector.x * ENEMYBOSSBULLET_START;
			pos.y += vector.y * ENEMYBOSSBULLET_START;
			pos.z += vector.z * ENEMYBOSSBULLET_START;


			// パーティクル発生
			static float	g_fWidthBase  = ENEMYBULLETBULLET_WIDTH;			// 基準の幅
			static float	g_fHeightBase = ENEMYBULLETBULLET_HEIGHT;			// 基準の高さ

			fLength = rand() % (int)(g_fWidthBase * 200) / g_fHeightBase - g_fWidthBase;

			// プレイヤーとパーティクルの差分を求めている
			XMVECTOR vpos = XMLoadFloat3(&pos);
			XMVECTOR vec = XMLoadFloat3(&enemyboss->pos) - vpos;

			// 正規化（大きさを０～１にする）
			vec = XMVector3Normalize(vec);

			//// スピードを求めている
			float speed = ENEMYBOSSBULLET_SPEED;		// 正規化したとき

			// 弾のライフ
			nLife = ENEMYBOSSBULLET_HP;

			// fSize → これで弾の基本サイズを設定する
			fSize = ENEMYBOSSBULLET_SCL;

			SetEneyBossBullet(pos,move, nLife, speed);

			break;
		}
	}

	return nIdxBullet;
}