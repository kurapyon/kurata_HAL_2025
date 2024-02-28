//=============================================================================
//
// バレット処理 [bullet.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMY_BULLETE_MAX			(2)			// バレットのMax数
#define ENEMY_BULLETE_SPEED			(4.0f)		// バレットの移動スピード


// エネミーバレット構造体
struct ENEMYBULLETE
{
	XMFLOAT3			move;				// バレットの移動量
	XMFLOAT3			pos;				// バレットの座標
	XMFLOAT3			rot;				// バレットの回転量
	BOOL				use;				// true:使っている  false:未使用
	BOOL				hit;				// エネミーとプレイヤーどっちに当たったかの判定
	BOOL				tex;				// テクスチャーの識別
	float				w, h;				// 幅と高さ
	int					countAnim;			// アニメーションカウント
	int					patternAnim;		// アニメーションパターンナンバー
	int					texNo;				// 何番目のテクスチャーを使用するのか
	int					time;				// 弾が発射されてからの時間

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyBullet(void);
void UninitEnemyBullet(void);
void UpdateEnemyBullet(void);
void DrawEnemyBullet(void);
void SetEnemyBullet(XMFLOAT3 enemypos, BOOL tex);
ENEMYBULLETE* GetEnemyBullet(void);


