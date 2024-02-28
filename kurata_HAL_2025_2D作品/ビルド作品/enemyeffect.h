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


#define ENEMY_NUM_PARTS			 (30)		//エフェクト内パーティクル数
#define ENEMY_NUM_EFFECTS		 (10)		//エフェクト最大数

// ボム
#define EFFECT_PATTERN_X		 (10)
#define EFFECT_PATTERN_Y		 (1)

enum 
{
	TEXTURE_EFFECT00,
	TEXTURE_EFFECT,
	TEXTURE_DEATHBLOW,
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
}ENEMY_PARTICLE;

typedef struct	// エフェクト構造体
{
	int				use;
	int				isEnding;
	bool			isRemoveOnFinish;

	XMFLOAT3		pos;						// ポリゴンの移動量

	int				duration;
	int				elapsed;
	int				numFinish;

	int				effectCount;
	int				emitCounter;

	int				texno;

	ENEMY_PARTICLE		pParticle[ENEMY_NUM_PARTS];

}ENEMY_EFFECT;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyEffect(void);
void UninitEnemyEffect(void);
void UpdateEnemyEffect(void);
void DrawEnemyEffect(void);
void SetEnemyEffect(float x, float y, int duration, int j);
