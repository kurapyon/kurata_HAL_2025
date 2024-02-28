//=============================================================================
//
// �G�t�F�N�g���� [effect.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************


#define ENEMY_NUM_PARTS			 (30)		//�G�t�F�N�g���p�[�e�B�N����
#define ENEMY_NUM_EFFECTS		 (10)		//�G�t�F�N�g�ő吔

// �{��
#define EFFECT_PATTERN_X		 (10)
#define EFFECT_PATTERN_Y		 (1)

enum 
{
	TEXTURE_EFFECT00,
	TEXTURE_EFFECT,
	TEXTURE_DEATHBLOW,
};

//*****************************************************************************
// �\���̐錾
//*****************************************************************************

typedef struct	// �G�t�F�N�g�\����
{
	XMFLOAT3		pos;						// �|���S���̈ړ���
	XMFLOAT3		move;						// �ړ���
	int				PatternAnim;				// �A�j���[�V�����p�^�[���i���o�[
	int				CountAnim;					// �A�j���[�V�����J�E���g

	int				liveTime;

	bool			isFinish;
}ENEMY_PARTICLE;

typedef struct	// �G�t�F�N�g�\����
{
	int				use;
	int				isEnding;
	bool			isRemoveOnFinish;

	XMFLOAT3		pos;						// �|���S���̈ړ���

	int				duration;
	int				elapsed;
	int				numFinish;

	int				effectCount;
	int				emitCounter;

	int				texno;

	ENEMY_PARTICLE		pParticle[ENEMY_NUM_PARTS];

}ENEMY_EFFECT;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyEffect(void);
void UninitEnemyEffect(void);
void UpdateEnemyEffect(void);
void DrawEnemyEffect(void);
void SetEnemyEffect(float x, float y, int duration, int j);
