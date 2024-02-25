//=============================================================================
//
// 空処理 [sky.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "shadow.h"
#include "player.h"
#include "sky.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MODEL_SKY			"data/MODEL/sky/sky.obj"

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量
#define VALUE_SIZE			(1.0f)						// スケール

#define SKY_SHADOW_SIZE		(0.4f)						// 影の大きさ
#define SKY_OFFSET_Y		(0.0f)						// エネミーの足元をあわせる
		
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static SKY			g_Sky[MAX_SKY];						// エネミー

int g_Sky_load = 0;


static INTERPOLATION_DATA move_tbl_sky[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,		  0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), 480 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 2,	  0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), 480 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI,		  0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), 480 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 1.5f,0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), 480 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_2PI,	  0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f),   1 },
};

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_sky,
};

enum
{
	tbl_sky,
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		LoadModel(MODEL_SKY, &g_Sky[i].model);
		g_Sky[i].load = TRUE;

		g_Sky[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Sky[i].spd  = 0.0f;			// 移動スピードクリア
		g_Sky[i].size = SKY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Sky[i].model, &g_Sky[i].diffuse[0]);
		
		g_Sky[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Sky[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Sky[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		g_Sky[i].use = TRUE;			// TRUE:生きてる
	}

	// 0番だけ線形補間で動かしてみる
	g_Sky[0].time   = 0.0f;			// 線形補間用のタイマーをクリア
	g_Sky[0].tblNo  = tbl_sky;		// 再生するアニメデータの先頭アドレスをセット
	g_Sky[0].tblMax = sizeof(move_tbl_sky) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].load)
		{
			UnloadModel(&g_Sky[i].model);
			g_Sky[i].load = FALSE;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSky(void)
{
	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes

			// 移動処理
			if (g_Sky[i].tblMax > 0)	// 線形補間を実行する？
			{	// 線形補間の処理
				int nowNo = (int)g_Sky[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Sky[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Sky[i].tblNo];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Sky[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Sky[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Sky[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Sky[i].scl, nowScl + Scl);

				// frameを使て時間経過処理をする
				g_Sky[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Sky[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Sky[i].time -= maxNo;					// ０番目にリセットしつつも小数部分を引き継いでいる
				}
			}
		}
	}

//#ifdef _DEBUG
//
//#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSky(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Sky[i].scl.x * VALUE_SIZE, g_Sky[i].scl.y * VALUE_SIZE, g_Sky[i].scl.z * VALUE_SIZE);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Sky[i].rot.x, g_Sky[i].rot.y + XM_PI, g_Sky[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Sky[i].pos.x, g_Sky[i].pos.y, g_Sky[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Sky[i].mtxWorld, mtxWorld);

		//// モデル描画
		DrawModel(&g_Sky[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_NONE);

}

//=============================================================================
// エネミーの取得
//=============================================================================
SKY *GetSky()
{
	return &g_Sky[0];
}
