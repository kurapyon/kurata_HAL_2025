//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "tree.h"
#include "bullet.h"
#include "enemybullet.h"
#include "enemyatc.h"
#include "particle.h"
#include "particlesphere.h"
#include "particleatc.h"
#include "collision.h"
#include "debugproc.h"
#include "sky.h"
#include "ui.h"
#include "sound.h"
#include "particleeffect.h"
#include "bossbullet.h"
#include "enemybossbullet .h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

// TPS視点の座標用
#define		PERSPECTIVE_X		(20)
#define		PERSPECTIVE_Y		(35)
#define		PERSPECTIVE_Z		(-30)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHit(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// ポーズON/OFF


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 150, 13.0f, 13.0f);

	// ライトを有効化	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	// 壁の初期化
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	//// 壁(裏側用の半透明)
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// 木を生やす
	InitTree();

	// 弾の初期化
	InitBullet();
	InitEnemyBullet();
	InitEnemyAtc();
	InitBossBullet();
	InitEnemyBossBullet();

	// UIの初期化
	InitUi();

	// パーティクルの初期化
	InitParticle();
	InitParticleSphere();
	InitParticleAtc();
	InitParticleEffect();

	// 空の初期化
	InitSky();

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_enemy_bgm);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	// パーティクルの終了処理
	UninitParticle();
	UninitParticleSphere();
	UninitParticleAtc();
	UninitParticleEffect();

	//// UIの終了処理
	UninitUi();

	// 弾の終了処理
	UninitBullet();
	UninitEnemyBullet();
	UninitEnemyAtc();
	UninitBossBullet();
	UninitEnemyBossBullet();

	// 木の終了処理
	UninitTree();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

	// 空の終了処理
	UninitSky();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;

	
	// モノクロ
	//SetMonochrome(1);

	// プレイヤーの更新処理
	UpdatePlayer();

	// 地面処理の更新
	UpdateMeshField();

	// エネミーの更新処理
	UpdateEnemy();

	// 壁処理の更新
	UpdateMeshWall();

	// 木の更新処理
	UpdateTree();

	// 弾の更新処理
	UpdateBullet();
	UpdateEnemyBullet();
	UpdateEnemyAtc();
	UpdateBossBullet();
	UpdateEnemyBossBullet();

	// パーティクルの更新処理
	UpdateParticle();
	UpdateParticleSphere();
	UpdateParticleAtc();
	UpdateParticleEffect();

	// 影の更新処理
	UpdateShadow();

	// 当たり判定
	CheckHit();

	// スコアの更新処理
	UpdateUi();

	// 空の更新処理
	UpdateSky();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// 3Dの物を描画する処理
	
	// 空の描画処理
	DrawSky();

	// 地面の描画処理
	DrawMeshField();

	// 影の描画処理
	DrawShadow();

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	// 壁の描画処理
	DrawMeshWall();

	// 木の描画処理
	DrawTree();

	// パーティクルの描画処理
	DrawParticle();
	DrawParticleSphere();
	DrawParticleEffect();

#ifndef _DEBUG

	DrawParticleAtc();

#endif 
	// 弾の描画処理
	DrawBullet();
	DrawBossBullet();
	//DrawEnemyAtc();
	DrawEnemyBullet();
	DrawEnemyBossBullet();

	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	// スコアの描画処理
	DrawUi();

	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);


}

void DrawGame(void)
{
	CAMERA* cam = GetCamera();
	PLAYER* player = GetPlayer();
	XMFLOAT3 pos;


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// TPS視点
	{	// プレイヤーの注視点の取得
		// ワールドマトリックスの初期化
		// 正方行列の初期化
		XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
		mtxWorld = XMMatrixIdentity();

		// アフィン変換
		// 移動を反映
		mtxTranslate = XMMatrixTranslation(PERSPECTIVE_X, PERSPECTIVE_Y, PERSPECTIVE_Z);// 右ベクトル,
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(cam->rot.x, cam->rot.y, cam->rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, mtxWorld);

		// ベクトル
		XMFLOAT3 vector;
		// 行列の要素を取り出してベクトルに足す
		vector.x = result._41;
		vector.y = result._42;
		vector.z = result._43;

		// プレイヤーの座標を足す
		vector.x += player->pos.x;
		vector.y += player->pos.y;
		vector.z += player->pos.z;

		SetCameraAT(vector);
	}

	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		//SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		//SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}
}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	// キャラクター関係
	ENEMY *enemy = GetEnemy();						// エネミーのポインターを初期化
	ENEMYBOSS* enemyboss = GetEnemyBoss();			// ボスのポインターの初期化
	PLAYER *player = GetPlayer();					// プレイヤーのポインターを初期化

	// 弾関係
	BULLET *bullet = GetBullet();					// 弾のポインターを初期化
	ENEMYBULLET* enemybullet = GetEnemyBullet();	// エネミーの弾のポインターを初期化
	ENEMYATC* enemyatc = GetEnemyAtc();				// エネミーアタックのポインターの初期化
	BOSSBULLET* bossbullet = GetBossBullet();		// ベジェ曲線バレットの初期化


	//プレイヤーの弾と敵
	{
		for (int i = 0; i < MAX_BULLET; i++)
		{
			//弾の有効フラグをチェックする
			if (bullet[i].use == FALSE)
				continue;

			XMFLOAT3 epos = enemy[1].pos;
			epos.y *= 15.0f;

			//BCの当たり判定(エネミー1番目)
			if (CollisionBC(bullet[i].pos, epos, bullet[i].fWidth, enemy[1].size))
			{

				// エフェクト用座標
				for (int p = 0; p < 20; p++)
				{
					XMFLOAT3 move = CalcParticleSparkMove(bullet[i].pos);
					SetParticleEffect(bullet[i].pos, move, PARTICLE_SPARK_LIFE, TRUE);
				}

				// 効果音
				PlaySound(SOUND_LABEL_SE_hit);

				// 当たったから未使用に戻す
				bullet[i].use = FALSE;
				ReleaseShadow(bullet[i].shadowIdx);

				// エネミーのライフを減らす
				enemy[1].hp -= 1.0f;
				enemy[1].dissolve.threshold += 0.1f;

				if (enemy[1].hp <= 0.0f)
				{
					// 敵キャラクターは倒される
					enemy[1].use = FALSE;
					ReleaseShadow(enemy[1].shadowIdx);
				}
			}

			// エネミー1番目が倒されたら0番目と当たり判定
			if (enemy[1].use == FALSE)
			{
				//BCの当たり判定
				if (CollisionBC(bullet[i].pos, enemy[0].pos, bullet[i].fWidth, enemy[0].size))
				{
					// 当たったから未使用に戻す
					bullet[i].use = FALSE;
					ReleaseShadow(bullet[i].shadowIdx);

					// 効果音
					PlaySound(SOUND_LABEL_SE_hit);

					// エフェクト用座標
					for (int p = 0; p < 20; p++)
					{
						XMFLOAT3 move = CalcParticleSparkMove(bullet[i].pos);
						SetParticleEffect(bullet[i].pos, move, PARTICLE_SPARK_LIFE, TRUE);
					}

					// エネミーのライフを減らす
					enemy[0].hp -= 1.0f;
					enemy[0].dissolve.threshold += 0.1f;
					enemy[0].ans = TRUE;

					if (enemy[0].hp <= 0.0f)
					{
						// 敵キャラクターは倒される
						enemy[0].use = FALSE;
						ReleaseShadow(enemy[0].shadowIdx);
					}
				}
			}

			// ボスとプレイヤーの球の判定
			//BCの当たり判定
			if (enemyboss->use == TRUE)
			{
				if (CollisionBC(bullet[i].pos, enemyboss->pos, bullet[i].fWidth, enemyboss->size))
				{
					// 当たったから未使用に戻す
					bullet[i].use = FALSE;
					ReleaseShadow(bullet[i].shadowIdx);

					// 効果音
					PlaySound(SOUND_LABEL_SE_hit);

					// エフェクト用座標
					for (int p = 0; p < 20; p++)
					{
						XMFLOAT3 move = CalcParticleSparkMove(bullet[i].pos);
						SetParticleEffect(bullet[i].pos, move, PARTICLE_SPARK_LIFE, TRUE);
					}

					// ボスのライフを減らしてダメージをカウントをためる
					enemyboss->hp -= 1.0f;
					enemyboss->dmgcnt += 1.0f;
					if (enemyboss->hp <= 0.0f)
					{
						// 敵キャラクターは倒される
						enemyboss->use = FALSE;
						ReleaseShadow(enemyboss->shadowIdx);
					}

					if (enemyboss->hp <= 0.0f)
					{
						SetFadeMode(FADE_OUT, MODE_CLEAR_RESULT);
					}
				}

			}
		}
	}

	// EnemyBulletとプレイヤーの当たり判定
	{
		for (int i = 0; i < MAX_ENEMYBULLET; i++)
		{
			//弾の有効フラグをチェックする
			if (enemybullet[i].use == FALSE)
				continue;

			//プレイヤーの有効フラグをチェックする
			if (player[0].use == FALSE)
				continue;

			//BCの当たり判定
			if (CollisionBC(enemybullet[i].pos, player[0].pos, enemybullet[i].fWidth, player[0].size))
			{
				// 当たったから未使用に戻す
				enemybullet[i].use = FALSE;
				ReleaseShadow(enemybullet[i].shadowIdx);
				// プレイヤーの色変更
				player[0].ans = TRUE;


				// エネミーのライフを減らす
				player[0].hp -= 1.0f;

				if (player[0].hp <= 0.0f)
				{
					// 敵キャラクターは倒される
					player[0].use = FALSE;
					ReleaseShadow(player[0].shadowIdx);
				}
			}
		}

		// プレイヤーが全部死亡したら状態遷移
		int player_count = 0;
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			if (player[0].use == FALSE) continue;
			player_count++;
		}

		if (player_count == 0)
		{
			SetFadeMode(FADE_OUT, MODE_GAMEOVER_RESULT);
		}

	}

	// EnemyAtcとプレイヤーの当たり判定
	{
		for (int i = 0; i < ENEMYATC_MAX_BULLET; i++)
		{
			//弾の有効フラグをチェックする
			if (enemyatc[i].use == FALSE)
				continue;

			//敵の有効フラグをチェックする
			if (player[0].use == FALSE)
				continue;

			//BCの当たり判定
			if (CollisionBC(enemyatc[i].pos, player[0].pos, enemyatc[i].fWidth, player[0].size))
			{
				// 当たったから未使用に戻す
				enemyatc[i].use = FALSE;
				ReleaseShadow(enemyatc[i].shadowIdx);
				// プレイヤーの色変更
				player[0].ans = TRUE;

				// エネミーのライフを減らす
				player[0].hp -= 1.0f;

				if (player[0].hp <= 0.0f)
				{
					// 敵キャラクターは倒される
					player[0].use = FALSE;
					ReleaseShadow(player[0].shadowIdx);
				}
			}
		}

		// プレイヤーが全部死亡したら状態遷移
		int player_count = 0;
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			if (player[0].use == FALSE) continue;
			player_count++;
		}

		if (player_count == 0)
		{
			SetFadeMode(FADE_OUT, MODE_GAMEOVER_RESULT);
		}
	}

	// BossBulletとプレイヤーの当たり判定
	{
		for (int i = 0; i < ENEMYATC_MAX_BULLET; i++)
		{
			//弾の有効フラグをチェックする
			if (bossbullet[i].use == FALSE)
				continue;

			//敵の有効フラグをチェックする
			if (player[0].use == FALSE)
				continue;

			//BCの当たり判定
			if (CollisionBC(bossbullet[i].pos, player[0].pos, bossbullet[i].fWidth, player[0].size))
			{
				// 当たったから未使用に戻す
				bossbullet[i].use = FALSE;
				ReleaseShadow(enemyatc[i].shadowIdx);
				// プレイヤーの色変更
				player[0].ans = TRUE;

				// エネミーのライフを減らす
				player[0].hp -= 0.1f;

				if (player[0].hp <= 0.0f)
				{
					// 敵キャラクターは倒される
					player[0].use = FALSE;
					ReleaseShadow(player[0].shadowIdx);
				}
			}
		}

		// プレイヤーが全部死亡したら状態遷移
		int player_count = 0;
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			if (player[0].use == FALSE) continue;
			player_count++;
		}

		if (player_count == 0)
		{
			SetFadeMode(FADE_OUT, MODE_GAMEOVER_RESULT);
		}
	}
}


