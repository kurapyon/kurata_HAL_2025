//=============================================================================
//
// �p�[�e�B�N��ATC���� [particleatc.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define POS_BASE_ATC		(0.0f)
#define WIDTH_BASE_ATC		(5.0f)
#define HEIGHTBASE_ATC		(100.0f)
#define PARTICLEATC_LIFE	(120)	// �p�[�e�B�N���̃��C�t
#define PARTICLEATC_SCALE	(1.0f)	// �p�[�e�B�N���̊g�嗦

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticleAtc(void);
void UninitParticleAtc(void);
void UpdateParticleAtc(void);
void   DrawParticleAtc(void);

void SetColorParticleAtc(int nIdxParticle, XMFLOAT4 col);

// �p�[�e�B�N��
int SetParticleAtc(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);

