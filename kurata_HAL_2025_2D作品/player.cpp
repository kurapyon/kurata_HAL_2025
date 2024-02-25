//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "enemy.h"
#include "collision.h"
#include "bullet.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/player.obj"			// 読み込むモデル名

#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(7.0f)							// プレイヤーの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER				g_Player[MAX_PLAYER];			// プレイヤー


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		LoadModel(MODEL_PLAYER, &g_Player[i].model);
		g_Player[i].load = true;

		g_Player[i].pos = { 0.0f, PLAYER_OFFSET_Y, 0.0f };
		g_Player[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Player[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Player[i].spd = 0.0f;			// 移動スピードクリア
		g_Player[i].size = PLAYER_SIZE;	// 当たり判定の大きさ

		g_Player[i].use = true;

		// ここでプレイヤー用の影を作成している
		XMFLOAT3 pos = g_Player[i].pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		g_Player[i].shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
		//          ↑
		//        このメンバー変数が生成した影のIndex番号


	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		// モデルの解放処理
		if (g_Player[i].load)
		{
			UnloadModel(&g_Player[i].model);
			g_Player[i].load = false;
		}

	}


}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		// 移動させちゃう
		if (GetKeyboardPress(DIK_LEFT))
		{	// 左へ移動
			g_Player[i].spd = VALUE_MOVE;
			g_Player[i].dir = XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_RIGHT))
		{	// 右へ移動
			g_Player[i].spd = VALUE_MOVE;
			g_Player[i].dir = -XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_UP))
		{	// 上へ移動
			g_Player[i].spd = VALUE_MOVE;
			g_Player[i].dir = XM_PI;
		}
		if (GetKeyboardPress(DIK_DOWN))
		{	// 下へ移動
			g_Player[i].spd = VALUE_MOVE;
			g_Player[i].dir = 0.0f;
		}

		// 弾発射
		if (GetKeyboardTrigger(DIK_SPACE))
		{
			XMFLOAT3 pos = g_Player->pos;
			XMFLOAT3 rot = g_Player->rot;

			SetBullet(pos, rot);

		}

#ifdef _DEBUG
		if (GetKeyboardPress(DIK_R))
		{
			g_Player[i].pos.z = g_Player[i].pos.x = 0.0f;
			g_Player[i].rot.y = g_Player[i].dir = 0.0f;
			g_Player[i].spd = 0.0f;
		}
#endif


		//	// Key入力があったら移動処理する
		if (g_Player[i].spd > 0.0f)
		{
			g_Player[i].rot.y = g_Player[i].dir + cam->rot.y;

			// 入力のあった方向へプレイヤーを向かせて移動させる
			g_Player[i].pos.x -= sinf(g_Player[i].rot.y) * g_Player[i].spd;
			g_Player[i].pos.z -= cosf(g_Player[i].rot.y) * g_Player[i].spd;



			// 移動が終わったらエネミーとの当たり判定
			ENEMY* enemy = GetEnemy();
			for (int j = 0; j < MAX_ENEMY; j++)
			{
				// もしエネミーが生きていたら
				if (enemy[j].use == TRUE)
				{
					BOOL ans = CollisionBC(
						g_Player[i].pos,
						enemy[j].pos,
						g_Player[i].size,
						enemy[j].size);

					// 当たった時の処理
					if (ans == TRUE)
					{
						enemy[j].use = FALSE;

						// 影を消している
						ReleaseShadow(enemy[j].shadowIdx);

						break;
					}
				}
			}

			// 慣性を入れている
			g_Player[i].spd *= 0.5f;

		}

		// 影もプレイヤーの位置に合わせる
		XMFLOAT3 pos = g_Player[i].pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		SetPositionShadow(g_Player[i].shadowIdx, pos);




		//{	// ポイントライトのテスト
		//	LIGHT *light = GetLightData(1);
		//	XMFLOAT3 pos = g_Player[i].pos;
		//	pos.y += 20.0f;

		//	light->Position = pos;
		//	light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		//	light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		//	light->Type = LIGHT_TYPE_POINT;
		//	light->Enable = true;
		//	SetLightData(1, light);
		//}




#ifdef _DEBUG	// デバッグ情報を表示する
		PrintDebugProc("Player:↑ → ↓ ←　Space\n");
		PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
#endif

	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Player[i].scl.x, g_Player[i].scl.y, g_Player[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Player[i].model);

	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player[0];
}

