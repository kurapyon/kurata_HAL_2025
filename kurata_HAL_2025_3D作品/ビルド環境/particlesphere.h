//=============================================================================
//
// パーティクル球体処理 [particlesphere.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PARTICLESPHERE_POS_BASE			(0.0f)
#define PARTICLESPHERE_WIDTH_BASE		(5.0f)
#define PARTICLESPHERE_HEIGHTBASE		(100.0f)
#define PARTICLESPHERE_LIFE				(240)	// パーティクルのライフ
#define PARTICLESPHERE_SCALE			(1.0f)	// パーティクルの拡大率

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticleSphere(void);
void UninitParticleSphere(void);
void UpdateParticleSphere(void);
void   DrawParticleSphere(void);

void SetColorParticleSphere(int nIdxParticle, XMFLOAT4 col);

// パーティクル
int Particle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetParticleSphere(void);

