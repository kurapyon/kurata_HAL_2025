//=============================================================================
//
// サウンド処理 [sound.h]
// Author : GP11A132 09 倉田勇次
// 
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	// BGM
	SOUND_LABEL_BGM_enemy_bgm,		// 戦闘中
	SOUND_LABEL_BGM_gameoverrsult,	// ゲームオーバー
	SOUND_LABEL_BGM_rsult,			// クリア
	SOUND_LABEL_BGM_title_bgm,			// クリア


	// SE
	SOUND_LABEL_SE_enemy,		// エネミー出現効果音
	SOUND_LABEL_SE_hit,			// 攻撃ヒット時
	SOUND_LABEL_SE_player_atc,	// プレイヤーの攻撃音

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

