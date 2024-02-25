//=============================================================================
//
// ���f������ [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "enemy.h"
#include "collision.h"
#include "bullet.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/player.obj"			// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(2.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define PLAYER_SHADOW_SIZE	(0.4f)							// �e�̑傫��
#define PLAYER_OFFSET_Y		(7.0f)							// �v���C���[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER				g_Player[MAX_PLAYER];			// �v���C���[


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		LoadModel(MODEL_PLAYER, &g_Player[i].model);
		g_Player[i].load = true;

		g_Player[i].pos = { 0.0f, PLAYER_OFFSET_Y, 0.0f };
		g_Player[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Player[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Player[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Player[i].size = PLAYER_SIZE;	// �����蔻��̑傫��

		g_Player[i].use = true;

		// �����Ńv���C���[�p�̉e���쐬���Ă���
		XMFLOAT3 pos = g_Player[i].pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		g_Player[i].shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
		//          ��
		//        ���̃����o�[�ϐ������������e��Index�ԍ�


	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		// ���f���̉������
		if (g_Player[i].load)
		{
			UnloadModel(&g_Player[i].model);
			g_Player[i].load = false;
		}

	}


}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		// �ړ��������Ⴄ
		if (GetKeyboardPress(DIK_LEFT))
		{	// ���ֈړ�
			g_Player[i].spd = VALUE_MOVE;
			g_Player[i].dir = XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_RIGHT))
		{	// �E�ֈړ�
			g_Player[i].spd = VALUE_MOVE;
			g_Player[i].dir = -XM_PI / 2;
		}
		if (GetKeyboardPress(DIK_UP))
		{	// ��ֈړ�
			g_Player[i].spd = VALUE_MOVE;
			g_Player[i].dir = XM_PI;
		}
		if (GetKeyboardPress(DIK_DOWN))
		{	// ���ֈړ�
			g_Player[i].spd = VALUE_MOVE;
			g_Player[i].dir = 0.0f;
		}

		// �e����
		if (GetKeyboardTrigger(DIK_SPACE))
		{
			XMFLOAT3 pos = g_Player->pos;
			XMFLOAT3 rot = g_Player->rot;

			SetBullet(pos, rot);

		}

#ifdef _DEBUG
		if (GetKeyboardPress(DIK_R))
		{
			g_Player[i].pos.z = g_Player[i].pos.x = 0.0f;
			g_Player[i].rot.y = g_Player[i].dir = 0.0f;
			g_Player[i].spd = 0.0f;
		}
#endif


		//	// Key���͂���������ړ���������
		if (g_Player[i].spd > 0.0f)
		{
			g_Player[i].rot.y = g_Player[i].dir + cam->rot.y;

			// ���͂̂����������փv���C���[���������Ĉړ�������
			g_Player[i].pos.x -= sinf(g_Player[i].rot.y) * g_Player[i].spd;
			g_Player[i].pos.z -= cosf(g_Player[i].rot.y) * g_Player[i].spd;



			// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
			ENEMY* enemy = GetEnemy();
			for (int j = 0; j < MAX_ENEMY; j++)
			{
				// �����G�l�~�[�������Ă�����
				if (enemy[j].use == TRUE)
				{
					BOOL ans = CollisionBC(
						g_Player[i].pos,
						enemy[j].pos,
						g_Player[i].size,
						enemy[j].size);

					// �����������̏���
					if (ans == TRUE)
					{
						enemy[j].use = FALSE;

						// �e�������Ă���
						ReleaseShadow(enemy[j].shadowIdx);

						break;
					}
				}
			}

			// ���������Ă���
			g_Player[i].spd *= 0.5f;

		}

		// �e���v���C���[�̈ʒu�ɍ��킹��
		XMFLOAT3 pos = g_Player[i].pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		SetPositionShadow(g_Player[i].shadowIdx, pos);




		//{	// �|�C���g���C�g�̃e�X�g
		//	LIGHT *light = GetLightData(1);
		//	XMFLOAT3 pos = g_Player[i].pos;
		//	pos.y += 20.0f;

		//	light->Position = pos;
		//	light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		//	light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		//	light->Type = LIGHT_TYPE_POINT;
		//	light->Enable = true;
		//	SetLightData(1, light);
		//}




#ifdef _DEBUG	// �f�o�b�O����\������
		PrintDebugProc("Player:�� �� �� ���@Space\n");
		PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
#endif

	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Player[i].scl.x, g_Player[i].scl.y, g_Player[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Player[i].model);

	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player[0];
}

