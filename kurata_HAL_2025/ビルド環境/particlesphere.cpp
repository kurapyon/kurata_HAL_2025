//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "light.h"
#include "particlesphere.h"
#include "player.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)			// テクスチャの数

#define	PARTICLE_SIZE_X		(40.0f)		// 頂点サイズ
#define	PARTICLE_SIZE_Y		(40.0f)		// 頂点サイズ
#define	VALUE_MOVE_PARTICLE	(5.0f)		// 移動速度

#define	MAX_PARTICLESPHERE	(512)		// パーティクル最大数

#define	DISP_SHADOW						// 影の表示
//#undef DISP_SHADOW

#define PARTICLE_START		(100.0f)	// パーティクルのスタート位置
#define PARTICLE_SPEED		(10.0f)		// パーティクルのスピード
#define PARTICLE_SAIZE		(1.0f)		// サイズ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		localpos;		// ローカル座標
	XMFLOAT3		rot;			// 回転
	XMFLOAT3		scale;			// スケール
	XMFLOAT3		move;			// 移動量
	MATERIAL		material;		// マテリアル
	float			fSizeX;			// 幅
	float			fSizeY;			// 高さ
	int				nIdxShadow;		// 影ID
	int				nLife;			// 寿命
	BOOL			use;			// 使用しているかどうか

} PARTICLESPHERE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticleSphere(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;					// テクスチャ番号

static PARTICLESPHERE			g_ParticleSphere[MAX_PARTICLESPHERE];		// パーティクルワーク
static XMFLOAT3					g_posBase;						// ビルボード発生位置
static float					g_fWidthBase = 5.0f;			// 基準の幅
static float					g_fHeightBase = 10.0f;			// 基準の高さ
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード

XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);					// パーティクルの座標(初期化)
XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);					// パーティクルの回転(初期化)

static char *g_TextureName[] =
{
	"data/TEXTURE/effect.jpg",
};

static BOOL							g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticleSphere(void)
{
	// 頂点情報の作成
	MakeVertexParticleSphere();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// パーティクルワークの初期化
	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLESPHERE; nCntParticle++)
	{
		g_ParticleSphere[nCntParticle].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleSphere[nCntParticle].localpos = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_ParticleSphere[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleSphere[nCntParticle].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_ParticleSphere[nCntParticle].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_ParticleSphere[nCntParticle].material, sizeof(g_ParticleSphere[nCntParticle].material));
		g_ParticleSphere[nCntParticle].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_ParticleSphere[nCntParticle].fSizeX = PARTICLE_SIZE_X;
		g_ParticleSphere[nCntParticle].fSizeY = PARTICLE_SIZE_Y;
		g_ParticleSphere[nCntParticle].nIdxShadow = -1;
		g_ParticleSphere[nCntParticle].nLife = 0;
		g_ParticleSphere[nCntParticle].use = FALSE;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticleSphere(void)
{
	if (g_Load == FALSE) return;

	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticleSphere(void)
{
	ENEMY* enemy = GetEnemy();
	XMFLOAT3 g_enemypos = enemy[0].pos;

	{
		for(int nCntParticle = 0; nCntParticle < MAX_PARTICLESPHERE; nCntParticle++)
		{
			if(g_ParticleSphere[nCntParticle].use)
			{// ローカル座標とmoveを足す

				g_ParticleSphere[nCntParticle].localpos.x += g_ParticleSphere[nCntParticle].move.x;
				g_ParticleSphere[nCntParticle].localpos.y += g_ParticleSphere[nCntParticle].move.y;
				g_ParticleSphere[nCntParticle].localpos.z += g_ParticleSphere[nCntParticle].move.z;

				g_ParticleSphere[nCntParticle].pos.x = g_ParticleSphere[nCntParticle].localpos.x + g_enemypos.x;
				g_ParticleSphere[nCntParticle].pos.y = g_ParticleSphere[nCntParticle].localpos.y + g_enemypos.y;
				g_ParticleSphere[nCntParticle].pos.z = g_ParticleSphere[nCntParticle].localpos.z + g_enemypos.z;



				g_ParticleSphere[nCntParticle].nLife--;
				if(g_ParticleSphere[nCntParticle].nLife <= 0)
				{
					g_ParticleSphere[nCntParticle].use = FALSE;
					ReleaseShadow(g_ParticleSphere[nCntParticle].nIdxShadow);
					g_ParticleSphere[nCntParticle].nIdxShadow = -1;
				}
				else
				{
					if(g_ParticleSphere[nCntParticle].nLife <= 80)
					{
						g_ParticleSphere[nCntParticle].material.Diffuse.x = 0.8f - (float)(80 - g_ParticleSphere[nCntParticle].nLife) / 80 * 0.8f;
						g_ParticleSphere[nCntParticle].material.Diffuse.y = 0.7f - (float)(80 - g_ParticleSphere[nCntParticle].nLife) / 80 * 0.7f;
						g_ParticleSphere[nCntParticle].material.Diffuse.z = 0.2f - (float)(80 - g_ParticleSphere[nCntParticle].nLife) / 80 * 0.2f;
					}

					if(g_ParticleSphere[nCntParticle].nLife <= 20)
					{
						// α値設定
						g_ParticleSphere[nCntParticle].material.Diffuse.w -= 0.05f;
						if(g_ParticleSphere[nCntParticle].material.Diffuse.w < 0.0f)
						{
							g_ParticleSphere[nCntParticle].material.Diffuse.w = 0.0f;
						}
					}
				}
			}
		}

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticleSphere(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(FALSE);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_SUBTRACT);

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
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLESPHERE; nCntParticle++)
	{
		if(g_ParticleSphere[nCntParticle].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

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
			mtxScl = XMMatrixScaling(g_ParticleSphere[nCntParticle].scale.x, g_ParticleSphere[nCntParticle].scale.y, g_ParticleSphere[nCntParticle].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_ParticleSphere[nCntParticle].pos.x, g_ParticleSphere[nCntParticle].pos.y, g_ParticleSphere[nCntParticle].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_ParticleSphere[nCntParticle].material);

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
	SetFogEnable( GetFogEnable() );

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticleSphere(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorParticleSphere(int nIdxParticle, XMFLOAT4 col)
{
	g_ParticleSphere[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
//=============================================================================
int Particle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLESPHERE; nCntParticle++)
	{
		if (!g_ParticleSphere[nCntParticle].use)
		{
			//g_ParticleSphere[nCntParticle].pos = pos;
			g_ParticleSphere[nCntParticle].localpos = pos;
			g_ParticleSphere[nCntParticle].rot = rot;
			g_ParticleSphere[nCntParticle].scale = { PARTICLESPHERE_SCALE, PARTICLESPHERE_SCALE, PARTICLESPHERE_SCALE };
			g_ParticleSphere[nCntParticle].move = move;
			g_ParticleSphere[nCntParticle].material.Diffuse = col;
			g_ParticleSphere[nCntParticle].fSizeX = fSizeX;
			g_ParticleSphere[nCntParticle].fSizeY = fSizeY;
			g_ParticleSphere[nCntParticle].nLife = nLife;
			g_ParticleSphere[nCntParticle].use = TRUE;

			nIdxParticle = nCntParticle;

#ifdef DISP_SHADOW
			// 影の設定
			g_ParticleSphere[nCntParticle].nIdxShadow = CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// 影の設定
			if (g_ParticleSphere[nCntParticle].nIdxShadow != -1)
			{
				SetColorShadow(g_ParticleSphere[nCntParticle].nIdxShadow, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxParticle;
}




//=============================================================================
// パーティクル 球体
//=============================================================================
void SetParticleSphere(void)
{

	ENEMY* enemy = GetEnemy();

	// パーティクル用変数
	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);		// パーティクルの回転(初期化)
	XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// パーティクルの回転(初期化)
	XMFLOAT3		move;
	float			fAngleX, fAngleY, fAngleZ, fLength;
	int				nLife;
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
	move.x = vector.x * 1.0f * -1.0f;
	move.y = vector.y * 1.0f * -1.0f;
	move.z = vector.z * 1.0f * -1.0f;

	// 座標
	pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pos.x += vector.x * PARTICLE_START;
	pos.y += vector.y * PARTICLE_START;
	pos.z += vector.z * PARTICLE_START;


	// パーティクル発生
	static float	g_fWidthBase  = PARTICLESPHERE_WIDTH_BASE;			// 基準の幅
	static float	g_fHeightBase = PARTICLESPHERE_HEIGHTBASE;			// 基準の高さ

	fLength = rand() % (int)(g_fWidthBase * 200) / g_fHeightBase - g_fWidthBase;



	// プレイヤーとパーティクルの差分を求めている
	XMVECTOR vpos = XMLoadFloat3(&pos);
	XMVECTOR vec = XMLoadFloat3(&enemy[0].pos) - vpos;

	// 正規化（大きさを０～１にする）
	vec = XMVector3Normalize(vec);

	//// スピードを求めている
	float speed = PARTICLE_SPEED;		// 正規化したとき

	// パーティクルのライフ
	nLife = PARTICLESPHERE_LIFE;

	// fSize → これでパーティクルの基本サイズを設定する
	fSize = PARTICLE_SAIZE;

	// パーティクルの発生 エネミーに向けて集めている
	Particle(pos, move,  rot, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), fSize, fSize, nLife);


}

//// プレイヤーの位置に合わせて発生させている
//pos.x = pPlayer->pos.x + sinf(fAngleX) * 100;		// プレイヤーの左右に発生する
////pos.x = pPlayer->pos.x + rand() % 100;			// プレイヤーの右からに発生する
////pos.x = pPlayer->pos.x + rand() % 100 * -1;		// プレイヤーの左からに発生する

//pos.y = pPlayer->pos.y + sinf(fAngleY) * 100;		// プレイヤーの上下に発生する
////pos.y = pPlayer->pos.y + rand() % 100;			// プレイヤーの上からに発生する
////pos.y = pPlayer->pos.y + rand() % 100 * -1;		// プレイヤーの下からに発生する

//pos.z = pPlayer->pos.z + cosf(fAngleZ) * 100;		// プレイヤーの前後に発生する
////pos.z = pPlayer->pos.z + rand() % 100;			// プレイヤーの後ろからに発生する
////pos.z = pPlayer->pos.z + rand() % 100 * -1;		// プレイヤーの前からに発生する
	// ベクトルにスピードをかけて動かしている
	//move.x = vec.m128_f32[0] * speed * 1;
	//move.y = vec.m128_f32[1] * speed * 1;
	//move.z = vec.m128_f32[2] * speed * 1;

	//float speed = 0.01f;				// 正規化しないとき
