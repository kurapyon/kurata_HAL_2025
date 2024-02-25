//=============================================================================
//
// �`���[�g���A������ [tutorial.h]
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
#define TUTORIAL_ENEMY_MAX			(4)		// �G�l�~�[��Max�l��

#define TUTORIALENEMY_WAIT			(180)	// �`���[�g���A���G�l�~�[�̍U���Ԋu


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


// �G�l�~�[�\����
//struct TUTORIAL_ENEMY
//{
//	XMFLOAT3	pos;			// �|���S���̍��W
//	XMFLOAT3	rot;			// �|���S���̉�]��
//	XMFLOAT3	move;			// �ړ����x
//	XMFLOAT3	scl;			// �|���S���̊g��k��
//	BOOL		use;			// true:�g���Ă���  false:���g�p
//	BOOL		ans;			// �G�l�~�[�ƃv���C���[���������Ă��邩�̔���
//	BOOL		dir;			// �G�l�~�[�̌���
//	BOOL		receive;		// �U�����󂯂����ǂ���
//	float		w, h;			// ���ƍ���
//	float		time;			// ���`��ԗp
//	float		countAnim;		// �A�j���[�V�����J�E���g
//	float		guage;			// �o���l�֌W
//	float		guagemax;
//	int			level;
//	int			life;			// �G�l�~�[�̃��C�t
//	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
//	int			id;				// �G�l�~�[�̊Ǘ��ԍ�
//	int			tblNo;			// �s���f�[�^�̃e�[�u���ԍ�
//	int			tblMax;			// ���̃e�[�u���̃f�[�^��
//	int			wait;			// �t���[���J�E���g�i�e�̔��˂̒����j
//	int			receivewait;	// ��_���[�W���ɐF��ς��鎞��
//};




//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

// ��{�Z�b�g
HRESULT  InitTutorial(void);
void	 UninitTutorial(void);
void	 UpdatTutorial(void);
void	 DrawTutorial(void);

// �֐��ɂ܂Ƃ߂��n
//void TUTORIAL_ENEMY_UPDATE(void);
//void TUTORIAL_ENEMY_DRAW(void);
//
//// ���̑�
//TUTORIAL_ENEMY* GetTutorial(void);




