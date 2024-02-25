//=============================================================================
//
// 弾発射処理 [bullet.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "model.h"
#include "renderer.h"
#include "shadow.h"
#include "collision.h"
#include "enemybullet.h"
#include "enemy.h"
#include "fade.h"
#include "debugproc.h"
#include "camera.h"
#include "light.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	ENEMYBULLET_WIDTH		(10.0f)			// 頂点サイズ
#define	ENEMYBULLET_HEIGHT		(10.0f)			// 頂点サイズ

#define	ENEMYBULLET_SPEED		(10.0f)			// 弾の移動スピード


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexEnemyBullet(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static ENEMYBULLET					g_EnemyBullet[MAX_ENEMYBULLET];	// 弾ワーク
static int							g_TexNo;				// テクスチャ番号
static BOOL							g_bAlpaTest;		// アルファテストON/OFF

static char *g_TextureName[] =
{
	"data/TEXTURE/f00.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemyBullet(void)
{
	MakeVertexEnemyBullet();

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

	// 弾ワークの初期化
	for (int nCntBullet = 0; nCntBullet < MAX_ENEMYBULLET; nCntBullet++)
	{
		ZeroMemory(&g_EnemyBullet[nCntBullet].material, sizeof(g_EnemyBullet[nCntBullet].material));
		g_EnemyBullet[nCntBullet].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_EnemyBullet[nCntBullet].pos  = { 0.0f, 0.0f, 0.0f };
		g_EnemyBullet[nCntBullet].rot  = { 0.0f, 0.0f, 0.0f };
		g_EnemyBullet[nCntBullet].move = { 0.0f, 0.0f, 0.0f };
		g_EnemyBullet[nCntBullet].scl  = { 1.0f, 1.0f, 1.0f };
		g_EnemyBullet[nCntBullet].spd  = { ENEMYBULLET_SPEED, ENEMYBULLET_SPEED, ENEMYBULLET_SPEED };
		g_EnemyBullet[nCntBullet].fWidth = ENEMYBULLET_WIDTH;
		g_EnemyBullet[nCntBullet].fHeight = ENEMYBULLET_HEIGHT;
		g_EnemyBullet[nCntBullet].use = FALSE;

	}


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemyBullet(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemyBullet(void)
{

	for (int i = 0; i < MAX_ENEMYBULLET; i++)
	{
		if (g_EnemyBullet[i].use)
		{
			// 弾の移動処理
			g_EnemyBullet[i].pos.x -= sinf(g_EnemyBullet[i].rot.y) * g_EnemyBullet[i].spd.x;
			//g_EnemyAtc[i].pos.y -= sinf(g_EnemyAtc[i].rot.x) * g_EnemyAtc[i].spd.y;
			g_EnemyBullet[i].pos.z -= cosf(g_EnemyBullet[i].rot.y) * g_EnemyBullet[i].spd.z;

			// 影の位置設定
			SetPositionShadow(g_EnemyBullet[i].shadowIdx, XMFLOAT3(g_EnemyBullet[i].pos.x, 0.1f, g_EnemyBullet[i].pos.z));


			// フィールドの外に出たら弾を消す処理
			if (g_EnemyBullet[i].pos.x < MAP_LEFT
				|| g_EnemyBullet[i].pos.x > MAP_RIGHT
				|| g_EnemyBullet[i].pos.z < MAP_DOWN
				|| g_EnemyBullet[i].pos.z > MAP_TOP)
			{
				g_EnemyBullet[i].use = FALSE;
				ReleaseShadow(g_EnemyBullet[i].shadowIdx);
			}

		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemyBullet(void)
{
	// αテスト設定
	if (g_bAlpaTest == TRUE)
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

	for (int i = 0; i < MAX_ENEMYBULLET; i++)
	{
		if (g_EnemyBullet[i].use)
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
			mtxScl = XMMatrixScaling(g_EnemyBullet[i].scl.x * 2.0f, g_EnemyBullet[i].scl.y * 2.0f, g_EnemyBullet[i].scl.z * 2.0f);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_EnemyBullet[i].pos.x, g_EnemyBullet[i].pos.y, g_EnemyBullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_EnemyBullet[i].material);

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
HRESULT MakeVertexEnemyBullet(void)
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

	float fWidth = ENEMYBULLET_WIDTH;
	float fHeight = ENEMYBULLET_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = { -fWidth / 2.0f,  fHeight / 2.0f , 0.0f};
	vertex[1].Position = {  fWidth / 2.0f,  fHeight / 2.0f , 0.0f};
	vertex[2].Position = { -fWidth / 2.0f, -fHeight / 2.0f , 0.0f};
	vertex[3].Position = {  fWidth / 2.0f, -fHeight / 2.0f , 0.0f};

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
// 弾のパラメータをセット
//=============================================================================
int SetEnemyBullet(XMFLOAT3 pos, XMFLOAT3 rot)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_ENEMYBULLET; nCntBullet++)
	{
		if (!g_EnemyBullet[nCntBullet].use)
		{
			g_EnemyBullet[nCntBullet].pos = pos;
			g_EnemyBullet[nCntBullet].rot = rot;
			g_EnemyBullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_EnemyBullet[nCntBullet].use = TRUE;

			// 影の設定
			g_EnemyBullet[nCntBullet].shadowIdx = CreateShadow(g_EnemyBullet[nCntBullet].pos, 0.5f, 0.5f);

			nIdxBullet = nCntBullet;

		//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

//=============================================================================
// 弾の取得
//=============================================================================
ENEMYBULLET *GetEnemyBullet(void)
{
	return &(g_EnemyBullet[0]);
}

