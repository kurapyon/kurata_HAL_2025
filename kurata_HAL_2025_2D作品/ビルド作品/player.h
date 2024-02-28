//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_MAX					(1)			// プレイヤーのMax人数

#define	PLAYER_OFFSET_CNT			(16)		// 16分身

#define PLAYER_INVINCIBLE_FRAME		(180)		// 180フレーム無敵

#define PLAYER_COLOR_CHANGE_TIME	(180)		// プレイヤーの色を変えている時間

#define PLAYER_ACCEL_Y_DOWN			(5.0f)		// 重力関係

enum
{
	PLAYER_DIR_DOWN,
	PLAYER_DIR_LEFT,
	PLAYER_DIR_RIGHT,
	PLAYER_DIR_UP,


	PLAYER_DIR_MAX
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

// プレイヤーのアニメーションテクスチャー構造体
struct TEX_PLAYER
{
	int		id;						// テクスチャー番号
	int		animex;					// アニメーションパターンのX
	int     animey;					// アニメーションパターンのY
	int     animemax;				// アニメーションパターンのマックス
	float	w, h;					// アニメーションの幅と高さ
	float   waitanime;				// アニメーションの時間
};

// プレイヤー構造体
struct PLAYER
{
	XMFLOAT3	move;							// 移動速度
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// 残像ポリゴンの座標
	XMFLOAT3	pos;							// ポリゴンの座標
	XMFLOAT3	pos_old;						// プレイヤーの座標の保存
	XMFLOAT3	rot;							// ポリゴンの回転量
	BOOL		use;							// true:使っている  false:未使用
	BOOL		moving;							// 移動中フラグ
	BOOL		dash;							// ダッシュ中フラグ
	BOOL		defense;						// 守りフラグ
	BOOL		jump;							// ジャンプフラグ
	BOOL		isDamaged;						// ダメージを受けた時にTRUEになる
	BOOL		levelup;						// 経験値（レベルアップしたかどうかの判定）
	BOOL		receive;						// 攻撃を受けたかどうかの判定
	float		w, h;							// 幅と高さ
	float		jumpYMax;						// ジャンプのマックス
	float		energy;							// スタミナ
	float		jumpY;							// ジャンプの高さ
	float		countAnim;						// アニメーションカウント
	float		defense_time;					// 守り状態になってから難病経過したか数える
	int			patternAnim;					// アニメーションパターンナンバー
	int			texNo;							// テクスチャ番号
	int			dir;							// 向き（0:上 1:右 2:下 3:左）
	int			jumpCnt;						// ジャンプ中のカウント
	int			life;							// プレイヤーのライフ
	int			invincibleCnt;					// 無敵時間
	int			receivewait;					// 色を変更する時間


};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化
HRESULT InitPlayer(void);

// 終了処理
void UninitPlayer(void);

// 更新処理
void PlayerUpdate(void);
void PoseUpdate(void);
void UpdatePlayer(void);

// 描画処理
void DrawPlayer(void);

// その他
void SetLevelUP(void);
void SetDamage(int damage);
void SetHpGauge(void);
void SetStaminaGauge(void);

PLAYER* GetPlayer(void);



