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
#define ENEMY_BULLETE_MAX			(2)			// �o���b�g��Max��
#define ENEMY_BULLETE_SPEED			(4.0f)		// �o���b�g�̈ړ��X�s�[�h


// �G�l�~�[�o���b�g�\����
struct ENEMYBULLETE
{
	XMFLOAT3			move;				// �o���b�g�̈ړ���
	XMFLOAT3			pos;				// �o���b�g�̍��W
	XMFLOAT3			rot;				// �o���b�g�̉�]��
	BOOL				use;				// true:�g���Ă���  false:���g�p
	BOOL				hit;				// �G�l�~�[�ƃv���C���[�ǂ����ɓ����������̔���
	BOOL				tex;				// �e�N�X�`���[�̎���
	float				w, h;				// ���ƍ���
	int					countAnim;			// �A�j���[�V�����J�E���g
	int					patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
	int					time;				// �e�����˂���Ă���̎���

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyBullet(void);
void UninitEnemyBullet(void);
void UpdateEnemyBullet(void);
void DrawEnemyBullet(void);
void SetEnemyBullet(XMFLOAT3 enemypos, BOOL tex);
ENEMYBULLETE* GetEnemyBullet(void);


