//=============================================================================
//
// �G�l�~�[���� [enemy.h]
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
#define ENEMY_MAX					(3)		// �G�l�~�[��Max�l��
#define BOSS_MAX					(1)		// �{�X�̃}�b�N�X�l��

#define ENEMY_WAIT					(120)	// �U���̃^�C�~���O
#define BOSS_WAIT					(60)	// �U���̃^�C�~���O


#define ENEMY_INVINCIBLE_FRAME		(180)	// 180�t���[�����G


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

// �A�j���[�V�����e�N�X�`���[�\����
struct BOSS_TEX
{
	int		id;						// �e�N�X�`���[�ԍ�
	int		animex;					// �A�j���[�V�����p�^�[����X
	int     animey;					// �A�j���[�V�����p�^�[����Y
	int     animemax;				// �A�j���[�V�����p�^�[���̃}�b�N�X
	float	w, h;					// �A�j���[�V�����̕��ƍ���
	float   waitanime;				// �A�j���[�V�����̎���
};

// �G�l�~�[�\����
struct ENEMY
{
	XMFLOAT3	pos;					// �|���S���̍��W
	XMFLOAT3	rot;					// �|���S���̉�]��
	XMFLOAT3	move;					// �ړ����x
	XMFLOAT3	scl;					// �|���S���̊g��k��
	BOOL		use;					// true:�g���Ă���  false:���g�p
	BOOL		ans;					// �G�l�~�[�ƃv���C���[���������Ă��邩�̔���
	BOOL		dir;					// �G�l�~�[�̌���
	BOOL		receive;				// �U�����󂯂����ǂ���
	BOOL		enemy_isDamaged;		// ���G���ԊJ�n
	float		w, h;					// ���ƍ���
	float		time;					// ���`��ԗp
	float		countAnim;				// �A�j���[�V�����J�E���g
	float		guage;					// �o���l�֌W
	float		guagemax;
	int			life;					// �G�l�~�[�̃��C�t
	int			enemy_invincibleCnt;	// ���G����
	int			level;
	int			patternAnim;			// �A�j���[�V�����p�^�[���i���o�[
	int			id;						// �G�l�~�[�̊Ǘ��ԍ�
	int			tblNo;					// �s���f�[�^�̃e�[�u���ԍ�
	int			tblMax;					// ���̃e�[�u���̃f�[�^��
	int			wait;					// �t���[���J�E���g�i�e�̔��˂̒����j
	int			receivewait;			// ��_���[�W���ɐF��ς��鎞��
};

// �{�X�\����
struct BOSS 
{
	XMFLOAT3	boss_pos;				// �|���S���̍��W
	XMFLOAT3	boss_rot;				// �|���S���̉�]��
	XMFLOAT3	bosss_cl;				// �|���S���̊g��k��
	XMFLOAT3	boss_move;				// �ړ����x
	BOOL		boss_dir;				// ����
	BOOL		boss_use;				// true:�g���Ă���  false:���g�p
	BOOL		boss_ans;				// �G�l�~�[�ƃv���C���[���������Ă��邩�̔���
	BOOL		boss_receive;			// �U�����󂯂����ǂ���
	BOOL		boss_atcposition;		// �U���|�W�V�����ɂ������ǂ���
	BOOL		boss_check;				// �K�E�Z��ł���
	float		boss_w, boss_h;			// ���ƍ���
	float		boss_countanim;			// �A�j���[�V�����J�E���g
	float		boss_time;				// ���`��ԗp
	float		boss_deathblow;			// �K�E�Z��ł�
	int			boss_patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int			boss_id;					// �G�l�~�[�̊Ǘ��ԍ�
	int			boss_tblNo;				// �s���f�[�^�̃e�[�u���ԍ�
	int			boss_tblMax;				// ���̃e�[�u���̃f�[�^��
	int			boss_wait;				// �t���[���J�E���g�i�e�̔��˂̒����j
	int			boss_life;				// �{�X�̃��C�t
	int			boss_receivewait;		// ��_���[�W���ɐF��ς��鎞��
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

// ��{�Z�b�g
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

// �֐��ɂ܂Ƃ߂��n
void ENEMY_UPDATE(void);
void BOSS_UPDATE(void);
void ENEMY_DRAW(void);
void BOSS_DRAW(void);
//void BOSS_ANIME(int no);

// ���̑�
ENEMY* GetEnemy(void);
BOSS* GetBoss(void);



