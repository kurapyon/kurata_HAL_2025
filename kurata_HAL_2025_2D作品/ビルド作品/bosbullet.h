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
#define BOS_BULLETE_MAX		(50)		// バレットのMax数
#define BULLETE_SPEED		(1.0f)		// バレットの移動スピード


// ボスバレット構造体
struct BULLETE
{
	XMFLOAT3			P0;					// 制御点P0
	XMFLOAT3			P1;					// 制御点P1
	XMFLOAT3			pos;				// バレットの座標
	XMFLOAT3			rot;				// バレットの回転量
	XMFLOAT3			move;				// バレットの移動量
	BOOL				hit;				// エネミーとプレイヤーどっちに当たったかの判定
	BOOL				tex;				// テクスチャーの識別
	BOOL				use;				// true:使っている  false:未使用
	float				w, h;				// 幅と高さ
	float				s;					// 制御点s
	int					countAnim;			// アニメーションカウント
	int					patternAnim;		// アニメーションパターンナンバー
	int					texNo;				// 何番目のテクスチャーを使用するのか
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBulletBos(void);
void UninitBulletBos(void);
void UpdateBulletBos(void);
void DrawBulletBos(void);
XMVECTOR LinearInterpolation(XMVECTOR P0, XMVECTOR P1, float s);
XMVECTOR QuadraticBezierCurve(XMVECTOR p0, XMVECTOR p1, XMVECTOR p2, float s);
void SetBulletBos(int check, BOOL tex);

BULLETE *GetBossBullet(void);


