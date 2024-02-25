//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define POS_BASE		(0.0f)
#define WIDTH_BASE		(5.0f)
#define HEIGHTBASE		(100.0f)
#define PARTICLE_LIFE	(3)	// パーティクルのライフ
#define PARTICLE_SCALE	(1.0f)	// パーティクルの拡大率

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

void SetColorParticle(int nIdxParticle, XMFLOAT4 col);

// パーティクル
int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);

