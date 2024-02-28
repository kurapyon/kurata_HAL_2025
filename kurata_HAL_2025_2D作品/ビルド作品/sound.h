//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************

// BGM:6	/	SE:0
enum 
{
	// BGM
	SOUND_LABEL_BGM_title,
	SOUND_LABEL_BGM_explanation,
	SOUND_LABEL_BGM_enemy00,
	SOUND_LABEL_BGM_boss00,
	SOUND_LABEL_BGM_boss01,
	SOUND_LABEL_BGM_clear,
	SOUND_LABEL_BGM_gameover,




	// SE
	SOUND_LABEL_game_start,
	SOUND_LABEL_cut00,
	SOUND_LABEL_cut01,
	SOUND_LABEL_bossatc,
	SOUND_LABEL_player_bullet,

	SOUND_LABEL_MAX,

	
};


// 	SOUND_LABEL_BGM_sample000,
//  SOUND_LABEL_SE_shot001,


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

