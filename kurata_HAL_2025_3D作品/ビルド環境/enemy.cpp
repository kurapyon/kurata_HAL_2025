//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "shadow.h"
#include "player.h"
#include "collision.h"
#include "fade.h"
#include "bullet.h"
#include "enemybullet.h"
#include "enemyatc.h"
#include "particle.h"
#include "particlesphere.h"
#include "particleatc.h"
#include "sound.h"
#include "keyframe.h"
#include "bossbullet.h"
#include "enemybossbullet .h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// モデルの読み込み
#define MODEL_ENEMY			"data/MODEL/ENEMY/enemy/enemy01.obj"
#define MODEL_TEMPLE		"data/MODEL/ENEMY/enemy/temple.obj"

// ボス
#define MODEL_ENEMYBOSS		"data/MODEL/ENEMY/boss/ENEMY_BOSS.obj"
#define MODEL_BOSSRING		"data/MODEL/ENEMY/boss/ENEMY_RING.obj"


// エネミー
#define	VALUE_MOVE					(2.0f)				// 移動量
#define	VALUE_ROTATE				(XM_PI * 0.02f)		// 回転量
#define VALUE_SIZE					(5.0f)				// スケール
#define ENEMY_SHADOW_SIZE			(0.4f)				// 影の大きさ
#define ENEMY_OFFSET_Y				(30.0f)				// エネミーの足元をあわせる
// 攻撃カウント
#define ENEMY_USUALLY_CNTLOWER		(1000)				// 通常攻撃のカウント
#define ENEMY_USUALLY_CNTUPPER		(2000)				// 通常攻撃のカウント
// 被ダメージ
#define ENEMY_RECEIVEWAIT			(180)				// 被ダメージ時の色変更時間
// dissolve
#define ENEMY_DISSOLVE				(1.0f)

// ボス
#define	VALUE_BOSSMOVE				(2.0f)				// 移動量
#define	VALUE_BOSSROTATE			(XM_PI * 0.02f)		// 回転量
#define VALUE_BOSSSIZE				(5.0f)				// スケール
#define ENEMYBOSS_SHADOW_SIZE		(0.4f)				// 影の大きさ
#define ENEMYBOSS_SPEED				(3.0f)				// ボスの移動スピード
#define ENEMYBOSS_OFFSET_Y			(7.0f)				// エネミーの足元をあわせる
#define ENEMYBOSS_PARTS_MAX			(2)					// プレイヤーのパーツの数
// 攻撃カウント
#define ENEMYBOSS_USUALLY_CNTLOWER	(1000)			// 通常攻撃のカウント
#define ENEMYBOSS_USUALLY_CNTUPPER	(2000)			// 通常攻撃のカウント
// 被ダメージ
#define ENEMBOSS_RECEIVEWAIT		(180)			// 被ダメージ時の色変更時間
#define ENEMYBOSS_DMGCNT			(10.0f)			// ダメージカウント
// dissolve
#define ENEMYBOSS_DISSOLVE			(1.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// エネミー処理関数
void INITENEMY(void);
void UPDATEENEMY(void);
void DRAWENEMY(void);

// ボス処理関数
void INITBOSS(void);
void UPDATEBOSS(void);
void DRAWBOSS(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];						// エネミー

static ENEMYBOSS		g_EnemyBoss;							// エネミーボス
static ENEMYBOSS		g_BossParts[ENEMYBOSS_PARTS_MAX];		// ボスのパーツ用

int g_Enemy_load = 0;
int g_EnemyBoss_load = 0;

int bosscnt = 0;
BOOL check = 0;

//　エネミー
static INTERPOLATION_DATA move_tbl_enemy[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,  30.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,		   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  40.0f, 400.0f), XMFLOAT3(0.0f, XM_PI / 4,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  50.0f, 400.0f), XMFLOAT3(0.0f, XM_PI / 2,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  60.0f, 400.0f), XMFLOAT3(0.0f, XM_PI ,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  70.0f, 400.0f), XMFLOAT3(0.0f, XM_PI * 1.25f,0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  80.0f, 400.0f), XMFLOAT3(0.0f, XM_PI * 1.5f, 0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  30.0f, 400.0f), XMFLOAT3(0.0f, XM_2PI,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE),   1 },
};

static INTERPOLATION_DATA move_tbl_temple[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,		  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_PI / 2,	  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_PI,		  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_PI * 1.5f,  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_2PI,	      0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f),   1 },
};

// ボス
// 出現時
static INTERPOLATION_DATA move_tbl_enemyboss0[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,   10.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,	   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE), 60 },

};

// 1回目の攻撃
static INTERPOLATION_DATA move_tbl_enemyboss1[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,   100.0f,  400.0f), XMFLOAT3(0.0f, 0.0f,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(-400.0f, 100.0f, -400.0f), XMFLOAT3(0.0f, XM_PIDIV2,	   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(-750.0f,  50.0f, -750.0f), XMFLOAT3(0.0f, XM_PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3( 750.0f, 50.0f, -750.0f), XMFLOAT3(0.0f, XM_PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3( 400.0f, 50.0f, -400.0f), XMFLOAT3(0.0f, XM_PI * 1.5f,  0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(0.0f,   100.0f,  400.0f), XMFLOAT3(0.0f, XM_2PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(0.0f,    100.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),    1 },

};

// 2回目の攻撃
static INTERPOLATION_DATA move_tbl_enemyboss2[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,   100.0f,  400.0f), XMFLOAT3(0.0f, 0.0f,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(400.0f, 100.0f, -400.0f), XMFLOAT3(0.0f, XM_PIDIV2,	   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(750.0f,  50.0f, -750.0f), XMFLOAT3(0.0f, XM_PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(-750.0f, 50.0f, -750.0f), XMFLOAT3(0.0f, XM_PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(-400.0f, 50.0f, -400.0f), XMFLOAT3(0.0f, XM_PI * 1.5f,  0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(0.0f,   100.0f,  400.0f), XMFLOAT3(0.0f, XM_2PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(0.0f,    100.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),    1 },
};														 

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_enemy,
	move_tbl_temple,
	move_tbl_enemyboss0,
	move_tbl_enemyboss1,
	move_tbl_enemyboss2,

};

enum 
{
	tbl_enemy,
	tbl_temple,
	tbl_enemyboss0,
	tbl_enemyboss1,
	tbl_enemyboss2,
};

static INTERPOLATION_DATA move_tbl_boss[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },


};

static INTERPOLATION_DATA move_tbl_ring1[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI,         0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f,         XM_PI, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI,         0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f,        XM_2PI, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI  * 3.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI,         0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_2PI * 2.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f,          0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f),   1 },
};


static INTERPOLATION_DATA* g_AnimTblAdr[] =
{
	move_tbl_boss,
	move_tbl_ring1,
};

enum ANIM
{
	tbl_boss,
	tbl_ring1,
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{

	// エネミー
	INITENEMY();

	// ボス
	INITBOSS();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}
	// パーツの解放処理
	for (int i = 0; i < ENEMYBOSS_PARTS_MAX; i++)
	{
		if (g_BossParts[i].load == TRUE)
		{
			// パーツの解放処理
			UnloadModel(&g_BossParts[i].model);
			g_BossParts[i].load = FALSE;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{

	// エネミー
	UPDATEENEMY();

	if (g_Enemy[0].chenge == TRUE)
	{
		// ボス
		UPDATEBOSS();
	}

#ifdef _DEBUG
	
		if (GetKeyboardTrigger(DIK_P))
		{
			// モデルの色を変更できるよ！半透明にもできるよ。
			for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
			{
				SetModelDiffuse(&g_Enemy[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));
			}
		}

		if (GetKeyboardTrigger(DIK_L))
		{
			// モデルの色を元に戻している
			for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
			{
				SetModelDiffuse(&g_Enemy[0].model, j, g_Enemy[0].diffuse[j]);
			}
		}
#endif

#ifdef _DEBUG
		// デバッグ表示
		PrintDebugProc("Enemy00:%f,Enemy01:%f\n", g_Enemy[0].hp, g_Enemy[1].hp);
		PrintDebugProc("ENEMYBOSS pos.x:%f,pos.y:%f,pos.z:%f\n", g_EnemyBoss.pos.x, g_EnemyBoss.pos.y, g_EnemyBoss.pos.z);
#endif
	
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{


	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// エネミー
	DRAWENEMY();

	if (g_Enemy[0].chenge == TRUE)
	{
		// ボス
		DRAWBOSS();
	}

	SetFuchi(0);

}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

//=============================================================================
// ボスの取得
//=============================================================================
ENEMYBOSS* GetEnemyBoss()
{
	return &g_EnemyBoss;
}


//=============================================================================
// エネミーの処理関数
//=============================================================================
void INITENEMY(void)
{
	LoadModel(MODEL_ENEMY, &g_Enemy[0].model);
	LoadModel(MODEL_TEMPLE, &g_Enemy[1].model);

	for (int i = 0; i < MAX_ENEMY; i++)
	{

		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 0.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Enemy[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		//GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

		g_Enemy[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Enemy[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Enemy[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		g_Enemy[i].use = TRUE;			// TRUE:生きてる
		g_Enemy[i].chek = FALSE;		// 
		g_Enemy[i].cnt = 0;				// 攻撃回数をカウントする

		// 被ダメージ時のダメージ時の変更時間
		g_Enemy[i].receivewait = 0.0f;

		//// 当たり判定のチェック
		//g_Enemy[i].ans = FALSE;

		// dissolve
		g_Enemy[i].dissolve.Enable = TRUE;
		g_Enemy[i].dissolve.threshold = 1.0f;
	}

	// 初期座標
	g_Enemy[0].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 400.0f);
	g_Enemy[1].pos = XMFLOAT3(0.0f, 1.0f, 400.0f);

	// スケール
	g_Enemy[0].scl = XMFLOAT3(2.0f, 2.0f, 2.0f);
	g_Enemy[1].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	// ライフ
	g_Enemy[0].hp = ENEMY_HP;
	g_Enemy[1].hp = TEMPLE_HP;

	// ボス戦移行
	g_Enemy[0].chenge = FALSE;


	// 影
	XMFLOAT3 pos = g_Enemy[0].pos;
	pos.y -= (ENEMY_OFFSET_Y - 0.1f);
	g_Enemy[0].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

	//// 0番だけ線形補間で動かしてみる
	g_Enemy[0].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[0].tblNo = tbl_enemy;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[0].tblMax = sizeof(move_tbl_enemy) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	//// 1番だけ線形補間で動かしてみる
	g_Enemy[1].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[1].tblNo = tbl_temple;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[1].tblMax = sizeof(move_tbl_temple) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

}

void UPDATEENEMY(void)
{
	BULLET* bullet = GetBullet();
	ENEMYATC* enemyatc = GetEnemyAtc();
	PLAYER* player = GetPlayer();

	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// プレイヤーが開始位置についたら登場させる
		if (player->chek == FALSE) continue;

		// dissolve
		g_Enemy[i].dissolve.threshold -= 0.01f;
		g_EnemyBoss.dissolve.threshold -= 0.01f;
		if (g_Enemy[i].dissolve.threshold <= 0.0f && g_EnemyBoss.dissolve.threshold <= 0.0f)
		{
			g_Enemy[i].chek = TRUE;
		}

		// dissolveで出現させる
		if (g_Enemy[i].chek == FALSE)	continue;

		if (g_Enemy[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes

			// 移動処理
			if (g_Enemy[i].tblMax > 0)	// 線形補間を実行する？
			{	// 線形補間の処理
				int nowNo = (int)g_Enemy[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Enemy[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Enemy[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);

				// frameを使て時間経過処理をする
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Enemy[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Enemy[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				}

			}

			// 影もプレイヤーの位置に合わせる
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);

			// 通常攻撃処理

			XMFLOAT3 epos = g_Enemy[0].pos;
			XMFLOAT3 erot = g_Enemy[0].rot;

			SetEnemyBullet(epos, erot);

			//// エネミー1番目が倒されるかつ、攻撃カウントが一定数の間実行する
			//g_Enemy[0].cnt++;
			//if (g_Enemy[1].use == FALSE
			//	&& ENEMY_USUALLY_CNTLOWER <= g_Enemy[0].cnt
			//	&& g_Enemy[0].cnt <= ENEMY_USUALLY_CNTUPPER)
			//{
			//	// 必殺技
			//	// パーティクル
			//	SetParticleSphere();

			//	XMFLOAT3 bpos = g_Enemy[0].pos;
			//	XMFLOAT3 brot = g_Enemy[0].rot;

			//	SetEnemyAtc(bpos, brot);

			//}

			//// 攻撃カウントの初期化
			//if (g_Enemy[0].cnt > ENEMY_USUALLY_CNTUPPER)
			//{
			//	g_Enemy[0].cnt = 0;
			//}

		}
	}

	// エネミーを倒したらボス戦へ
	if (g_Enemy[0].hp <= 0.0f)
	{
		g_Enemy[0].chenge = TRUE;
	}

}

void DRAWENEMY(void)
{
	PLAYER* player = GetPlayer();
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//// dissolve
		SetDissolve(&g_Enemy[i].dissolve);
		SetAlphaTestEnable(g_Enemy[i].dissolve.Enable);

		// プレイヤーが開始位置についたら登場させる
		if (player->chek == FALSE) continue;


		if (g_Enemy[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		//// モデル描画
		DrawModel(&g_Enemy[i].model);

	}

	//// dissolve 描画後にFALSEに
	SetDissolveEnable(FALSE);
	SetAlphaTestEnable(g_Enemy[0].dissolve.Enable);
	SetAlphaTestEnable(g_Enemy[1].dissolve.Enable);


}


// ボス処理関数
void INITBOSS(void)
{

	g_EnemyBoss.pos = XMFLOAT3(0.0f, ENEMYBOSS_OFFSET_Y, 400.0f);
	g_EnemyBoss.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_EnemyBoss.hp = ENEMYBOSS_HP;
	g_EnemyBoss.spd = 0.0f;				// 移動スピードクリア
	g_EnemyBoss.size = ENEMYBOSS_SIZE;	// 当たり判定の大きさ
	g_EnemyBoss.use = FALSE;			// TRUE:生きてる

	g_EnemyBoss.time = 0.0f;		// 線形補間用のタイマーをクリア
	g_EnemyBoss.tblNo = 0;			// 再生する行動データテーブルNoをセット
	g_EnemyBoss.tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

	g_EnemyBoss.chek = FALSE;		// 
	g_EnemyBoss.cnt = 0;			// 攻撃回数をカウントする
	g_EnemyBoss.dmgcnt = 0.0f;		// ダメージをカウント

	// 被ダメージ時のダメージ時の変更時間
	g_EnemyBoss.receivewait = 0.0f;

	//// 当たり判定のチェック
	//g_EnemyBoss[i].ans = FALSE;

	// dissolve
	g_EnemyBoss.dissolve.Enable = TRUE;
	g_EnemyBoss.dissolve.threshold = 1.0f;

	//// 0番だけ線形補間で動かしてみる
	g_EnemyBoss.time = 0.0f;		// 線形補間用のタイマーをクリア
	g_EnemyBoss.tblNo = tbl_enemyboss0;		// 再生するアニメデータの先頭アドレスをセット
	g_EnemyBoss.tblMax = sizeof(move_tbl_enemyboss0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット


	//階層アニメーションの初期化
	for (int j = 0; j < ENEMYBOSS_PARTS_MAX; j++)
	{
		g_BossParts[j].use = FALSE;

		// 位置・回転・スケールの初期設定
		g_BossParts[j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BossParts[j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BossParts[j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// 親子関係
		g_BossParts[j].parent = &g_EnemyBoss;		// ← ここに親のアドレスを入れる
	//	g_BossParts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
	//	g_BossParts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

		// 階層アニメーション用のメンバー変数の初期化
		g_BossParts[j].time   = 0.0f;			// 線形補間用のタイマーをクリア
		g_BossParts[j].tblNo  = 0;			// 再生する行動データテーブルNoをセット
		g_BossParts[j].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		// パーツの読み込みはまだしていない
		g_BossParts[j].load = FALSE;

		// モデルのディフューズを保存しておく。色変え対応の為。
		//GetModelDiffuse(&g_BossParts[j].model, &g_BossParts[j].diffuse[0]);

	}

	// キーフレーム変更部分
	g_BossParts[0].use = TRUE;
	g_BossParts[0].parent = &g_EnemyBoss;	// 親をセット
	g_BossParts[0].tblNo = tbl_boss;			// 再生するアニメデータの先頭アドレスをセット
	g_BossParts[0].tblMax = sizeof(move_tbl_boss) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	//g_BossParts[0].tblMax = (int)g_TblAdr[g_BossParts[0].tblNo]->size();	// ← 変更部分
	g_BossParts[0].load = TRUE;
	LoadModel(MODEL_ENEMYBOSS, &g_BossParts[0].model);

	// キーフレーム変更部分
	g_BossParts[1].use = TRUE;
	g_BossParts[1].parent = &g_BossParts[0];	// 親をセット
	g_BossParts[1].tblNo = tbl_ring1;			// 再生するアニメデータの先頭アドレスをセット
	g_BossParts[1].tblMax = sizeof(move_tbl_ring1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	//g_BossParts[1].tblMax = (int)g_TblAdr[g_BossParts[1].tblNo]->size();	// ← 変更部分
	g_BossParts[1].load = TRUE;
	LoadModel(MODEL_BOSSRING, &g_BossParts[1].model);

}

void UPDATEBOSS(void)
{
	PLAYER* player = GetPlayer();

	// TRUE:生きてる
	g_EnemyBoss.use = TRUE;	

	g_EnemyBoss.oldpos = g_EnemyBoss.pos;

	XMFLOAT3 changepos = XMFLOAT3(0.0f,100.0f,400.0f);


	// エネミーの行動パターン
	if (   g_EnemyBoss.pos.x == changepos.x
		&& g_EnemyBoss.pos.y == changepos.y
		&& g_EnemyBoss.pos.z == changepos.z)
	{
		SetSepia(0);

		bosscnt += 2;
		check = bosscnt % 2;
		// ダメージをカウントを初期化
		g_EnemyBoss.dmgcnt = 0.0f;
	}

	switch (check)
	{
	case 0:
		g_EnemyBoss.tblNo = tbl_enemyboss2;		// 再生するアニメデータの先頭アドレスをセット
		g_EnemyBoss.tblMax = sizeof(move_tbl_enemyboss2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		// ベジェ曲線
		XMFLOAT3 bpos = g_EnemyBoss.pos;
		bpos.y += 5.0f;
		SetBossBullet(bpos, g_EnemyBoss.rot, player->pos);

		SetSepia(1);

		// 模試ダメージカウントが超えたならボスの攻撃を止める
		if (g_EnemyBoss.dmgcnt >= ENEMYBOSS_DMGCNT)
		{
			break;
		}

		break;

	case 1:
		g_EnemyBoss.tblNo  = tbl_enemyboss1;		// 再生するアニメデータの先頭アドレスをセット
		g_EnemyBoss.tblMax = sizeof(move_tbl_enemyboss1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		// ベジェ曲線
		bpos = g_EnemyBoss.pos;
		bpos.y += 5.0f;
		SetBossBullet(bpos, g_EnemyBoss.rot, player->pos);

		// 模試ダメージカウントが超えたならボスの攻撃を止める
		if (g_EnemyBoss.dmgcnt >= ENEMYBOSS_DMGCNT)
		{
			break;
		}

		break;

	default:
		break;
	}

	// 移動処理
	if (g_EnemyBoss.tblMax > 0)	// 線形補間を実行する？
	{	// 線形補間の処理
		int nowNo = (int)g_EnemyBoss.time;			// 整数分であるテーブル番号を取り出している
		int maxNo = g_EnemyBoss.tblMax;				// 登録テーブル数を数えている
		int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
		INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_EnemyBoss.tblNo];	// 行動テーブルのアドレスを取得

		XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
		XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
		XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

		XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
		XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
		XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

		float nowTime = g_EnemyBoss.time - nowNo;	// 時間部分である少数を取り出している

		Pos *= nowTime;								// 現在の移動量を計算している
		Rot *= nowTime;								// 現在の回転量を計算している
		Scl *= nowTime;								// 現在の拡大率を計算している

		// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
		XMStoreFloat3(&g_EnemyBoss.pos, nowPos + Pos);

		// 計算して求めた回転量を現在の移動テーブルに足している
		XMStoreFloat3(&g_EnemyBoss.rot, nowRot + Rot);

		// 計算して求めた拡大率を現在の移動テーブルに足している
		XMStoreFloat3(&g_EnemyBoss.scl, nowScl + Scl);

		// frameを使て時間経過処理をする
		g_EnemyBoss.time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
		if ((int)g_EnemyBoss.time >= maxNo)			// 登録テーブル最後まで移動したか？
		{
			g_EnemyBoss.time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
		}
	}

	// 階層アニメーション
	for (int j = 0; j < ENEMYBOSS_PARTS_MAX; j++)
	{
		// 使われているなら処理する
		if ((g_BossParts[j].use == TRUE) && (g_BossParts[j].tblMax > 0))
		{	// 線形補間の処理
			int nowNo = (int)g_BossParts[j].time;			// 整数分であるテーブル番号を取り出している
			int maxNo = g_BossParts[j].tblMax;				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
			INTERPOLATION_DATA* tbl = g_AnimTblAdr[g_BossParts[j].tblNo];	// 行動テーブルのアドレスを取得

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			float nowTime = g_BossParts[j].time - nowNo;	// 時間部分である少数を取り出している

			Pos *= nowTime;								// 現在の移動量を計算している
			Rot *= nowTime;								// 現在の回転量を計算している
			Scl *= nowTime;								// 現在の拡大率を計算している

			// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
			XMStoreFloat3(&g_BossParts[j].pos, nowPos + Pos);

			// 計算して求めた回転量を現在の移動テーブルに足している
			XMStoreFloat3(&g_BossParts[j].rot, nowRot + Rot);

			// 計算して求めた拡大率を現在の移動テーブルに足している
			XMStoreFloat3(&g_BossParts[j].scl, nowScl + Scl);

			// frameを使て時間経過処理をする
			g_BossParts[j].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			if ((int)g_BossParts[j].time >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				g_BossParts[j].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる

			}
		}
	}
}

void DRAWBOSS(void)
{
	if (g_EnemyBoss.use == FALSE) return;
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// dissolve
	SetDissolve(&g_EnemyBoss.dissolve);
	SetAlphaTestEnable(g_EnemyBoss.dissolve.Enable);

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_EnemyBoss.scl.x, g_EnemyBoss.scl.y, g_EnemyBoss.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_EnemyBoss.rot.x, g_EnemyBoss.rot.y + XM_PI, g_EnemyBoss.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//// クォータニオンを反映
	//quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_EnemyBoss.Quaternion));
	//mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_EnemyBoss.pos.x, g_EnemyBoss.pos.y, g_EnemyBoss.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_EnemyBoss.mtxWorld, mtxWorld);

	// モデル描画
	DrawModel(&g_EnemyBoss.model);

	// dissolve 描画後にFALSEに
	SetDissolveEnable(FALSE);
	SetAlphaTestEnable(g_EnemyBoss.dissolve.Enable);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);


	// 階層アニメーション
	for (int i = 0; i < ENEMYBOSS_PARTS_MAX; i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_BossParts[i].scl.x, g_BossParts[i].scl.y, g_BossParts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_BossParts[i].rot.x, g_BossParts[i].rot.y, g_BossParts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_BossParts[i].pos.x, g_BossParts[i].pos.y, g_BossParts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_BossParts[i].parent != NULL)	// 子供だったら親と結合する
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_BossParts[i].parent->mtxWorld));
			// ↑
			// g_Player.mtxWorldを指している
		}

		XMStoreFloat4x4(&g_BossParts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する
		if (g_BossParts[i].use == FALSE) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// モデル描画
		DrawModel(&g_BossParts[i].model);
	}
}

