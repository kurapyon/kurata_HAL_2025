//=============================================================================
//
// パーティクルATC処理 [particleatc.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define POS_BASE_ATC		(0.0f)
#define WIDTH_BASE_ATC		(5.0f)
#define HEIGHTBASE_ATC		(100.0f)
#define PARTICLEATC_LIFE	(120)	// パーティクルのライフ
#define PARTICLEATC_SCALE	(1.0f)	// パーティクルの拡大率

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticleAtc(void);
void UninitParticleAtc(void);
void UpdateParticleAtc(void);
void   DrawParticleAtc(void);

void SetColorParticleAtc(int nIdxParticle, XMFLOAT4 col);

// パーティクル
int SetParticleAtc(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);

