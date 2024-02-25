//=============================================================================
//
// バレット処理 [bullet.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMYBOSSBULLET_MAX		(256)			// バレットのMax数
#define ENEMYBOSSBULLET_SPEED	(4.0f)			// バレットの移動スピード


// バレット構造体
struct ENEMYBOSSBULLET
{
	BOOL		use;				// true:使っている  false:未使用
	float		w, h;				// 幅と高さ
	int			countAnim;			// アニメーションカウント
	int			patternAnim;		// アニメーションパターンナンバー
	int			texNo;				// 何番目のテクスチャーを使用するのか
	XMFLOAT4X4	mtxWorld;		// ワールドマトリックス
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	localpos;		// ローカル位置
	XMFLOAT3	rot;			// 角度
	XMFLOAT3	move;			// 方向・移動
	XMFLOAT3	scl;			// スケール
	XMFLOAT3	p0;				// ベジェ曲線
	XMFLOAT3	p1;				// ベジェ曲線
	XMFLOAT3	p2t;			// ベジェ曲線
	MATERIAL	material;		// マテリアル
	float		spd;			// 移動量
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	float		hp;				// 弾の出現時間
	float		s;				// ベジェ曲線
	int			shadowIdx;		// 影ID


};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyBossBullet(void);
void  UninitEnemyBossBullet(void);
void  UpdateEnemyBossBullet(void);
void    DrawEnemyBossBullet(void);

ENEMYBOSSBULLET* GetEnemyBossBullet(void);

int SetBossAttackBullet(void);

