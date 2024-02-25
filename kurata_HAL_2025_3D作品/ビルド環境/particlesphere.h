//=============================================================================
//
// �p�[�e�B�N�����̏��� [particlesphere.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PARTICLESPHERE_POS_BASE			(0.0f)
#define PARTICLESPHERE_WIDTH_BASE		(5.0f)
#define PARTICLESPHERE_HEIGHTBASE		(100.0f)
#define PARTICLESPHERE_LIFE				(240)	// �p�[�e�B�N���̃��C�t
#define PARTICLESPHERE_SCALE			(1.0f)	// �p�[�e�B�N���̊g�嗦

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticleSphere(void);
void UninitParticleSphere(void);
void UpdateParticleSphere(void);
void   DrawParticleSphere(void);

void SetColorParticleSphere(int nIdxParticle, XMFLOAT4 col);

// �p�[�e�B�N��
int Particle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetParticleSphere(void);

