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
#include "bossbullet.h"
#include "enemy.h"
#include "fade.h"
#include "debugproc.h"
#include "camera.h"
#include "light.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	BULLET_WIDTH		(10.0f)			// 頂点サイズ
#define	BULLET_HEIGHT		(10.0f)			// 頂点サイズ

#define	BULLET_SPEED		(5.0f)			// 弾の移動スピード
#define BOSSBULLET_SCL		(5.0f)			// 弾のスケール
#define BOSSBULLET_HP		(60.0f)		// 弾の出現時間

//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexBossBullet(void);
// ベジェ曲線
XMVECTOR LinearInterpolation(XMVECTOR P0, XMVECTOR P1, float s);
XMVECTOR QuadraticBezierCurve(XMVECTOR p0, XMVECTOR p1, XMVECTOR p2, float s);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static BOSSBULLET						g_BossBullet[MAX_BOSSBULLET];	// 弾ワーク
static int							g_TexNo;				// テクスチャ番号
static BOOL							g_bAlpaTest;		// アルファテストON/OFF

static char *g_TextureName[] =
{
	"data/TEXTURE/blt.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBossBullet(void)
{
	MakeVertexBossBullet();

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
	for (int nCntBullet = 0; nCntBullet < MAX_BOSSBULLET; nCntBullet++)
	{
		ZeroMemory(&g_BossBullet[nCntBullet].material, sizeof(g_BossBullet[nCntBullet].material));
		g_BossBullet[nCntBullet].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_BossBullet[nCntBullet].pos  = { 0.0f, 0.0f, 0.0f };
		g_BossBullet[nCntBullet].rot  = { 0.0f, 0.0f, 0.0f };
		g_BossBullet[nCntBullet].move = { 0.0f, 0.0f, 0.0f };
		g_BossBullet[nCntBullet].scl  = { 1.0f, 1.0f, 1.0f };
		g_BossBullet[nCntBullet].spd  = BULLET_SPEED;
		g_BossBullet[nCntBullet].hp   = BOSSBULLET_HP;
		g_BossBullet[nCntBullet].fWidth = BULLET_WIDTH;
		g_BossBullet[nCntBullet].fHeight = BULLET_HEIGHT;
		g_BossBullet[nCntBullet].use = FALSE;

	}


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBossBullet(void)
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
void UpdateBossBullet(void)
{
	ENEMY* enemy = GetEnemy();
	//PLAYER* player = GetPlayer();


	for (int i = 0; i < MAX_BOSSBULLET; i++)
	{
		if (g_BossBullet[i].use)
		{
			// 弾の移動処理 ベジェ曲線
			g_BossBullet[i].s += 0.005f;
			g_BossBullet[i].hp++;

			if (g_BossBullet[i].s > 1.0f)
			{
				g_BossBullet[i].use = FALSE;
				g_BossBullet[i].s = 0.0f;
				ReleaseShadow(g_BossBullet[i].shadowIdx);
			}


			XMVECTOR vP0 = XMLoadFloat3(&g_BossBullet[i].p0);
			XMVECTOR vP1 = XMLoadFloat3(&g_BossBullet[i].p1);
			XMVECTOR vP2 = XMLoadFloat3(&g_BossBullet[i].p2t);


			float s = g_BossBullet[i].s * g_BossBullet[i].s;
			XMVECTOR vAns = QuadraticBezierCurve(vP0, vP1, vP2, s);

			XMStoreFloat3(&g_BossBullet[i].pos, vAns);
			XMVECTOR vold_pos = XMLoadFloat3(&g_BossBullet[i].pos);

			XMVECTOR vec = vAns - vold_pos;        //移動前とあとの座標の差分（角度）
			float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);        //その差分を使って角度を求めている

			// 影の位置設定
			SetPositionShadow(g_BossBullet[i].shadowIdx, XMFLOAT3(g_BossBullet[i].pos.x, 0.1f, g_BossBullet[i].pos.z));

			// 時間経過で消す
			if (g_BossBullet[i].hp >= BOSSBULLET_HP)
			{
				g_BossBullet[i].use = FALSE;
				g_BossBullet[i].hp = 0.0f;
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBossBullet(void)
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

	for (int i = 0; i < MAX_BOSSBULLET; i++)
	{
		if (g_BossBullet[i].use)
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
			mtxScl = XMMatrixScaling(g_BossBullet[i].scl.x * BOSSBULLET_SCL, g_BossBullet[i].scl.y * BOSSBULLET_SCL, g_BossBullet[i].scl.z * BOSSBULLET_SCL);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_BossBullet[i].pos.x, g_BossBullet[i].pos.y, g_BossBullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_BossBullet[i].material);

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
HRESULT MakeVertexBossBullet(void)
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

	float fWidth = BULLET_WIDTH;
	float fHeight = BULLET_HEIGHT;

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
int SetBossBullet(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 tpos)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BOSSBULLET; nCntBullet++)
	{
		if (!g_BossBullet[nCntBullet].use)
		{
			g_BossBullet[nCntBullet].pos = pos;
			g_BossBullet[nCntBullet].p0 = pos;
			g_BossBullet[nCntBullet].p1 = pos;

			g_BossBullet[nCntBullet].p1.x = 200.0f;
			g_BossBullet[nCntBullet].p1.y = 200.0f;
			g_BossBullet[nCntBullet].p1.z = 200.0f;

			g_BossBullet[nCntBullet].p2t = tpos;

			g_BossBullet[nCntBullet].rot = rot;
			g_BossBullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_BossBullet[nCntBullet].use = TRUE;

			// 影の設定
			g_BossBullet[nCntBullet].shadowIdx = CreateShadow(g_BossBullet[nCntBullet].pos, 0.5f, 0.5f);

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
BOSSBULLET * GetBossBullet(void)
{
	return &(g_BossBullet[0]);
}

//=============================================================================
// ベジェ曲線関連
//=============================================================================

XMVECTOR LinearInterpolation(XMVECTOR P0, XMVECTOR P1, float s)
{
	XMVECTOR A0;
	A0 = P0 + (P1 - P0) * s;
	return A0;
}



XMVECTOR QuadraticBezierCurve(XMVECTOR p0, XMVECTOR p1, XMVECTOR p2, float s)
{
	XMVECTOR a0 = LinearInterpolation(p0, p1, s);
	XMVECTOR a1 = LinearInterpolation(p1, p2, s);
	XMVECTOR b0 = LinearInterpolation(a0, a1, s);

	return b0;
}
