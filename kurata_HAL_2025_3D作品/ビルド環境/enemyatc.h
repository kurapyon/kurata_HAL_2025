//=============================================================================
//
// エネミーATC処理 [enemyatc.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	ENEMYATC_MAX_BULLET		(256)	// 弾最大数
#define ENEMYATC_LIFE			(300)	// 弾のライフ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;		// ワールドマトリックス
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	rot;			// 角度
	XMFLOAT3	scl;			// スケール
	XMFLOAT3	spd;			// 移動量
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			blife;			// バレットのライフ
	int			shadowIdx;		// 影ID
	BOOL		use;			// 使用しているかどうか


} ENEMYATC;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyAtc(void);
void UninitEnemyAtc(void);
void UpdateEnemyAtc(void);
void DrawEnemyAtc(void);

int SetEnemyAtc(XMFLOAT3 pos, XMFLOAT3 rot);

ENEMYATC *GetEnemyAtc(void);

