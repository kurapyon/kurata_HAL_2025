//=============================================================================
//
// カット処理 [cutting.h]
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
#define CUTTING_MAX		(10)		// カットのMax数
#define CUTTING_SPEED	(12.0f)		// カットの移動スピード

// バレット構造体
struct CUTTING
{
	BOOL				use;				// true:使っている  false:未使用
	float				w, h;				// 幅と高さ
	XMFLOAT3			pos;				// カットの座標
	XMFLOAT3			rot;				// カットの回転量
	XMFLOAT3			move;				// カットの移動量
	XMFLOAT3			xmove;
	int					countAnim;			// アニメーションカウント
	int					patternAnim;		// アニメーションパターンナンバー
	int					texNo;				// 何番目のテクスチャーを使用するのか
	int					dir;			            // 向き（0:上 1:右 2:下 3:左）
	int					bulletlife;
	int					bulletmode;
};

//斬撃の描画用列挙型
enum
{

	CUTTING_START,
	CUTTING_CHU,
	CUTTING_FIN,

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitCutting(void);
void UninitCutting(void);
void UpdateCutting(void);
void DrawCutting(void);

CUTTING *GetCutting(void);
void SetCutting(XMFLOAT3 pos);


