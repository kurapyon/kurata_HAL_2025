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
#define BOS_BULLETE_MAX		(50)		// �o���b�g��Max��
#define BULLETE_SPEED		(1.0f)		// �o���b�g�̈ړ��X�s�[�h


// �{�X�o���b�g�\����
struct BULLETE
{
	XMFLOAT3			P0;					// ����_P0
	XMFLOAT3			P1;					// ����_P1
	XMFLOAT3			pos;				// �o���b�g�̍��W
	XMFLOAT3			rot;				// �o���b�g�̉�]��
	XMFLOAT3			move;				// �o���b�g�̈ړ���
	BOOL				hit;				// �G�l�~�[�ƃv���C���[�ǂ����ɓ����������̔���
	BOOL				tex;				// �e�N�X�`���[�̎���
	BOOL				use;				// true:�g���Ă���  false:���g�p
	float				w, h;				// ���ƍ���
	float				s;					// ����_s
	int					countAnim;			// �A�j���[�V�����J�E���g
	int					patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBulletBos(void);
void UninitBulletBos(void);
void UpdateBulletBos(void);
void DrawBulletBos(void);
XMVECTOR LinearInterpolation(XMVECTOR P0, XMVECTOR P1, float s);
XMVECTOR QuadraticBezierCurve(XMVECTOR p0, XMVECTOR p1, XMVECTOR p2, float s);
void SetBulletBos(int check, BOOL tex);

BULLETE *GetBossBullet(void);


