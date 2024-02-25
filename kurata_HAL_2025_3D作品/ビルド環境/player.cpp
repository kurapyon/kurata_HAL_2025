//=============================================================================
//
// モデル処理 [player.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "bullet.h"
#include "debugproc.h"
#include "meshfield.h"
#include "collision.h"
#include "enemy.h"
#include "fade.h"
#include "sound.h"
#include "keyframe.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

// モデル名
#define	MODEL_PLAYER_BODY		"data/MODEL/PLAYER/Player/player_body.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_HEAD		"data/MODEL/PLAYER/Player/player_head.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_ARM_L		"data/MODEL/PLAYER/Player/player_arm_l.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_ARM_R		"data/MODEL/PLAYER/Player/player_arm_r.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_THINGS_L	"data/MODEL/PLAYER/Player/player_things_l.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_THINGS_R	"data/MODEL/PLAYER/Player/player_things_r.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_LEG_L		"data/MODEL/PLAYER/Player/player_leg_l.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_LEG_R		"data/MODEL/PLAYER/Player/player_leg_r.obj"		// 読み込むモデル名

//--------------------------------------------------------------------------------------
// キーフレーム変更部分
// アニメーションのディレクトリ
// 待機状態
#define ANIM_STAY_BODY			"data/ANIMDATA/Player_Stay/Player_Body.csv"
#define ANIM_STAY_HEAD			"data/ANIMDATA/Player_Stay/Player_Head.csv"
#define ANIM_STAY_LARM			"data/ANIMDATA/Player_Stay/Player_Arm_L.csv"
#define ANIM_STAY_RARM			"data/ANIMDATA/Player_Stay/Player_Arm_R.csv"
#define ANIM_STAY_LTHINGS		"data/ANIMDATA/Player_Stay/Player_Things_L.csv"
#define ANIM_STAY_RTHINGS		"data/ANIMDATA/Player_Stay/Player_Things_R.csv"
#define ANIM_STAY_LLEG			"data/ANIMDATA/Player_Stay/Player_Leg_L.csv"
#define ANIM_STAY_RLEG			"data/ANIMDATA/Player_Stay/Player_Leg_R.csv"

// 移動状態
#define ANIM_Walk_BODY			"data/ANIMDATA/Player_Walk/Player_Body.csv"
#define ANIM_Walk_HEAD			"data/ANIMDATA/Player_Walk/Player_Head.csv"
#define ANIM_Walk_LARM			"data/ANIMDATA/Player_Walk/Player_Arm_L.csv"
#define ANIM_Walk_RARM			"data/ANIMDATA/Player_Walk/Player_Arm_R.csv"
#define ANIM_Walk_LTHINGS		"data/ANIMDATA/Player_Walk/Player_Things_L.csv"
#define ANIM_Walk_RTHINGS		"data/ANIMDATA/Player_Walk/Player_Things_R.csv"
#define ANIM_Walk_LLEG			"data/ANIMDATA/Player_Walk/Player_Leg_L.csv"
#define ANIM_Walk_RLEG			"data/ANIMDATA/Player_Walk/Player_Leg_R.csv"

// 攻撃状態
#define ANIM_ATK_BODY			"data/ANIMDATA/Player_Atc/Player_Body.csv"
#define ANIM_ATK_HEAD			"data/ANIMDATA/Player_Atc/Player_Head.csv"
#define ANIM_ATK_LARM			"data/ANIMDATA/Player_Atc/Player_Arm_L.csv"
#define ANIM_ATK_RARM			"data/ANIMDATA/Player_Atc/Player_Arm_R.csv"
#define ANIM_ATK_LTHINGS		"data/ANIMDATA/Player_Atc/Player_Things_L.csv"
#define ANIM_ATK_RTHINGS		"data/ANIMDATA/Player_Atc/Player_Things_R.csv"
#define ANIM_ATK_LLEG			"data/ANIMDATA/Player_Atc/Player_Leg_L.csv"
#define ANIM_ATK_RLEG			"data/ANIMDATA/Player_Atc/Player_Leg_R.csv"

// ↑追加したい時はここに追加
//--------------------------------------------------------------------------------------

#define	VALUE_MOVE					(7.0f)				// 移動量
#define	VALUE_ROTATE				(D3DX_PI * 0.02f)	// 回転量

#define PLAYER_SHADOW_SIZE			(0.4f)				// 影の大きさ
#define PLAYER_OFFSET_Y				(30.0f)				// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX			(8)					// プレイヤーのパーツの数

#define GRAVITY_PLUS				(10000)				// 重力判定
#define GRAVITY_MINUS				(-100.0)			// 重力判定
#define GRAVITY						(15.0)

#define PLAYER_PARTS_SIZE			(2.0f)				// プレイヤーのパーツのサイズ

// ジャンプ処理
#define	PLAYER_JUMP_CNT_MAX			(30)				// 30フレームで着地する
#define	PLAYER_JUMP_Y_MAX			(30.0f)				// ジャンプの高さ

// 弾の発射
#define PLAYER_BULLET				(10.0f)				// 弾の発射用

// 被ダメージ
#define PLAYER_RECEIVEWAIT			(180)				// 被ダメージ時の色変更時間

// ゲーム開始位置
#define START_POS_X_1				(  22.0f)
#define START_POS_X_2				( -60.0f)
#define START_POS_Z_1				( 200.0f)
#define START_POS_Z_2				(-450.0f)

//--------------------------------------------------------------------------------------
// キーフレーム変更部分
// アニメーションのリスト
enum ANIM_LIST
{
	// 待機状態
	STAY_BODY,
	STAY_HEAD,
	STAY_LARM,
	STAY_RARM,
	STAY_LTHINGS,
	STAY_RTHINGS,
	STAY_LLEG,
	STAY_RLEG,

	// 移動状態
	WALK_BODY,
	WALK_HEAD,
	WALK_LARM,
	WALK_RARM,
	WALK_LTHINGS,
	WALK_RTHINGS,
	WALK_LLEG,
	WALK_RLEG,

	// 攻撃状態
	ATK_BODY,
	ATK_HEAD,
	ATK_LARM,
	ATK_RARM,
	ATK_LTHINGS,
	ATK_RTHINGS,
	ATK_LLEG,
	ATK_RLEG,

	// ↑追加したい時はここに追加

	ANIM_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InputWalkPlayer(void);
void InputAttackPlayer(void);

void WalkPlayer(void);

void AttackPlayer(void);

void SetTblStay(void);
void SetTblWalk(void);
void SetTblAttack(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player[MAX_PLAYER];					// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];				// プレイヤーのパーツ用

static PLAYER_MODEL g_PartModel[PLAYER_PARTS_MAX];			// プレイヤーモデル

static char g_ModelName[PLAYER_PARTS_MAX][256]
{
	MODEL_PLAYER_BODY,
	MODEL_PLAYER_HEAD,
	MODEL_PLAYER_ARM_L,
	MODEL_PLAYER_ARM_R,
	MODEL_PLAYER_THINGS_L,
	MODEL_PLAYER_THINGS_R,
	MODEL_PLAYER_LEG_L,
	MODEL_PLAYER_LEG_R,
};

static float		roty = 0.0f;

static LIGHT		g_Light;

// ジャンプ関係
static int      g_jumpCnt = 0;


// プレイヤーの階層アニメーションデータ
static char* g_AnimDir[] = 
{
	// 待機状態
	ANIM_STAY_BODY,
	ANIM_STAY_HEAD,
	ANIM_STAY_LARM,
	ANIM_STAY_RARM,
	ANIM_STAY_LTHINGS,
	ANIM_STAY_RTHINGS,
	ANIM_STAY_LLEG,
	ANIM_STAY_RLEG,

	// 移動状態
	ANIM_Walk_BODY,
	ANIM_Walk_HEAD,
	ANIM_Walk_LARM,
	ANIM_Walk_RARM,
	ANIM_Walk_LTHINGS,
	ANIM_Walk_RTHINGS,
	ANIM_Walk_LLEG,
	ANIM_Walk_RLEG,

	// 攻撃状態
	ANIM_ATK_BODY,
	ANIM_ATK_HEAD,
	ANIM_ATK_LARM,
	ANIM_ATK_RARM,
	ANIM_ATK_LTHINGS,
	ANIM_ATK_RTHINGS,
	ANIM_ATK_LLEG,
	ANIM_ATK_RLEG,
};

//--------------------------------------------------------------------------------------
// キーフレーム変更部分
// プレイヤーの階層アニメーションデータ
// 待機状態
static std::vector<INTERPOLATION_DATA> stay_body;
static std::vector<INTERPOLATION_DATA> stay_head;
static std::vector<INTERPOLATION_DATA> stay_larm;
static std::vector<INTERPOLATION_DATA> stay_rarm;
static std::vector<INTERPOLATION_DATA> stay_lthings;
static std::vector<INTERPOLATION_DATA> stay_rthings;
static std::vector<INTERPOLATION_DATA> stay_lleg;
static std::vector<INTERPOLATION_DATA> stay_rleg;

// 移動状態
static std::vector<INTERPOLATION_DATA> walk_body;
static std::vector<INTERPOLATION_DATA> walk_head;
static std::vector<INTERPOLATION_DATA> walk_larm;
static std::vector<INTERPOLATION_DATA> walk_rarm;
static std::vector<INTERPOLATION_DATA> walk_lthings;
static std::vector<INTERPOLATION_DATA> walk_rthings;
static std::vector<INTERPOLATION_DATA> walk_lleg;
static std::vector<INTERPOLATION_DATA> walk_rleg;

// 攻撃状態
static std::vector<INTERPOLATION_DATA> atk_body;
static std::vector<INTERPOLATION_DATA> atk_head;
static std::vector<INTERPOLATION_DATA> atk_larm;
static std::vector<INTERPOLATION_DATA> atk_rarm;
static std::vector<INTERPOLATION_DATA> atk_lthings;
static std::vector<INTERPOLATION_DATA> atk_rthings;
static std::vector<INTERPOLATION_DATA> atk_lleg;
static std::vector<INTERPOLATION_DATA> atk_rleg;



//--------------------------------------------------------------------------------------
// キーフレーム変更部分
// プレイヤーの階層アニメーションデータの配列
static std::vector<INTERPOLATION_DATA>* g_TblAdr[] =
{
	// 待機状態
	&stay_body,
	&stay_head,
	&stay_larm,
	&stay_rarm,
	&stay_lthings,
	&stay_rthings,
	&stay_lleg,
	&stay_rleg,

	// 移動状態
	 &walk_body,
	 &walk_head,
	 &walk_larm,
	 &walk_rarm,
	 &walk_lthings,
	 &walk_rthings,
	 &walk_lleg,
	 &walk_rleg,
	
	// 攻撃状態
	&atk_body,
	&atk_head,
	&atk_larm,
	&atk_rarm,
	&atk_lthings,
	&atk_rthings,
	&atk_lleg,
	&atk_rleg,

};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	//--------------------------------------------------------------------------------------
	// キーフレーム変更部分
	// アニメーションデータをファイルから読み込む 
	for (int i = 0; i < ANIM_MAX; i++)
	{
		LoadAnimDataCSV(g_AnimDir[i], *g_TblAdr[i]);
	}
	//--------------------------------------------------------------------------------------

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		//g_Player[i].load = TRUE;
		//LoadModel(MODEL_PLAYER, &g_Player[i].model);

		g_Player[i].pos = XMFLOAT3(22.0f, PLAYER_OFFSET_Y, -600.0f);
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Player[i].spd = 0.0f;			// 移動スピードクリア
		g_Player[i].ans = FALSE;		// 当たり判定（ダメージを受けたかどうか）
		g_Player[i].use = TRUE;			// TRUE:生きてる
		g_Player[i].size = PLAYER_SIZE;	// 当たり判定の大きさ

		// ジャンプの初期化
		g_Player[i].jump		= FALSE;
		g_Player[i].jumpCnt		= 0;
		g_Player[i].jumpY		= 0.0f;
		g_Player[i].jumpYMax	= PLAYER_JUMP_Y_MAX;
		g_Player[i].jump_pos	= 0.0f;

		// バレット
		g_Player[i].bulletX = 0.0f;

		// プレイヤーのライフ
		g_Player[i].hp = PLAYER_HP;

		// 被ダメージ時の色変更時間
		g_Player[i].receivewait = 0;

		// ゲーム開始位置判定
		g_Player[i].chek = FALSE;

		// アニメーション
		g_Player[i].state  = STATE_STAY;
		g_Player[i].setTbl = STATE_STAY;

		// 弾の残弾数
		g_Player[i].reload = PLAYER_RELOAD;

		// モデルインデックス
		g_Player[i].ModelIdx = NULL;


		// ここでプレイヤー用の影を作成している
		XMFLOAT3 pos = g_Player[i].pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		g_Player[i].shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
		//          ↑
		//        このメンバー変数が生成した影のIndex番号

		// キーを押した時のプレイヤーの向き
		roty = 0.0f;

		g_Player[i].parent = NULL;			// 本体（親）なのでNULLを入れる

		 //階層アニメーションの初期化
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			g_Parts[j].use = FALSE;

			// 位置・回転・スケールの初期設定
			g_Parts[j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// 親子関係
			g_Parts[j].parent = &g_Player[i];		// ← ここに親のアドレスを入れる
		//	g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
		//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

			// 階層アニメーション用のメンバー変数の初期化
			g_Parts[j].time = 0.0f;			// 線形補間用のタイマーをクリア
			g_Parts[j].tblNo = 0;			// 再生する行動データテーブルNoをセット
			g_Parts[j].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

			// パーツの読み込みはまだしていない
			g_PartModel[j].load = FALSE;
		}

		// キーフレーム変更部分
		g_Parts[0].use = TRUE;
		g_Parts[0].parent = &g_Player[0];	// 親をセット
		g_Parts[0].tblNo = 0;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[0].tblMax = (int)g_TblAdr[g_Parts[0].tblNo]->size();	// ← 変更部分
		//LoadModel(MODEL_PLAYER_BODY, &g_PartModel[0].model);

		// キーフレーム変更部分
		g_Parts[1].use = TRUE;
		g_Parts[1].parent = &g_Parts[0];	// 親をセット
		g_Parts[1].tblNo = 1;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[1].tblMax = (int)g_TblAdr[g_Parts[1].tblNo]->size();	// ← 変更部分
		//LoadModel(MODEL_PLAYER_HEAD, &g_PartModel[1].model);

		// キーフレーム変更部分
		g_Parts[2].use = TRUE;
		g_Parts[2].parent = &g_Parts[0];	// 親をセット
		g_Parts[2].tblNo = 2;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[2].tblMax = (int)g_TblAdr[g_Parts[2].tblNo]->size();	// ← 変更部分
		//LoadModel(MODEL_PLAYER_ARM_L, &g_PartModel[2].model);

		// キーフレーム変更部分
		g_Parts[3].use = TRUE;
		g_Parts[3].parent = &g_Parts[0];	// 親をセット
		g_Parts[3].tblNo = 3;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[3].tblMax = (int)g_TblAdr[g_Parts[3].tblNo]->size();	// ← 変更部分
		//LoadModel(MODEL_PLAYER_ARM_R, &g_PartModel[3].model);

		// キーフレーム変更部分
		g_Parts[4].use = TRUE;
		g_Parts[4].parent = &g_Parts[0];	// 親をセット
		g_Parts[4].tblNo = 4;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[4].tblMax = (int)g_TblAdr[g_Parts[4].tblNo]->size();	// ← 変更部分
		//LoadModel(MODEL_PLAYER_THINGS_L, &g_PartModel[4].model);

		// キーフレーム変更部分
		g_Parts[5].use = TRUE;
		g_Parts[5].parent = &g_Parts[0];	// 親をセット
		g_Parts[5].tblNo = 5;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[5].tblMax = (int)g_TblAdr[g_Parts[5].tblNo]->size();	// ← 変更部分
		//LoadModel(MODEL_PLAYER_THINGS_R, &g_PartModel[5].model);

		// キーフレーム変更部分
		g_Parts[6].use = TRUE;
		g_Parts[6].parent = &g_Parts[4];	// 親をセット
		g_Parts[6].tblNo = 6;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[6].tblMax = (int)g_TblAdr[g_Parts[6].tblNo]->size();	// ← 変更部分
		//LoadModel(MODEL_PLAYER_LEG_L, &g_PartModel[6].model);

		// キーフレーム変更部分
		g_Parts[7].use = TRUE;
		g_Parts[7].parent = &g_Parts[5];	// 親をセット
		g_Parts[7].tblNo = 7;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[7].tblMax = (int)g_TblAdr[g_Parts[7].tblNo]->size();	// ← 変更部分
		//LoadModel(MODEL_PLAYER_LEG_R, &g_PartModel[7].model);

		// モデルの読み込み
		for (int m = 0; m < PLAYER_PARTS_MAX; m++)
		{
			LoadModel(g_ModelName[m], &g_PartModel[m].model);
			g_PartModel[m].load = TRUE;
		}

		// クォータニオンの初期化
		XMStoreFloat4(&g_Player[i].Quaternion, XMQuaternionIdentity());
	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
		// パーツの解放処理
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_PartModel[i].load == TRUE)
			{
				// パーツの解放処理
				UnloadModel(&g_PartModel[i].model);
				g_PartModel[i].load = FALSE;
			}
		}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();
	ENEMY* enemy = GetEnemy();


	switch (g_Player[0].state)
	{
	case STATE_STAY:
		InputWalkPlayer();
		InputAttackPlayer();
		SetTblWalk();
		SetTblStay();
		break;

	case STATE_WALK:
		InputWalkPlayer();
		WalkPlayer();
		SetTblWalk();
		break;

	case STATE_ATK:
		AttackPlayer();
		SetTblAttack();
		break;

	default:
		break;
	}


	for (int i = 0; i < MAX_PLAYER; i++)
	{
		// レイキャストして足元の高さを求める・重力判定
		XMFLOAT3 HitPosition;		// 交点 地面の当たったところ
		XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
		BOOL ans = RayHitField(g_Player[i].pos, &HitPosition, &Normal);	// 当たっていたら
		BOOL gravity = TRUE;		// 重力をつけるかどうか
		float distance = 0.0f;		// 地面とプレイヤーの差

		if (ans)
		{
			distance = g_Player[i].pos.y - HitPosition.y;	// プレイヤーと地面の高さの差が出る

			// 地面に埋まらなくする
			if (distance <= GRAVITY_PLUS && distance >= GRAVITY_MINUS)
			{
				// 重力をなくす
				gravity = FALSE;
			}
		}

		if (gravity == TRUE)
		{
			// 重力を付ける
			g_Player[i].pos.y -= GRAVITY;

		}

		// もし地面より下なら元の位置に戻す
		if (distance < 0.0f && ans == TRUE)
		{
			g_Player[i].pos.y -= distance - PLAYER_OFFSET_Y;
		}

		// ジャンプ処理
		//{
		//	if (g_Player[i].jump == TRUE)
		//	{
		//		// カウントがマックスの時に「XM_PI / 2」になるようにする
		//		float angle = (XM_PI / (PLAYER_JUMP_CNT_MAX * 2)) * g_Player[i].jumpCnt;
		//		// ジャンプマックスに0~1をかけている
		//		// cosfの値が１になるようにする
		//		float y = g_Player[i].jumpYMax * cosf(XM_PI / 2 + angle);
		//		g_Player[i].jumpY = y;

		//		// 元の位置からジャンプYを引いている(改良の余地あり)
		//		g_Player[i].pos.y = g_Player[i].jump_pos - g_Player[i].jumpY;

		//		g_Player[i].jumpCnt++;
		//		if (g_Player[i].jumpCnt > PLAYER_JUMP_CNT_MAX)
		//		{
		//			g_Player[i].jump = FALSE;
		//			g_Player[i].jumpCnt = 0;
		//			g_Player[i].jumpY = 0.0f;
		//		}
		//	}
		//	else if ((g_Player[i].jump == FALSE) && (MoveJumpKeyboard()))
		//	{// ジャンプボタン押した？
		//		g_Player[i].jump = TRUE;
		//		g_Player[i].jumpCnt = 0;
		//		g_Player[i].jumpY = 0.0f;
		//		g_Player[i].jump_pos = g_Player[i].pos.y;
		//	}
		//	else // ジャンプ中は重力をかけない
		//	{
		//		if (gravity == TRUE)
		//		{
		//			// 重力を付ける
		//			g_Player[i].pos.y -= GRAVITY;

		//		}
		//	}
		//}

		// ゲーム開始
		if (START_POS_X_2 <= g_Player[i].pos.x &&
			g_Player[i].pos.x <= START_POS_X_1 &&
			START_POS_Z_2 <= g_Player[i].pos.z &&
			g_Player[i].pos.z <= START_POS_Z_1)
		{
			// ゲームをスタートさせる
			g_Player[i].chek = TRUE;
		}

		// アニメーション
		HierarchyAnimation(PLAYER_PARTS_MAX);

		// 影もプレイヤーの位置に合わせる
		XMFLOAT3 pos = g_Player[i].pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		SetPositionShadow(g_Player[i].shadowIdx, pos);

		// ポイントライトのテスト
		{
			LIGHT* light = GetLightData(1);
			XMFLOAT3 pos = g_Player[i].pos;
			//pos.y += 20.0f;

			light->Position = pos;
			light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			light->Type = LIGHT_TYPE_POINT;
			light->Enable = TRUE;
			SetLightData(1, light);
		}

		//////////////////////////////////////////////////////////////////////
		// 姿勢制御
		//////////////////////////////////////////////////////////////////////

		XMVECTOR vx, nvx, up;
		XMVECTOR quat;
		float len, angle;


		g_Player[i].UpVector = Normal;
		up = { 0.0f, 1.0f, 0.0f, 0.0f };
		vx = XMVector3Cross(up, XMLoadFloat3(&g_Player[i].UpVector));

		//垂直のベクトルを求めている
		nvx = XMVector3Length(vx);
		XMStoreFloat(&len, nvx);
		nvx = XMVector3Normalize(vx);
		//nvx = vx / len;
		angle = asinf(len);

		//quat = XMQuaternionIdentity();

		//	quat = XMQuaternionRotationAxis(nvx, angle);
		quat = XMQuaternionRotationNormal(nvx, angle);


		quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player[i].Quaternion), quat, 0.05f);
		XMStoreFloat4(&g_Player[i].Quaternion, quat);



#ifdef _DEBUG

		// テスト
		if (GetKeyboardPress(DIK_3))
		{
			g_Player[i].pos.y += 0.1f;
		}

		if (GetKeyboardPress(DIK_4))
		{
			g_Player[i].pos.y -= 0.1f;
		}

		if (GetKeyboardPress(DIK_5))
		{
			SetFadeMode(FADE_OUT, MODE_CLEAR_RESULT);
		}

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

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].use == FALSE) return;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Player[i].scl.x * PLAYER_PARTS_SIZE, g_Player[i].scl.y * PLAYER_PARTS_SIZE, g_Player[i].scl.z * PLAYER_PARTS_SIZE);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// クォータニオンを反映
		quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player[i].Quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);
		

		// 変更時間を足す
		g_Player[i].receivewait++;
		if (g_Player[i].ans == TRUE)
		{
			// 縁取りの設定
			SetFuchi(1);

			// もし、receivewaitが一定時間たったら元に戻す
			if (g_Player[i].receivewait >= PLAYER_RECEIVEWAIT)
			{
				SetFuchi(0);
				g_Player[i].ans = FALSE;
				g_Player[i].receivewait = 0;
			}
		}

		// カリング設定を戻す
		SetCullingMode(CULL_MODE_BACK);

		// 階層アニメーション
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Parts[j].scl.x, g_Parts[j].scl.y, g_Parts[j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[j].rot.x, g_Parts[j].rot.y, g_Parts[j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Parts[j].pos.x, g_Parts[j].pos.y, g_Parts[j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[j].parent != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[j].parent->mtxWorld));
				// ↑
				// g_Player[i].mtxWorldを指している
			}

			XMStoreFloat4x4(&g_Parts[j].mtxWorld, mtxWorld);

			// 使われているなら処理する
			if (g_Parts[j].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);
				
			DrawModel(&g_PartModel[j].model);
		}

		SetFuchi(0);

	}
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
		return &g_Player[0];

}

BOOL AnyKeyPress(void)
{
	// どれかが押されたら
	return (MoveLeftKeyboard() || MoveRightKeyboard() || MoveUpKeyboard() || MoveDownKeyboard());

}

// 左移動
BOOL MoveLeftKeyboard(void)
{
	return GetKeyboardPress(DIK_LEFT);
}

// 右移動
BOOL MoveRightKeyboard(void)
{
	return GetKeyboardPress(DIK_RIGHT);

}

// 上移動
BOOL MoveUpKeyboard(void)
{
	return GetKeyboardPress(DIK_UP);
}

// 下移動
BOOL MoveDownKeyboard(void)
{
	return GetKeyboardPress(DIK_DOWN);
}

// ジャンプ
BOOL MoveJumpKeyboard(void)
{
	return GetKeyboardTrigger(DIK_SPACE);
}


//=============================================================================
// 左スティックの入力処理
//=============================================================================

// どれかが押されたら
BOOL AnyStickpress(void)
{
	return (MoveLeftStick() || MoveRightStick() || MoveUpStick() || MoveDownStick());

}

// 左移動
BOOL MoveLeftStick(void)
{
	return IsButtonPressed(0, L_STICK_LEFT);
}

// 右移動
BOOL MoveRightStick(void)
{
	return IsButtonPressed(0, L_STICK_RIGHT);
}

// 上移動
BOOL MoveUpStick(void)
{
	return IsButtonPressed(0, L_STICK_UP);
}

// 下移動
BOOL MoveDownStick(void)
{
	return IsButtonPressed(0, L_STICK_DOWN);
}

//=============================================================================
// 右スティックの入力処理
//=============================================================================

// どれかが押されたら
BOOL CameraAnyStickpress(void)
{
	return (CameraLeftStick() || CameraRightStick() || CameraUpStick() || CameraDownStick());

}

// 左移動
BOOL CameraLeftStick(void)
{
	return IsButtonPressed(0, R_STICK_LEFT);
}

// 右移動
BOOL CameraRightStick(void)
{
	return IsButtonPressed(0, R_STICK_RIGHT);
}

// 上移動
BOOL CameraUpStick(void)
{
	return IsButtonPressed(0, R_STICK_UP);
}

// 下移動
BOOL CameraDownStick(void)
{
	return IsButtonPressed(0, R_STICK_DOWN);
}


// 子の当たり判定(パーツの番号、パーツ半径、エネミーの座標、エネミーの半径)
BOOL ChildCollision(int index, float r, XMFLOAT3 epos, float er)
{
	// パーツの子の付け根の座標を求めている
	XMMATRIX mtx = XMLoadFloat4x4(&g_Parts[index].mtxWorld);

	// 求めた座標を入れてる
	XMFLOAT3 pos = XMFLOAT3(mtx.r[3].m128_f32[0], mtx.r[3].m128_f32[1], mtx.r[3].m128_f32[2]);

	//BCの当たり判定の結果を返す
	return CollisionBC(pos, epos, r, er);
}

 //階層アニメーション
void HierarchyAnimation(int partmax)
{
	for (int j = 0; j < partmax; j++)
	{
		// 使われているなら処理する
		if ((g_Parts[j].use == TRUE) && (g_Parts[j].tblMax > 0))
		{	// 線形補間の処理
			int nowNo = (int)g_Parts[j].time;			// 整数分であるテーブル番号を取り出している
			int maxNo = g_Parts[j].tblMax;				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
			//INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Parts[j].tblNo];	// 行動テーブルのアドレスを取得

			//--------------------------------------------------------------------------------------
			// キーフレーム変更部分
			std::vector<INTERPOLATION_DATA> tbl = *g_TblAdr[g_Parts[j].tblNo];
			//--------------------------------------------------------------------------------------

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			float nowTime = g_Parts[j].time - nowNo;	// 時間部分である少数を取り出している

			Pos *= nowTime;								// 現在の移動量を計算している
			Rot *= nowTime;								// 現在の回転量を計算している
			Scl *= nowTime;								// 現在の拡大率を計算している

			// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
			XMStoreFloat3(&g_Parts[j].pos, nowPos + Pos);

			// 計算して求めた回転量を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[j].rot, nowRot + Rot);

			// 計算して求めた拡大率を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[j].scl, nowScl + Scl);

			// frameを使て時間経過処理をする
			g_Parts[j].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			if ((int)g_Parts[j].time >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				g_Parts[j].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる

				//--------------------------------------------------------------------------------------
				// キーフレーム変更部分
				switch (g_Player[0].setTbl)
				{
				case STATE_ATK:
					g_Parts[j].tblMax = -1;
					break;
				}
				//--------------------------------------------------------------------------------------

			}
		}
	}
}

// キー入力
void InputWalkPlayer(void)
{
	// 座標の記憶
	g_Player[0].oldpos = g_Player[0].pos;

	// -----キーボード---------------
	if (AnyKeyPress())
	{
		g_Player[0].spd = VALUE_MOVE;
		g_Player[0].state = STATE_WALK;

		// 斜め方向に移動する
		float x = 0.0f;
		float z = 0.0f;

		if (MoveLeftKeyboard())  x = -1.0f;
		if (MoveRightKeyboard()) x = 1.0f;
		if (MoveUpKeyboard())	 z = -1.0f;
		if (MoveDownKeyboard())  z = 1.0f;

		float angle = atan2f(x, z) * -1;
		g_Player[0].dir = angle;

	}

	// -----スティック---------------
	if (AnyStickpress())
	{
		g_Player[0].spd = VALUE_MOVE;
		g_Player[0].state = STATE_WALK;

		//// キー入力を取得している
		DIJOYSTATE2		dijs = GetDeviceState();

		// プレイヤーを360度回している
		float angle = atan2f((float)dijs.lX, (float)dijs.lY) * -1;

		g_Player[0].dir = angle;

	}

}

// 移動処理
void WalkPlayer(void)
{
	// Key入力があったら移動処理する
	CAMERA* cam = GetCamera();

	switch (g_Player[0].state)
	{
	case STATE_WALK:
		if (g_Player[0].spd < VALUE_MOVE) g_Player[0].state = STATE_STAY;
		break;

	default:
		break;
	}

	if (g_Player[0].spd > 0.0f)
	{
		g_Player[0].rot.y = g_Player[0].dir + cam->rot.y;

		// 入力のあった方向へプレイヤーを向かせて移動させる
		g_Player[0].pos.x -= sinf(g_Player[0].rot.y) * g_Player[0].spd;
		g_Player[0].pos.z -= cosf(g_Player[0].rot.y) * g_Player[0].spd;

		// 慣性をいれている
		g_Player[0].spd *= 0.9f;

	}
}

// 攻撃処理
void InputAttackPlayer(void)
{
	CAMERA* cam = GetCamera();

	// 弾発射処理
	if ( (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_R) ) && g_Player->reload != 0)
	{
		g_Player[0].rot.y = cam->rot.y + XM_PI;

		// 残弾数を減らす
		g_Player[0].reload--;

		// ワールドマトリックスの初期化
		// 正方行列の初期化
		XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
		mtxWorld = XMMatrixIdentity();

		// アフィン変換
		// 移動を反映
		float rot = g_Player[0].rot.z;
		mtxTranslate = XMMatrixTranslation(0, 0, 500); // ベクトルの強さ(プレイヤーのZ方向)

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

		vector.x += cam->pos.x;
		vector.y += cam->pos.y;
		vector.z += cam->pos.z;

		XMFLOAT3 move = {0.0f,0.0f,0.0f};
		XMFLOAT3 vPos = g_Player[0].pos;
		vPos.y += PLAYER_BULLET;

		// プレイヤーとカメラの差分
		float ofset_y = vPos.y - vector.y;

		// プレイヤーの頭基準にカメラの高さを反転させる
		vector.y += ofset_y * 3;

		// カメラ
		XMVECTOR vA = XMLoadFloat3(&vector);
		// プレイヤーの座標
		XMVECTOR vB = XMLoadFloat3(&vPos);

		// vABの正規化
		// 長さを求めて、正規化している(A,B)
		XMVECTOR vAB = XMVector3Normalize(vA - vB);

		XMStoreFloat3(&move, vAB);
		SetBullet(vPos, move);

		g_Player[0].state = STATE_ATK;

		// カメラシェイク
		// 移動する秒数をカウントする
		//cam->cnt++;
		//if (cam->cnt == 50)
		//{
		//	cam->cnt = 0;
		//	g_Enemy[i].spd *= -1.0f;
		//	g_Enemy[i].rot.y += XM_PI;
		//}
		//// 移動する
		//g_Enemy[i].pos.z -= g_Enemy[i].spd;


		// カメラシェイク
		// 移動する秒数をカウントする
		//cam->pos.y -= cam->spd;
		//cam->cnt++;
		//if (cam->cnt == 5)
		//{
		//	cam->cnt = 0;
		//	cam->spd *= -1.0f;
		//}
		//// 移動する
		//cam->pos.y -= cam->spd;

		// 効果音
		PlaySound(SOUND_LABEL_SE_player_atc);
	}

	// リロード
	if ( (GetKeyboardTrigger(DIK_TAB) || IsButtonTriggered(0, BUTTON_L) ) && g_Player[0].reload <= 5)
	{
		g_Player[0].reload = PLAYER_RELOAD;
	}

}

void AttackPlayer(void)
{
	// 状態遷移
	{
		BOOL ans = TRUE;

		// すべてのパーツの最大テーブル数が -1 になっている場合(≒アニメーションが終了している場合)、状態遷移させる
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_Parts[i].tblMax != -1) ans = FALSE;
		}

		if (ans == TRUE) g_Player[0].state = STATE_STAY;
	}
}


void SetTblStay(void)
{
	switch (g_Player[0].setTbl)
	{
	case STATE_STAY:
		break;

	default:
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_Parts[i].use == FALSE) continue;

			g_Parts[i].time = 0.0f;
			g_Parts[i].tblNo = i + STAY_BODY;
			g_Parts[i].tblMax = (int)g_TblAdr[g_Parts[i].tblNo]->size();

		}
		g_Player[0].setTbl = STATE_STAY;
		break;
	}
}

void SetTblWalk(void)
{
	switch (g_Player[0].setTbl)
	{
	case STATE_WALK:
		break;

	default:
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_Parts[i].use == FALSE) continue;

			g_Parts[i].time = 0.0f;
			g_Parts[i].tblNo = i + WALK_BODY;
			g_Parts[i].tblMax = (int)g_TblAdr[g_Parts[i].tblNo]->size();

		}
		g_Player[0].setTbl = STATE_WALK;
		break;
	}
}

void SetTblAttack(void)
{
	switch (g_Player[0].setTbl)
	{
	case STATE_ATK:
		break;

	default:
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_Parts[i].use == FALSE) continue;

			g_Parts[i].time = 0.0f;
			g_Parts[i].tblNo = i + ATK_BODY;
			g_Parts[i].tblMax = (int)g_TblAdr[g_Parts[i].tblNo]->size();

		}
		g_Player[0].setTbl = STATE_ATK;
		break;
	}
}