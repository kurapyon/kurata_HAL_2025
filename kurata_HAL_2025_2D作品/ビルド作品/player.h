//=============================================================================
//
// �v���C���[���� [player.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PLAYER_MAX					(1)			// �v���C���[��Max�l��

#define	PLAYER_OFFSET_CNT			(16)		// 16���g

#define PLAYER_INVINCIBLE_FRAME		(180)		// 180�t���[�����G

#define PLAYER_COLOR_CHANGE_TIME	(180)		// �v���C���[�̐F��ς��Ă��鎞��

#define PLAYER_ACCEL_Y_DOWN			(5.0f)		// �d�͊֌W

enum
{
	PLAYER_DIR_DOWN,
	PLAYER_DIR_LEFT,
	PLAYER_DIR_RIGHT,
	PLAYER_DIR_UP,


	PLAYER_DIR_MAX
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

// �v���C���[�̃A�j���[�V�����e�N�X�`���[�\����
struct TEX_PLAYER
{
	int		id;						// �e�N�X�`���[�ԍ�
	int		animex;					// �A�j���[�V�����p�^�[����X
	int     animey;					// �A�j���[�V�����p�^�[����Y
	int     animemax;				// �A�j���[�V�����p�^�[���̃}�b�N�X
	float	w, h;					// �A�j���[�V�����̕��ƍ���
	float   waitanime;				// �A�j���[�V�����̎���
};

// �v���C���[�\����
struct PLAYER
{
	XMFLOAT3	move;							// �ړ����x
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// �c���|���S���̍��W
	XMFLOAT3	pos;							// �|���S���̍��W
	XMFLOAT3	pos_old;						// �v���C���[�̍��W�̕ۑ�
	XMFLOAT3	rot;							// �|���S���̉�]��
	BOOL		use;							// true:�g���Ă���  false:���g�p
	BOOL		moving;							// �ړ����t���O
	BOOL		dash;							// �_�b�V�����t���O
	BOOL		defense;						// ���t���O
	BOOL		jump;							// �W�����v�t���O
	BOOL		isDamaged;						// �_���[�W���󂯂�����TRUE�ɂȂ�
	BOOL		levelup;						// �o���l�i���x���A�b�v�������ǂ����̔���j
	BOOL		receive;						// �U�����󂯂����ǂ����̔���
	float		w, h;							// ���ƍ���
	float		jumpYMax;						// �W�����v�̃}�b�N�X
	float		energy;							// �X�^�~�i
	float		jumpY;							// �W�����v�̍���
	float		countAnim;						// �A�j���[�V�����J�E���g
	float		defense_time;					// ����ԂɂȂ��Ă����a�o�߂�����������
	int			patternAnim;					// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;							// �e�N�X�`���ԍ�
	int			dir;							// �����i0:�� 1:�E 2:�� 3:���j
	int			jumpCnt;						// �W�����v���̃J�E���g
	int			life;							// �v���C���[�̃��C�t
	int			invincibleCnt;					// ���G����
	int			receivewait;					// �F��ύX���鎞��


};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ������
HRESULT InitPlayer(void);

// �I������
void UninitPlayer(void);

// �X�V����
void PlayerUpdate(void);
void PoseUpdate(void);
void UpdatePlayer(void);

// �`�揈��
void DrawPlayer(void);

// ���̑�
void SetLevelUP(void);
void SetDamage(int damage);
void SetHpGauge(void);
void SetStaminaGauge(void);

PLAYER* GetPlayer(void);



