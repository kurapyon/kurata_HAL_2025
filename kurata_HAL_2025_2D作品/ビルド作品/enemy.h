//=============================================================================
//
// エネミー処理 [enemy.h]
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
#define ENEMY_MAX					(3)		// エネミーのMax人数
#define BOSS_MAX					(1)		// ボスのマックス人数

#define ENEMY_WAIT					(120)	// 攻撃のタイミング
#define BOSS_WAIT					(60)	// 攻撃のタイミング


#define ENEMY_INVINCIBLE_FRAME		(180)	// 180フレーム無敵


//*****************************************************************************
// 構造体定義
//*****************************************************************************

// アニメーションテクスチャー構造体
struct BOSS_TEX
{
	int		id;						// テクスチャー番号
	int		animex;					// アニメーションパターンのX
	int     animey;					// アニメーションパターンのY
	int     animemax;				// アニメーションパターンのマックス
	float	w, h;					// アニメーションの幅と高さ
	float   waitanime;				// アニメーションの時間
};

// エネミー構造体
struct ENEMY
{
	XMFLOAT3	pos;					// ポリゴンの座標
	XMFLOAT3	rot;					// ポリゴンの回転量
	XMFLOAT3	move;					// 移動速度
	XMFLOAT3	scl;					// ポリゴンの拡大縮小
	BOOL		use;					// true:使っている  false:未使用
	BOOL		ans;					// エネミーとプレイヤーが当たっているかの判定
	BOOL		dir;					// エネミーの向き
	BOOL		receive;				// 攻撃を受けたかどうか
	BOOL		enemy_isDamaged;		// 無敵時間開始
	float		w, h;					// 幅と高さ
	float		time;					// 線形補間用
	float		countAnim;				// アニメーションカウント
	float		guage;					// 経験値関係
	float		guagemax;
	int			life;					// エネミーのライフ
	int			enemy_invincibleCnt;	// 無敵時間
	int			level;
	int			patternAnim;			// アニメーションパターンナンバー
	int			id;						// エネミーの管理番号
	int			tblNo;					// 行動データのテーブル番号
	int			tblMax;					// そのテーブルのデータ数
	int			wait;					// フレームカウント（弾の発射の調整）
	int			receivewait;			// 被ダメージ時に色を変える時間
};

// ボス構造体
struct BOSS 
{
	XMFLOAT3	boss_pos;				// ポリゴンの座標
	XMFLOAT3	boss_rot;				// ポリゴンの回転量
	XMFLOAT3	bosss_cl;				// ポリゴンの拡大縮小
	XMFLOAT3	boss_move;				// 移動速度
	BOOL		boss_dir;				// 向き
	BOOL		boss_use;				// true:使っている  false:未使用
	BOOL		boss_ans;				// エネミーとプレイヤーが当たっているかの判定
	BOOL		boss_receive;			// 攻撃を受けたかどうか
	BOOL		boss_atcposition;		// 攻撃ポジションについたかどうか
	BOOL		boss_check;				// 必殺技を打つ準備
	float		boss_w, boss_h;			// 幅と高さ
	float		boss_countanim;			// アニメーションカウント
	float		boss_time;				// 線形補間用
	float		boss_deathblow;			// 必殺技を打つ
	int			boss_patternAnim;		// アニメーションパターンナンバー
	int			boss_id;					// エネミーの管理番号
	int			boss_tblNo;				// 行動データのテーブル番号
	int			boss_tblMax;				// そのテーブルのデータ数
	int			boss_wait;				// フレームカウント（弾の発射の調整）
	int			boss_life;				// ボスのライフ
	int			boss_receivewait;		// 被ダメージ時に色を変える時間
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

// 基本セット
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

// 関数にまとめた系
void ENEMY_UPDATE(void);
void BOSS_UPDATE(void);
void ENEMY_DRAW(void);
void BOSS_DRAW(void);
//void BOSS_ANIME(int no);

// その他
ENEMY* GetEnemy(void);
BOSS* GetBoss(void);



