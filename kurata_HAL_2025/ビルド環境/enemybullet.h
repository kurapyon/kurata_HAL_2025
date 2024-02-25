//=============================================================================
//
// 弾発射処理 [bullet.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_ENEMYBULLET		(10)	// 弾最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;		// ワールドマトリックス
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	rot;			// 角度
	XMFLOAT3	move;			// 方向・移動
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	XMFLOAT3	spd;			// 移動量
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			shadowIdx;		// 影ID
	BOOL		use;			// 使用しているかどうか


} ENEMYBULLET;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyBullet(void);
void UninitEnemyBullet(void);
void UpdateEnemyBullet(void);
void DrawEnemyBullet(void);

int SetEnemyBullet(XMFLOAT3 pos, XMFLOAT3 rot);

ENEMYBULLET * GetEnemyBullet(void);

