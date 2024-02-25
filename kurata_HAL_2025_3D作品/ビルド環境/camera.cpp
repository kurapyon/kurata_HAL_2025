//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "model.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	POS_X_CAM			(0.0f)			// カメラの初期位置(X座標)
#define	POS_Y_CAM			(50.0f)			// カメラの初期位置(Y座標)
#define	POS_Z_CAM			(-140.0f)		// カメラの初期位置(Z座標)

//#define	POS_X_CAM		(0.0f)			// カメラの初期位置(X座標)
//#define	POS_Y_CAM		(200.0f)		// カメラの初期位置(Y座標)
//#define	POS_Z_CAM		(-400.0f)		// カメラの初期位置(Z座標)


#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// ビュー平面の視野角
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比	
#define	VIEW_NEAR_Z		(10.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z		(10000.0f)										// ビュー平面のFarZ値

#define	VALUE_MOVE_CAMERA	(0.5f)				// カメラの移動量
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.005f)	// カメラの回転量

#define UPPERLIMIT			(50.0f)				// カメラの上限
#define LOWERLIMIT			(0.0f)				// カメラの下限


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CAMERA			g_Camera;			// カメラデータ

static int				g_ViewPortType = TYPE_FULL_SCREEN;

static float            g_ResetTargetPosX;	//リセット後のカメラ座標
static float            g_ResetTargetPosZ;	//リセット後のカメラ座標

//=============================================================================
// 初期化処理
//=============================================================================
void InitCamera(void)
{
	g_Camera.pos	= { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
	g_Camera.at		= { 0.0f, 0.0f, 0.0f };
	g_Camera.up		= { 0.0f, 1.0f, 0.0f };
	g_Camera.rot	= { 0.0f, 0.0f, 0.0f };
	g_Camera.spd = 5.0f;
	g_Camera.reset = FALSE;


	// 視点と注視点の距離を計算
	float vx, vz;
	vx = g_Camera.pos.x - g_Camera.at.x;
	vz = g_Camera.pos.z - g_Camera.at.z;
	//g_Camera.len = sqrtf(vx * vx + vz * vz);
	g_Camera.len = 100.0f;

	
	// ビューポートタイプの初期化
	SetViewPort(g_ViewPortType);
}


//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{

	PLAYER* player = GetPlayer();

//#ifdef _DEBUG
	
	if (GetKeyboardPress(DIK_A) || CameraLeftStick())
	{// 視点旋回「左」
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_D) || CameraRightStick())
	{// 視点旋回「右」
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_S) || CameraUpStick())
	{// 視点移動「上」
		g_Camera.pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_W) || CameraDownStick())
	{// 視点移動「下」
		g_Camera.pos.y += VALUE_MOVE_CAMERA;
	}

#ifdef _DEBAG
	if (GetKeyboardPress(DIK_U))
	{// 近づく
		g_Camera.len -= VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_M))
	{// 離れる
		g_Camera.len += VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

#endif


	// カメラを初期に戻す
	if (GetKeyboardPress(DIK_R) || IsButtonTriggered(0,BUTTON_L2))
	{
		g_Camera.len += VALUE_MOVE_CAMERA;

		g_ResetTargetPosX = g_Camera.at.x - sinf(player->rot.y + XM_PI) * g_Camera.len;
		g_ResetTargetPosZ = g_Camera.at.z - cosf(player->rot.y + XM_PI) * g_Camera.len;

		player->bulletX = 0.0f;

		g_Camera.reset = TRUE;
	}

	if (g_Camera.reset == TRUE)
	{
		// カメラの注視点
		XMVECTOR vA = XMLoadFloat3(&g_Camera.at);
		// 現在のカメラ
		XMVECTOR vB = XMLoadFloat3(&g_Camera.pos);
		// 移動後の座標
		XMVECTOR vC = XMLoadFloat3(&XMFLOAT3(g_ResetTargetPosX, g_Camera.pos.y, g_ResetTargetPosZ));

		// vABCの正規化
		// 長さを求めて、正規化している(A,B)(A,C)
		XMVECTOR vAB = XMVector3Normalize(vA - vB);
		XMVECTOR vAC = XMVector3Normalize(vA - vC);

		// Cosine値の割り出し 内積を使うとコサインが出るらしい
		// 内積を求めている	XMVector3Dot・・・内積を求める関数(cos値(-1 ~ 1)が求まる)
		float cos = 0.0f;
		//XMStoreFloat(&cos, XMVector3Dot(vAB, vAC));	← 同じ処理
		cos = dotProduct(&vAB, &vAC);

		// acosf = θ（角度が求まる）
		float angle = acosf(cos);

		// 0と0の除算の場合エラーを起こさない。後ろに戻す
		if (isnan(angle))	angle = XM_PI;


		// ABとACの外積 XMVector3Cross = 外積を求める関数
		XMFLOAT3 cross = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMStoreFloat3(&cross, XMVector3Cross(vAB, vAC));

		if (angle > 0.01f)
		{
			// 回転スピード
			if (cross.y > 0.0f)
			{
				g_Camera.rot.y += angle * 0.2f;

				if (g_Camera.rot.y > XM_PI)
				{
					g_Camera.rot.y -= XM_2PI;
				}
			}
			else
			{
				g_Camera.rot.y -= angle * 0.2f;

				if (g_Camera.rot.y < -XM_PI)
				{
					g_Camera.rot.y += XM_2PI;
				}
			}
		}
		else
		{
			g_Camera.reset = FALSE;
		}
	}

	// カメラの上限設定
	if (g_Camera.pos.y >= UPPERLIMIT)
	{
		g_Camera.pos.y = UPPERLIMIT;
	}
	else if(g_Camera.pos.y <= LOWERLIMIT)
	{
		g_Camera.pos.y = LOWERLIMIT;
	}

}


//=============================================================================
// カメラの更新
//=============================================================================
void SetCamera(void) 
{
	// ビューマトリックス設定
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera.mtxInvView, mtxInvView);


	// プロジェクションマトリックス設定
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera.pos);
}


//=============================================================================
// カメラの取得
//=============================================================================
CAMERA *GetCamera(void) 
{
	return &g_Camera;
}

//=============================================================================
// ビューポートの設定
//=============================================================================
void SetViewPort(int type)
{
	ID3D11DeviceContext *g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	g_ViewPortType = type;

	// ビューポート設定
	switch (g_ViewPortType)
	{
	case TYPE_FULL_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_LEFT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_RIGHT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = (FLOAT)SCREEN_WIDTH / 2;
		vp.TopLeftY = 0;
		break;

	case TYPE_UP_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_DOWN_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = (FLOAT)SCREEN_HEIGHT / 2;
		break;


	}
	g_ImmediateContext->RSSetViewports(1, &vp);

}


int GetViewPortType(void)
{
	return g_ViewPortType;
}



// カメラの視点と注視点をセット
void SetCameraAT(XMFLOAT3 pos)
{
	// カメラの注視点を引数の座標にしてみる
	g_Camera.at = pos;

	// カメラの視点をカメラのY軸回転に対応させている
	g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
	g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;

}

