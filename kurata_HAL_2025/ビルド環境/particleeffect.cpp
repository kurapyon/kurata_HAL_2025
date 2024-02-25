//=============================================================================
//
// スパークパーティクル処理 [particle_spark.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "debugproc.h"
#include "renderer.h"

#include "camera.h"
#include "particleeffect.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX						(1)

#define	PARTICLE_SPARK_SIZE_X			(4.0f)
#define	PARTICLE_SPARK_SIZE_Y			(4.0f)

#define	PARTICLE_SPARK_COLOR_R			(0.93f)
#define	PARTICLE_SPARK_COLOR_G			(0.47f)
#define	PARTICLE_SPARK_COLOR_B			(0.0f)
#define	PARTICLE_SPARK_COLOR_A			(1.0f)

#define	PARTICLE_SPARK_GENERATE_FRAME	(5)
#define	PARTICLE_SPARK_SCALE_FACTOR		(0.9f)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4		mtxWorld;	// ワールドマトリクス

	XMFLOAT3		scl;		// スケール
	XMFLOAT3		rot;		// ローテーション
	XMFLOAT3		pos;		// ポジション
	XMFLOAT3		basePos;	// ポジション(生成場所)

	MATERIAL		material;	// マテリアル

	XMFLOAT3		move;		// 移動方向
	float			spd;		// スピード

	int				life;		// 寿命

	BOOL			origin;		// 生成元かどうか

	BOOL			use;		// 使用中

} PARTICLE_SPARK;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticleEffect(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };

static PARTICLE_SPARK			g_ParticleSpark[PARTICLE_SPARK_MAX];

static int						g_TexNo;

static char *g_TextureName[] =
{
	"data/TEXTURE/effect.jpg",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticleEffect(void)
{
	// 頂点情報の作成
	MakeVertexParticleEffect();

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

	// スパークパーティクルの初期化
	for(int i = 0; i < PARTICLE_SPARK_MAX; i++)
	{
		XMStoreFloat4x4(&g_ParticleSpark[i].mtxWorld, XMMatrixIdentity());

		g_ParticleSpark[i].scl = { 1.0f, 1.0f, 1.0f };
		g_ParticleSpark[i].rot = { 0.0f, 0.0f, 0.0f };
		g_ParticleSpark[i].pos = { 0.0f, 0.0f, 0.0f };
		g_ParticleSpark[i].basePos = { 0.0f, 0.0f, 0.0f };

		ZeroMemory(&g_ParticleSpark[i].material, sizeof(g_ParticleSpark[i].material));
		g_ParticleSpark[i].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_ParticleSpark[i].move = { 0.0f, 0.0f, 0.0f };
		g_ParticleSpark[i].spd = 0.0f;

		g_ParticleSpark[i].life = 0;

		g_ParticleSpark[i].origin = FALSE;

		g_ParticleSpark[i].use = FALSE;
	}

	g_Load = TRUE;
	
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticleEffect(void)
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
void UpdateParticleEffect(void)
{
#ifdef _DEBUG
	int cnt = 0;
#endif

	for (int i = 0; i < PARTICLE_SPARK_MAX; i++)
	{
		if (g_ParticleSpark[i].use)
		{
			g_ParticleSpark[i].pos.x += g_ParticleSpark[i].move.x * g_ParticleSpark[i].spd;
			g_ParticleSpark[i].pos.y += g_ParticleSpark[i].move.y * g_ParticleSpark[i].spd;
			g_ParticleSpark[i].pos.z += g_ParticleSpark[i].move.z * g_ParticleSpark[i].spd;

			g_ParticleSpark[i].scl.x *= PARTICLE_SPARK_SCALE_FACTOR;
			g_ParticleSpark[i].scl.y *= PARTICLE_SPARK_SCALE_FACTOR;
			g_ParticleSpark[i].scl.z *= PARTICLE_SPARK_SCALE_FACTOR;

			if (g_ParticleSpark[i].origin && g_ParticleSpark[i].life > PARTICLE_SPARK_LIFE - PARTICLE_SPARK_GENERATE_FRAME)
			{
				SetParticleEffect(g_ParticleSpark[i].basePos, g_ParticleSpark[i].move, PARTICLE_SPARK_LIFE, FALSE);
			}

			g_ParticleSpark[i].life--;
			if (g_ParticleSpark[i].life <= 0)
			{
				ResetParticleSpark(i);
			}
			//cnt++;
		}
	}

#ifdef _DEBUG
	PrintDebugProc("[ParticleSpark]\n");
	PrintDebugProc("CNT: %d\n", cnt);
	PrintDebugProc("\n", cnt);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticleEffect(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *camera = GetCamera();

	// ライティング無効
	SetLightEnable(FALSE);

	// 加算合成
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無効
	SetDepthEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for(int i = 0; i < PARTICLE_SPARK_MAX; i++)
	{
		if (g_ParticleSpark[i].use)
		{
			// ワールドマトリクス初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリクス取得
			mtxView = XMLoadFloat4x4(&camera->mtxView);

			// 簡易的に逆行列にする
			// XMMatrixInverse()より高速
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
			mtxScl = XMMatrixScaling(g_ParticleSpark[i].scl.x, g_ParticleSpark[i].scl.y, g_ParticleSpark[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_ParticleSpark[i].pos.x, g_ParticleSpark[i].pos.y, g_ParticleSpark[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリクス設定
			SetWorldMatrix(&mtxWorld);

			// ワールドマトリクス保存
			XMStoreFloat4x4(&g_ParticleSpark[i].mtxWorld, mtxWorld);

			// マテリアル設定
			SetMaterial(g_ParticleSpark[i].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティング有効
	SetLightEnable(TRUE);

	// アルファ合成
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticleEffect(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);
	
	//頂点バッファの中身を埋める
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = { -PARTICLE_SPARK_SIZE_X / 2,  PARTICLE_SPARK_SIZE_Y / 2, 0.0f };
		vertex[1].Position = {  PARTICLE_SPARK_SIZE_X / 2,  PARTICLE_SPARK_SIZE_Y / 2, 0.0f };
		vertex[2].Position = { -PARTICLE_SPARK_SIZE_X / 2, -PARTICLE_SPARK_SIZE_Y / 2, 0.0f };
		vertex[3].Position = {  PARTICLE_SPARK_SIZE_X / 2, -PARTICLE_SPARK_SIZE_Y / 2, 0.0f };

		// ノーマルの設定
		vertex[0].Normal = { 0.0f, 0.0f, -1.0f };
		vertex[1].Normal = { 0.0f, 0.0f, -1.0f };
		vertex[2].Normal = { 0.0f, 0.0f, -1.0f };
		vertex[3].Normal = { 0.0f, 0.0f, -1.0f };

		// ディフーズの設定
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
	}

	return S_OK;
}

//=============================================================================
// スパークパーティクルを設定
//=============================================================================
int SetParticleEffect(XMFLOAT3 pos, XMFLOAT3 move, int life, BOOL origin)
{
	int idx = -1;

	for(int i = 0; i < PARTICLE_SPARK_MAX; i++)
	{
		if (!g_ParticleSpark[i].use)
		{
			g_ParticleSpark[i].scl = { 1.0f, 1.0f, 1.0f };
			g_ParticleSpark[i].rot = { 0.0f, 0.0f, 0.0f };
			g_ParticleSpark[i].pos = pos;
			g_ParticleSpark[i].basePos = pos;
			g_ParticleSpark[i].move = move;
			g_ParticleSpark[i].spd = PARTICLE_SPARK_SPD + (float)(rand() % 50 - 25) * 0.01f;
			g_ParticleSpark[i].material.Diffuse = { PARTICLE_SPARK_COLOR_R, PARTICLE_SPARK_COLOR_G, PARTICLE_SPARK_COLOR_B, PARTICLE_SPARK_COLOR_A };
			g_ParticleSpark[i].life = life;
			g_ParticleSpark[i].origin = origin;
			g_ParticleSpark[i].use = TRUE;

			idx = i;

			break;
		}
	}

	return idx;
}

//=============================================================================
// スパークパーティクルの移動方向を計算
//=============================================================================
XMFLOAT3 CalcParticleSparkMove(XMFLOAT3 hitPos)
{
	float theta = (float)((rand() % 628 - 314) * 0.01f);
	float phi = (float)((rand() % 628 - 314) * 0.01f);

	float moveX = sinf(phi) * cosf(theta);
	float moveY = cosf(phi);
	float moveZ = sinf(phi) * sinf(theta);
	XMFLOAT3 move = { moveX, moveY, moveZ };
	XMVECTOR vMove = XMVector3Normalize(XMLoadFloat3(&move));
	XMStoreFloat3(&move, vMove);

	return move;
}

//=============================================================================
// スパークパーティクルの再初期化
//=============================================================================
void ResetParticleSpark(int idx)
{
	XMStoreFloat4x4(&g_ParticleSpark[idx].mtxWorld, XMMatrixIdentity());

	g_ParticleSpark[idx].scl = { 1.0f, 1.0f, 1.0f };
	g_ParticleSpark[idx].rot = { 0.0f, 0.0f, 0.0f };
	g_ParticleSpark[idx].pos = { 0.0f, 0.0f, 0.0f };
	g_ParticleSpark[idx].basePos = { 0.0f, 0.0f, 0.0f };

	ZeroMemory(&g_ParticleSpark[idx].material, sizeof(g_ParticleSpark[idx].material));
	g_ParticleSpark[idx].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	g_ParticleSpark[idx].move = { 0.0f, 0.0f, 0.0f };
	g_ParticleSpark[idx].spd = 0.0f;

	g_ParticleSpark[idx].life = 0;
	g_ParticleSpark[idx].origin = FALSE;

	g_ParticleSpark[idx].use = FALSE;
}
