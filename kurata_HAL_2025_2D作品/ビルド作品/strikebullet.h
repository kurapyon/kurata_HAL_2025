//=============================================================================
//
// �o���b�g���� [bullet.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define STRIKE_BULLET_MAX		(9)			// �o���b�g��Max��
#define STRIKE_BULLET_SPEED		(10.0f)		// �o���b�g�̈ړ��X�s�[�h


// �o���b�g�\����
struct STRIKE
{
	XMFLOAT3			strike_pos;				// �o���b�g�̍��W
	XMFLOAT3			strike_rot;				// �o���b�g�̉�]��
	XMFLOAT3			strike_P0;				// ����_P0
	XMFLOAT3			strike_P1;				// ����_P1
	XMFLOAT3			strike_move;			// �o���b�g�̈ړ���
	BOOL				strike_hit;				// �G�l�~�[�ƃv���C���[�ǂ����ɓ����������̔���
	BOOL				strike_use;				// true:�g���Ă���  false:���g�p
	float				strike_speed;			// �X�s�[�h
	float				strike_w, strike_h;		// ���ƍ���
	int					strike_countAnim;		// �A�j���[�V�����J�E���g
	int					strike_patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					strike_tex;				// �e�N�X�`���[�̔ԍ�
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitStrikeBullet(void);
void UninitStrikeBullet(void);
void UpdateStrikeBullet(void);
void DrawStrikeBullet(void);

void SetStrikeBullet(XMFLOAT3 pos, int tex);

STRIKE* GetStrikeBullet(void);
