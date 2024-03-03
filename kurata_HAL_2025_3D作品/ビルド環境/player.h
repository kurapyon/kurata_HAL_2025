//=============================================================================
//
// モデル処理 [player.h]
// Author :  GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER		(1)					// プレイヤーの数

#define	PLAYER_SIZE		(20.0f)				// 当たり判定の大きさ

// プレイヤーのHP
#define PLAYER_HP		(10.0f)				// プレイヤーの体力

// 弾の残弾数
#define PLAYER_RELOAD	(6)					// 弾の残弾数

enum STATE_LIST
{
	STATE_STAY,
	STATE_WALK,
	STATE_STAND,
	STATE_ATK,

	STATE_MAX
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLAYER
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			oldpos;				// 座標の記憶
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	float				hp;					// ライフ
	float				spd;				// 移動スピード
	float				dir;				// 向き
	float				jump_pos;			// ジャンプ

	int					shadowIdx;			// 影のインデックス番号
	int					receivewait;		// 被ダメージ時の変更時間
	int					state;				// ステート(状態）
	int					reload;				// 弾の残りの数
	int					ModelIdx;			// モデルインデックス

	BOOL				use;
	BOOL				ans;				// 当たり判定
	BOOL				chek;				// ゲーム開始位置判定
	BOOL				falling;			// 落下判定

	float				size;

	// 階層アニメーション用のメンバー変数
	float				time;				// 線形補間用
	int					tblNo;				// 行動データのテーブル番号
	int					tblMax;				// そのテーブルのデータ数
	int					setTbl;				// セットしているテーブル

	// 親は、NULL、子供は親のアドレスを入れる
	PLAYER				*parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// クォータニオン
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// 自分が立っている所

	// ジャンプ
	BOOL		jump;						// ジャンプフラグ
	float		jumpY;						// ジャンプの高さ
	int			jumpCnt;					// ジャンプ中のカウント
	float		jumpYMax;					// 

	// バレット
	float		bulletX;

	// dissolve
	DISSOLVE			dissolve;
};

struct PLAYER_MODEL
{
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

// キーボード入力
BOOL AnyKeyPress(void);
BOOL MoveLeftKeyboard(void);
BOOL MoveRightKeyboard(void);
BOOL MoveUpKeyboard(void);
BOOL MoveDownKeyboard(void);
BOOL MoveJumpKeyboard(void);

// 左スティック入力
BOOL AnyStickpress(void);
BOOL MoveLeftStick(void);
BOOL MoveRightStick(void);
BOOL MoveUpStick(void);
BOOL MoveDownStick(void);

// 右スティック入力
BOOL CameraAnyStickpress(void);
BOOL CameraLeftStick(void);
BOOL CameraRightStick(void);
BOOL CameraUpStick(void);
BOOL CameraDownStick(void);


// 当たり判定
BOOL ChildCollision(int index, float r, XMFLOAT3 epos, float er);

// 階層アニメーション
void HierarchyAnimation(int partmax);
