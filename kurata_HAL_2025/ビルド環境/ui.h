//=============================================================================
//
// スコア処理 [score.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCORE_MAX			(99999)		// スコアの最大値
#define SCORE_DIGIT			(5)			// 桁数




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT   InitUi(void);
void	UninitUi(void);
void	UpdateUi(void);
void	  DrawUi(void);

//void AddScore(int add);
int GetUi(void);


