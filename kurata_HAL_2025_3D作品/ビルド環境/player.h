//=============================================================================
//
// ���f������ [player.h]
// Author :  GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PLAYER		(1)					// �v���C���[�̐�

#define	PLAYER_SIZE		(20.0f)				// �����蔻��̑傫��

// �v���C���[��HP
#define PLAYER_HP		(10.0f)				// �v���C���[�̗̑�

// �e�̎c�e��
#define PLAYER_RELOAD	(6)					// �e�̎c�e��

enum STATE_LIST
{
	STATE_STAY,
	STATE_WALK,
	STATE_STAND,
	STATE_ATK,

	STATE_MAX
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PLAYER
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			oldpos;				// ���W�̋L��
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	float				hp;					// ���C�t
	float				spd;				// �ړ��X�s�[�h
	float				dir;				// ����
	float				jump_pos;			// �W�����v

	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�
	int					receivewait;		// ��_���[�W���̕ύX����
	int					state;				// �X�e�[�g(��ԁj
	int					reload;				// �e�̎c��̐�
	int					ModelIdx;			// ���f���C���f�b�N�X

	BOOL				use;
	BOOL				ans;				// �����蔻��
	BOOL				chek;				// �Q�[���J�n�ʒu����
	BOOL				falling;			// ��������

	float				size;

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	float				time;				// ���`��ԗp
	int					tblNo;				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;				// ���̃e�[�u���̃f�[�^��
	int					setTbl;				// �Z�b�g���Ă���e�[�u��

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	PLAYER				*parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// �����������Ă��鏊

	// �W�����v
	BOOL		jump;						// �W�����v�t���O
	float		jumpY;						// �W�����v�̍���
	int			jumpCnt;					// �W�����v���̃J�E���g
	float		jumpYMax;					// 

	// �o���b�g
	float		bulletX;

	// dissolve
	DISSOLVE			dissolve;
};

struct PLAYER_MODEL
{
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

// �L�[�{�[�h����
BOOL AnyKeyPress(void);
BOOL MoveLeftKeyboard(void);
BOOL MoveRightKeyboard(void);
BOOL MoveUpKeyboard(void);
BOOL MoveDownKeyboard(void);
BOOL MoveJumpKeyboard(void);

// ���X�e�B�b�N����
BOOL AnyStickpress(void);
BOOL MoveLeftStick(void);
BOOL MoveRightStick(void);
BOOL MoveUpStick(void);
BOOL MoveDownStick(void);

// �E�X�e�B�b�N����
BOOL CameraAnyStickpress(void);
BOOL CameraLeftStick(void);
BOOL CameraRightStick(void);
BOOL CameraUpStick(void);
BOOL CameraDownStick(void);


// �����蔻��
BOOL ChildCollision(int index, float r, XMFLOAT3 epos, float er);

// �K�w�A�j���[�V����
void HierarchyAnimation(int partmax);
