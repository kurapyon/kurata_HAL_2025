//=============================================================================
//
// エネミーモデル処理 [enemy.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

// エネミーの数
#define MAX_ENEMY		(2)				
// 当たり判定の大きさ
#define	ENEMY_SIZE		(20.0f)			
// エネミーのライフ
#define ENEMY_HP		(5.0f)			
// 城のライフ
#define TEMPLE_HP		(1.0f)			

// ボス
#define MAX_ENEMYBOSS	(2)			// エネミーの数
#define	ENEMYBOSS_SIZE	(80.0f)		// 当たり判定の大きさ
#define ENEMYBOSS_HP	(5.0f)		// エネミーのライフ

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	BOOL				load;
	DX11_MODEL			model;				// モデル情報

	// 親は、NULL、子供は親のアドレスを入れる
	ENEMY* parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	float				hp;					// HP(ライフ)
	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号

	float				time;				// 線形補間用
	int					tblNo;				// 行動データのテーブル番号
	int					tblMax;				// そのテーブルのデータ数

	// クォータニオン
	XMFLOAT4			Quaternion;
	XMFLOAT3			UpVector;			// 自分が立っている所

	// ボス戦移行
	BOOL chenge;

	// カウント
	int			cnt;			// 攻撃回数をカウントする
	BOOL		chek;			// 

	// 被ダメージ時の変更時間
	float receivewait;
	BOOL  ans;

	// dissolve
	DISSOLVE			dissolve;
};

//struct ENEMY_MODEL
//{
//
//};


// ボス
struct ENEMYBOSS
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)
	XMFLOAT3			oldpos;				// 前の座標を保存する
	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色


	// 親は、NULL、子供は親のアドレスを入れる
	ENEMYBOSS* parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	float				hp;					// HP(ライフ)
	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号

	float				time;				// 線形補間用
	int					tblNo;				// 行動データのテーブル番号
	int					tblMax;				// そのテーブルのデータ数
	//int					setTbl;				// セットしているテーブル
	//int					state;				// ステート(状態）


	// クォータニオン
	XMFLOAT4			Quaternion;
	XMFLOAT3			UpVector;			// 自分が立っている所

	// ジャンプ
	BOOL		jump;			// ジャンプフラグ
	float		jumpY;			// ジャンプの高さ
	int			jumpCnt;		// ジャンプ中のカウント
	float		jumpYMax;		// 

	// カウント
	int			cnt;			// 攻撃回数をカウントする
	float		dmgcnt;			// ダメージをカウント
	BOOL		chek;			// 

	// 被ダメージ時の変更時間
	float receivewait;
	BOOL  ans;

	// dissolve
	DISSOLVE			dissolve;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
ENEMYBOSS* GetEnemyBoss(void);


