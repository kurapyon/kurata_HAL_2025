//=============================================================================
//
// 当たり判定処理 [collision.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "collision.h"
#include "map.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************


//=============================================================================
// BBによる当たり判定処理
// 回転は考慮しない
// 戻り値：当たってたらtrue
//=============================================================================
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh,
	XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// 外れをセットしておく

	// 座標が中心点なので計算しやすく半分にしている
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// バウンディングボックス(BB)の処理
	if ((mpos.x + mw > ypos.x - yw) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y - yh) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// 当たった時の処理
		ans = TRUE;
	}

	return ans;
}

//=============================================================================
// BCによる当たり判定処理
// サイズは半径
// 戻り値：当たってたらTRUE
//=============================================================================
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;						// 外れをセットしておく

	float len = (r1 + r2) * (r1 + r2);		// 半径を2乗した物
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2点間の距離（2乗した物）
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// 半径を2乗した物より距離が短い？
	if (len > lenSq)
	{
		ans = TRUE;	// 当たっている
	}

	return ans;
}

// プレイヤーの当たり判定
BOOL CollisionEdgeAndMapChip(XMFLOAT3 spos, XMFLOAT3 epos, int dir) //上:0 下:1 左:2 右:3 
{
    int y = 0;
    int x = 0;

    int sy = 0;
    int sx = 0;

    int ey = 0;
    int ex = 0;

    switch (dir)
    {
    case UPPER: //上
    case LOWER: //下
        y  = (int)(spos.y)  / MAP_TEXTURE_HEIGHT;
        sx = (int)(spos.x)  / MAP_TEXTURE_WIDTH;
        ex = (int)(epos.x)  / MAP_TEXTURE_WIDTH;

        for (int x = sx; x <= ex; x++)
        {
            if (GetMapDate(y, x) == 0)
            {
                return TRUE;
            }
        }
        break;


    case LEFT:  //左
    case RIGHT: //右

        x  = (int)(spos.x) / MAP_TEXTURE_WIDTH;
        sy = (int)(spos.y) / MAP_TEXTURE_HEIGHT;
        ey = (int)(epos.y) / MAP_TEXTURE_HEIGHT;

        for (int y = sy; y <= ey; y++)
        {
            if (GetMapDate(y, x) != -1)
            {
                return TRUE;
            }
        }
        break;
    }

    return FALSE;
}

