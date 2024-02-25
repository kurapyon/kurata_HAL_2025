//=============================================================================
//
// �o���b�g���� [bullet.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ENEMYBOSSBULLET_MAX		(256)			// �o���b�g��Max��
#define ENEMYBOSSBULLET_SPEED	(4.0f)			// �o���b�g�̈ړ��X�s�[�h


// �o���b�g�\����
struct ENEMYBOSSBULLET
{
	BOOL		use;				// true:�g���Ă���  false:���g�p
	float		w, h;				// ���ƍ���
	int			countAnim;			// �A�j���[�V�����J�E���g
	int			patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
	XMFLOAT4X4	mtxWorld;		// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	localpos;		// ���[�J���ʒu
	XMFLOAT3	rot;			// �p�x
	XMFLOAT3	move;			// �����E�ړ�
	XMFLOAT3	scl;			// �X�P�[��
	XMFLOAT3	p0;				// �x�W�F�Ȑ�
	XMFLOAT3	p1;				// �x�W�F�Ȑ�
	XMFLOAT3	p2t;			// �x�W�F�Ȑ�
	MATERIAL	material;		// �}�e���A��
	float		spd;			// �ړ���
	float		fWidth;			// ��
	float		fHeight;		// ����
	float		hp;				// �e�̏o������
	float		s;				// �x�W�F�Ȑ�
	int			shadowIdx;		// �eID


};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyBossBullet(void);
void  UninitEnemyBossBullet(void);
void  UpdateEnemyBossBullet(void);
void    DrawEnemyBossBullet(void);

ENEMYBOSSBULLET* GetEnemyBossBullet(void);

int SetBossAttackBullet(void);

