//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : GP11A132 09 �q�c�E��
// 
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	// BGM
	SOUND_LABEL_BGM_enemy_bgm,		// �퓬��
	SOUND_LABEL_BGM_gameoverrsult,	// �Q�[���I�[�o�[
	SOUND_LABEL_BGM_rsult,			// �N���A
	SOUND_LABEL_BGM_title_bgm,			// �N���A


	// SE
	SOUND_LABEL_SE_enemy,		// �G�l�~�[�o�����ʉ�
	SOUND_LABEL_SE_hit,			// �U���q�b�g��
	SOUND_LABEL_SE_player_atc,	// �v���C���[�̍U����

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

