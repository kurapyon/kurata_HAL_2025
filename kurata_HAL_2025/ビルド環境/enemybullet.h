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
#define	MAX_ENEMYBULLET		(10)	// �e�ő吔

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
	MATERIAL	material;		// �}�e���A��
	XMFLOAT3	spd;			// �ړ���
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			shadowIdx;		// �eID
	BOOL		use;			// �g�p���Ă��邩�ǂ���


} ENEMYBULLET;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyBullet(void);
void UninitEnemyBullet(void);
void UpdateEnemyBullet(void);
void DrawEnemyBullet(void);

int SetEnemyBullet(XMFLOAT3 pos, XMFLOAT3 rot);

ENEMYBULLET * GetEnemyBullet(void);

