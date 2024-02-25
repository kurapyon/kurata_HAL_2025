//=============================================================================
//
// チュートリアル処理 [tutorial.h]
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
#define TUTORIAL_ENEMY_MAX			(4)		// エネミーのMax人数

#define TUTORIALENEMY_WAIT			(180)	// チュートリアルエネミーの攻撃間隔


//*****************************************************************************
// 構造体定義
//*****************************************************************************


// エネミー構造体
//struct TUTORIAL_ENEMY
//{
//	XMFLOAT3	pos;			// ポリゴンの座標
//	XMFLOAT3	rot;			// ポリゴンの回転量
//	XMFLOAT3	move;			// 移動速度
//	XMFLOAT3	scl;			// ポリゴンの拡大縮小
//	BOOL		use;			// true:使っている  false:未使用
//	BOOL		ans;			// エネミーとプレイヤーが当たっているかの判定
//	BOOL		dir;			// エネミーの向き
//	BOOL		receive;		// 攻撃を受けたかどうか
//	float		w, h;			// 幅と高さ
//	float		time;			// 線形補間用
//	float		countAnim;		// アニメーションカウント
//	float		guage;			// 経験値関係
//	float		guagemax;
//	int			level;
//	int			life;			// エネミーのライフ
//	int			patternAnim;	// アニメーションパターンナンバー
//	int			id;				// エネミーの管理番号
//	int			tblNo;			// 行動データのテーブル番号
//	int			tblMax;			// そのテーブルのデータ数
//	int			wait;			// フレームカウント（弾の発射の調整）
//	int			receivewait;	// 被ダメージ時に色を変える時間
//};




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

// 基本セット
HRESULT  InitTutorial(void);
void	 UninitTutorial(void);
void	 UpdatTutorial(void);
void	 DrawTutorial(void);

// 関数にまとめた系
//void TUTORIAL_ENEMY_UPDATE(void);
//void TUTORIAL_ENEMY_DRAW(void);
//
//// その他
//TUTORIAL_ENEMY* GetTutorial(void);




