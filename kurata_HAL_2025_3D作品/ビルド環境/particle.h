//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define POS_BASE		(0.0f)
#define WIDTH_BASE		(5.0f)
#define HEIGHTBASE		(100.0f)
#define PARTICLE_LIFE	(3)	// �p�[�e�B�N���̃��C�t
#define PARTICLE_SCALE	(1.0f)	// �p�[�e�B�N���̊g�嗦

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

void SetColorParticle(int nIdxParticle, XMFLOAT4 col);

// �p�[�e�B�N��
int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);

