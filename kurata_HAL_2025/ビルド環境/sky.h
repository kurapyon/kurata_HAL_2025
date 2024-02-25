//=============================================================================
//
// �󏈗� [sky.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_SKY			(1)					// ��̐�

#define	SKY_SIZE		(5.0f)				// �����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct SKY
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F


	// �e�́ANULL�A�q���͐e�̃A�h���X������
	//ENEMY* parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	float				spd;				// �ړ��X�s�[�h
	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	float				time;				// ���`��ԗp
	int					tblNo;				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;				// ���̃e�[�u���̃f�[�^��

	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;
	XMFLOAT3			UpVector;			// �����������Ă��鏊

	// �W�����v
	//BOOL		jump;			// �W�����v�t���O
	//float		jumpY;			// �W�����v�̍���
	//int			jumpCnt;		// �W�����v���̃J�E���g
	//float		jumpYMax;		// 

	// dissolve
	DISSOLVE			dissolve;



};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSky(void);
void	UninitSky(void);
void	UpdateSky(void);
void	DrawSky(void);

SKY *GetSky(void);

