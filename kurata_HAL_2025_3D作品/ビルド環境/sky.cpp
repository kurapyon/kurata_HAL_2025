//=============================================================================
//
// �󏈗� [sky.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "shadow.h"
#include "player.h"
#include "sky.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MODEL_SKY			"data/MODEL/sky/sky.obj"

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��
#define VALUE_SIZE			(1.0f)						// �X�P�[��

#define SKY_SHADOW_SIZE		(0.4f)						// �e�̑傫��
#define SKY_OFFSET_Y		(0.0f)						// �G�l�~�[�̑��������킹��
		
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static SKY			g_Sky[MAX_SKY];						// �G�l�~�[

int g_Sky_load = 0;


static INTERPOLATION_DATA move_tbl_sky[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,		  0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), 480 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 2,	  0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), 480 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI,		  0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), 480 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 1.5f,0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), 480 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_2PI,	  0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f),   1 },
};

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_sky,
};

enum
{
	tbl_sky,
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		LoadModel(MODEL_SKY, &g_Sky[i].model);
		g_Sky[i].load = TRUE;

		g_Sky[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Sky[i].spd  = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Sky[i].size = SKY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Sky[i].model, &g_Sky[i].diffuse[0]);
		
		g_Sky[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Sky[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Sky[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		g_Sky[i].use = TRUE;			// TRUE:�����Ă�
	}

	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Sky[0].time   = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
	g_Sky[0].tblNo  = tbl_sky;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Sky[0].tblMax = sizeof(move_tbl_sky) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].load)
		{
			UnloadModel(&g_Sky[i].model);
			g_Sky[i].load = FALSE;
		}
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSky(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes

			// �ړ�����
			if (g_Sky[i].tblMax > 0)	// ���`��Ԃ����s����H
			{	// ���`��Ԃ̏���
				int nowNo = (int)g_Sky[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Sky[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Sky[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Sky[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Sky[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Sky[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Sky[i].scl, nowScl + Scl);

				// frame���g�Ď��Ԍo�ߏ���������
				g_Sky[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Sky[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Sky[i].time -= maxNo;					// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}
			}
		}
	}

//#ifdef _DEBUG
//
//#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSky(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Sky[i].scl.x * VALUE_SIZE, g_Sky[i].scl.y * VALUE_SIZE, g_Sky[i].scl.z * VALUE_SIZE);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Sky[i].rot.x, g_Sky[i].rot.y + XM_PI, g_Sky[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Sky[i].pos.x, g_Sky[i].pos.y, g_Sky[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Sky[i].mtxWorld, mtxWorld);

		//// ���f���`��
		DrawModel(&g_Sky[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_NONE);

}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
SKY *GetSky()
{
	return &g_Sky[0];
}
