//=============================================================================
//
// �G�l�~�[���f������ [enemy.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

// �G�l�~�[�̐�
#define MAX_ENEMY		(2)				
// �����蔻��̑傫��
#define	ENEMY_SIZE		(20.0f)			
// �G�l�~�[�̃��C�t
#define ENEMY_HP		(5.0f)			
// ��̃��C�t
#define TEMPLE_HP		(1.0f)			

// �{�X
#define MAX_ENEMYBOSS	(2)			// �G�l�~�[�̐�
#define	ENEMYBOSS_SIZE	(80.0f)		// �����蔻��̑傫��
#define ENEMYBOSS_HP	(5.0f)		// �G�l�~�[�̃��C�t

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	BOOL				load;
	DX11_MODEL			model;				// ���f�����

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	ENEMY* parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	float				hp;					// HP(���C�t)
	float				spd;				// �ړ��X�s�[�h
	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	float				time;				// ���`��ԗp
	int					tblNo;				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;				// ���̃e�[�u���̃f�[�^��

	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;
	XMFLOAT3			UpVector;			// �����������Ă��鏊

	// �{�X��ڍs
	BOOL chenge;

	// �J�E���g
	int			cnt;			// �U���񐔂��J�E���g����
	BOOL		chek;			// 

	// ��_���[�W���̕ύX����
	float receivewait;
	BOOL  ans;

	// dissolve
	DISSOLVE			dissolve;
};

//struct ENEMY_MODEL
//{
//
//};


// �{�X
struct ENEMYBOSS
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)
	XMFLOAT3			oldpos;				// �O�̍��W��ۑ�����
	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F


	// �e�́ANULL�A�q���͐e�̃A�h���X������
	ENEMYBOSS* parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	float				hp;					// HP(���C�t)
	float				spd;				// �ړ��X�s�[�h
	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	float				time;				// ���`��ԗp
	int					tblNo;				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;				// ���̃e�[�u���̃f�[�^��
	//int					setTbl;				// �Z�b�g���Ă���e�[�u��
	//int					state;				// �X�e�[�g(��ԁj


	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;
	XMFLOAT3			UpVector;			// �����������Ă��鏊

	// �W�����v
	BOOL		jump;			// �W�����v�t���O
	float		jumpY;			// �W�����v�̍���
	int			jumpCnt;		// �W�����v���̃J�E���g
	float		jumpYMax;		// 

	// �J�E���g
	int			cnt;			// �U���񐔂��J�E���g����
	float		dmgcnt;			// �_���[�W���J�E���g
	BOOL		chek;			// 

	// ��_���[�W���̕ύX����
	float receivewait;
	BOOL  ans;

	// dissolve
	DISSOLVE			dissolve;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
ENEMYBOSS* GetEnemyBoss(void);


