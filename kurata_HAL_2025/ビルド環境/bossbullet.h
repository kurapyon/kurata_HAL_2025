//=============================================================================
//
// �e���ˏ��� [bullet.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_BOSSBULLET		(10)	// �e�ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;		// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;			// �ʒu
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
	BOOL		use;			// �g�p���Ă��邩�ǂ���


} BOSSBULLET;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBossBullet(void);
void	UninitBossBullet(void);
void	UpdateBossBullet(void);
void	DrawBossBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 move);
int SetBossBullet(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 tpos);

BOSSBULLET *GetBossBullet(void);

