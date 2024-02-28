//=============================================================================
//
// バレット処理 [bullet.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define STRIKE_BULLET_MAX		(9)			// バレットのMax数
#define STRIKE_BULLET_SPEED		(10.0f)		// バレットの移動スピード


// バレット構造体
struct STRIKE
{
	XMFLOAT3			strike_pos;				// バレットの座標
	XMFLOAT3			strike_rot;				// バレットの回転量
	XMFLOAT3			strike_P0;				// 制御点P0
	XMFLOAT3			strike_P1;				// 制御点P1
	XMFLOAT3			strike_move;			// バレットの移動量
	BOOL				strike_hit;				// エネミーとプレイヤーどっちに当たったかの判定
	BOOL				strike_use;				// true:使っている  false:未使用
	float				strike_speed;			// スピード
	float				strike_w, strike_h;		// 幅と高さ
	int					strike_countAnim;		// アニメーションカウント
	int					strike_patternAnim;		// アニメーションパターンナンバー
	int					strike_tex;				// テクスチャーの番号
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitStrikeBullet(void);
void UninitStrikeBullet(void);
void UpdateStrikeBullet(void);
void DrawStrikeBullet(void);

void SetStrikeBullet(XMFLOAT3 pos, int tex);

STRIKE* GetStrikeBullet(void);
