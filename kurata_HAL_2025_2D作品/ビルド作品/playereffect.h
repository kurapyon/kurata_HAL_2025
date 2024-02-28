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


#define PLAYER_NUM_PARTS			(30)		//�G�t�F�N�g���p�[�e�B�N����
#define PLAYER_NUM_EFFECTS			(1)	//�G�t�F�N�g�ő吔

// ���x���A�b�v
#define EFFECT_PATTERN_LV_X			(10)
#define EFFECT_PATTERN_LV_Y			(1)

enum 
{
	TEXTURE_LVUP,
	TEXTURE_EFFECT01,

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
}PLAYER_PARTICLE;

typedef struct	// �G�t�F�N�g�\����
{
	int					use;
	int					isEnding;
	bool				isRemoveOnFinish;

	XMFLOAT3			pos;						// �|���S���̈ړ���

	int					duration;
	int					elapsed;
	int					numFinish;

	int					effectCount;
	int					emitCounter;

	int					texno;

	PLAYER_PARTICLE		pParticle[PLAYER_NUM_PARTS];

}PLAYER_EFFECT;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayerEffect(void);
void UninitPlayerEffect(void);
void UpdatePlayerEffect(void);
void DrawPlayerEffect(void);
void SetPlayerEffect(float x, float y, int duration, int j);
