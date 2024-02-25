//=============================================================================
//
// �G�l�~�[ATC���� [enemyatc.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	ENEMYATC_MAX_BULLET		(256)	// �e�ő吔
#define ENEMYATC_LIFE			(300)	// �e�̃��C�t


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;		// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	rot;			// �p�x
	XMFLOAT3	scl;			// �X�P�[��
	XMFLOAT3	spd;			// �ړ���
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			blife;			// �o���b�g�̃��C�t
	int			shadowIdx;		// �eID
	BOOL		use;			// �g�p���Ă��邩�ǂ���


} ENEMYATC;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyAtc(void);
void UninitEnemyAtc(void);
void UpdateEnemyAtc(void);
void DrawEnemyAtc(void);

int SetEnemyAtc(XMFLOAT3 pos, XMFLOAT3 rot);

ENEMYATC *GetEnemyAtc(void);

