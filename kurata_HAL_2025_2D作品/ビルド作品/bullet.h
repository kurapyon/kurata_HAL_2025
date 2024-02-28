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
#define PLAYER_BULLET_MAX		(100)		// �o���b�g��Max��
#define PLAYER_BULLET_SPEED		(10.0f)		// �o���b�g�̈ړ��X�s�[�h


// �o���b�g�\����
struct BULLET
{
	XMFLOAT3			pos;				// �o���b�g�̍��W
	XMFLOAT3			rot;				// �o���b�g�̉�]��
	XMFLOAT3			move;				// �o���b�g�̈ړ���
	XMFLOAT3			P0;					// ����_P0
	XMFLOAT3			P1;					// ����_P1
	BOOL				hit;				// �G�l�~�[�ƃv���C���[�ǂ����ɓ����������̔���
	BOOL				use;				// true:�g���Ă���  false:���g�p
	float				s;					// ����_s
	float				w, h;				// ���ƍ���
	int					countAnim;			// �A�j���[�V�����J�E���g
	int					patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

void SetBullet(XMFLOAT3 pos);

BULLET *GetBullet(void);
