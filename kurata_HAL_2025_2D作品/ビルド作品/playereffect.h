//=============================================================================
//
// エフェクト処理 [effect.h]
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


#define PLAYER_NUM_PARTS			(30)		//エフェクト内パーティクル数
#define PLAYER_NUM_EFFECTS			(1)	//エフェクト最大数

// レベルアップ
#define EFFECT_PATTERN_LV_X			(10)
#define EFFECT_PATTERN_LV_Y			(1)

enum 
{
	TEXTURE_LVUP,
	TEXTURE_EFFECT01,

};

//*****************************************************************************
// 構造体宣言
//*****************************************************************************

typedef struct	// エフェクト構造体
{
	XMFLOAT3		pos;						// ポリゴンの移動量
	XMFLOAT3		move;						// 移動量
	int				PatternAnim;				// アニメーションパターンナンバー
	int				CountAnim;					// アニメーションカウント

	int				liveTime;

	bool			isFinish;
}PLAYER_PARTICLE;

typedef struct	// エフェクト構造体
{
	int					use;
	int					isEnding;
	bool				isRemoveOnFinish;

	XMFLOAT3			pos;						// ポリゴンの移動量

	int					duration;
	int					elapsed;
	int					numFinish;

	int					effectCount;
	int					emitCounter;

	int					texno;

	PLAYER_PARTICLE		pParticle[PLAYER_NUM_PARTS];

}PLAYER_EFFECT;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayerEffect(void);
void UninitPlayerEffect(void);
void UpdatePlayerEffect(void);
void DrawPlayerEffect(void);
void SetPlayerEffect(float x, float y, int duration, int j);
