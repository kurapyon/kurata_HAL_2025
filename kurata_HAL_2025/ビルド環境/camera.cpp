//=============================================================================
//
// �J�������� [camera.cpp]
// Author : GP11A132 09 �q�c�E��
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
// �}�N����`
//*****************************************************************************
#define	POS_X_CAM			(0.0f)			// �J�����̏����ʒu(X���W)
#define	POS_Y_CAM			(50.0f)			// �J�����̏����ʒu(Y���W)
#define	POS_Z_CAM			(-140.0f)		// �J�����̏����ʒu(Z���W)

//#define	POS_X_CAM		(0.0f)			// �J�����̏����ʒu(X���W)
//#define	POS_Y_CAM		(200.0f)		// �J�����̏����ʒu(Y���W)
//#define	POS_Z_CAM		(-400.0f)		// �J�����̏����ʒu(Z���W)


#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// �r���[���ʂ̎���p
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��	
#define	VIEW_NEAR_Z		(10.0f)											// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z		(10000.0f)										// �r���[���ʂ�FarZ�l

#define	VALUE_MOVE_CAMERA	(0.5f)				// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.005f)	// �J�����̉�]��

#define UPPERLIMIT			(50.0f)				// �J�����̏��
#define LOWERLIMIT			(0.0f)				// �J�����̉���


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CAMERA			g_Camera;			// �J�����f�[�^

static int				g_ViewPortType = TYPE_FULL_SCREEN;

static float            g_ResetTargetPosX;	//���Z�b�g��̃J�������W
static float            g_ResetTargetPosZ;	//���Z�b�g��̃J�������W

//=============================================================================
// ����������
//=============================================================================
void InitCamera(void)
{
	g_Camera.pos	= { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
	g_Camera.at		= { 0.0f, 0.0f, 0.0f };
	g_Camera.up		= { 0.0f, 1.0f, 0.0f };
	g_Camera.rot	= { 0.0f, 0.0f, 0.0f };
	g_Camera.spd = 5.0f;
	g_Camera.reset = FALSE;


	// ���_�ƒ����_�̋������v�Z
	float vx, vz;
	vx = g_Camera.pos.x - g_Camera.at.x;
	vz = g_Camera.pos.z - g_Camera.at.z;
	//g_Camera.len = sqrtf(vx * vx + vz * vz);
	g_Camera.len = 100.0f;

	
	// �r���[�|�[�g�^�C�v�̏�����
	SetViewPort(g_ViewPortType);
}


//=============================================================================
// �J�����̏I������
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// �J�����̍X�V����
//=============================================================================
void UpdateCamera(void)
{

	PLAYER* player = GetPlayer();

//#ifdef _DEBUG
	
	if (GetKeyboardPress(DIK_A) || CameraLeftStick())
	{// ���_����u���v
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_D) || CameraRightStick())
	{// ���_����u�E�v
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_S) || CameraUpStick())
	{// ���_�ړ��u��v
		g_Camera.pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_W) || CameraDownStick())
	{// ���_�ړ��u���v
		g_Camera.pos.y += VALUE_MOVE_CAMERA;
	}

#ifdef _DEBAG
	if (GetKeyboardPress(DIK_U))
	{// �߂Â�
		g_Camera.len -= VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_M))
	{// �����
		g_Camera.len += VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

#endif


	// �J�����������ɖ߂�
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
		// �J�����̒����_
		XMVECTOR vA = XMLoadFloat3(&g_Camera.at);
		// ���݂̃J����
		XMVECTOR vB = XMLoadFloat3(&g_Camera.pos);
		// �ړ���̍��W
		XMVECTOR vC = XMLoadFloat3(&XMFLOAT3(g_ResetTargetPosX, g_Camera.pos.y, g_ResetTargetPosZ));

		// vABC�̐��K��
		// ���������߂āA���K�����Ă���(A,B)(A,C)
		XMVECTOR vAB = XMVector3Normalize(vA - vB);
		XMVECTOR vAC = XMVector3Normalize(vA - vC);

		// Cosine�l�̊���o�� ���ς��g���ƃR�T�C�����o��炵��
		// ���ς����߂Ă���	XMVector3Dot�E�E�E���ς����߂�֐�(cos�l(-1 ~ 1)�����܂�)
		float cos = 0.0f;
		//XMStoreFloat(&cos, XMVector3Dot(vAB, vAC));	�� ��������
		cos = dotProduct(&vAB, &vAC);

		// acosf = �Ɓi�p�x�����܂�j
		float angle = acosf(cos);

		// 0��0�̏��Z�̏ꍇ�G���[���N�����Ȃ��B���ɖ߂�
		if (isnan(angle))	angle = XM_PI;


		// AB��AC�̊O�� XMVector3Cross = �O�ς����߂�֐�
		XMFLOAT3 cross = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMStoreFloat3(&cross, XMVector3Cross(vAB, vAC));

		if (angle > 0.01f)
		{
			// ��]�X�s�[�h
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

	// �J�����̏���ݒ�
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
// �J�����̍X�V
//=============================================================================
void SetCamera(void) 
{
	// �r���[�}�g���b�N�X�ݒ�
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera.mtxInvView, mtxInvView);


	// �v���W�F�N�V�����}�g���b�N�X�ݒ�
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera.pos);
}


//=============================================================================
// �J�����̎擾
//=============================================================================
CAMERA *GetCamera(void) 
{
	return &g_Camera;
}

//=============================================================================
// �r���[�|�[�g�̐ݒ�
//=============================================================================
void SetViewPort(int type)
{
	ID3D11DeviceContext *g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	g_ViewPortType = type;

	// �r���[�|�[�g�ݒ�
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



// �J�����̎��_�ƒ����_���Z�b�g
void SetCameraAT(XMFLOAT3 pos)
{
	// �J�����̒����_�������̍��W�ɂ��Ă݂�
	g_Camera.at = pos;

	// �J�����̎��_���J������Y����]�ɑΉ������Ă���
	g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
	g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;

}

