//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "tree.h"
#include "bullet.h"
#include "enemybullet.h"
#include "enemyatc.h"
#include "particle.h"
#include "particlesphere.h"
#include "particleatc.h"
#include "collision.h"
#include "debugproc.h"
#include "sky.h"
#include "ui.h"
#include "sound.h"
#include "particleeffect.h"
#include "bossbullet.h"
#include "enemybossbullet .h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

// TPS���_�̍��W�p
#define		PERSPECTIVE_X		(20)
#define		PERSPECTIVE_Y		(35)
#define		PERSPECTIVE_Z		(-30)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void CheckHit(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// �|�[�YON/OFF


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 150, 13.0f, 13.0f);

	// ���C�g��L����	// �e�̏���������
	InitShadow();

	// �v���C���[�̏�����
	InitPlayer();

	// �G�l�~�[�̏�����
	InitEnemy();

	// �ǂ̏�����
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	//// ��(�����p�̔�����)
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// �؂𐶂₷
	InitTree();

	// �e�̏�����
	InitBullet();
	InitEnemyBullet();
	InitEnemyAtc();
	InitBossBullet();
	InitEnemyBossBullet();

	// UI�̏�����
	InitUi();

	// �p�[�e�B�N���̏�����
	InitParticle();
	InitParticleSphere();
	InitParticleAtc();
	InitParticleEffect();

	// ��̏�����
	InitSky();

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_enemy_bgm);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	// �p�[�e�B�N���̏I������
	UninitParticle();
	UninitParticleSphere();
	UninitParticleAtc();
	UninitParticleEffect();

	//// UI�̏I������
	UninitUi();

	// �e�̏I������
	UninitBullet();
	UninitEnemyBullet();
	UninitEnemyAtc();
	UninitBossBullet();
	UninitEnemyBossBullet();

	// �؂̏I������
	UninitTree();

	// �ǂ̏I������
	UninitMeshWall();

	// �n�ʂ̏I������
	UninitMeshField();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

	// ��̏I������
	UninitSky();

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;

	
	// ���m�N��
	//SetMonochrome(1);

	// �v���C���[�̍X�V����
	UpdatePlayer();

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// �G�l�~�[�̍X�V����
	UpdateEnemy();

	// �Ǐ����̍X�V
	UpdateMeshWall();

	// �؂̍X�V����
	UpdateTree();

	// �e�̍X�V����
	UpdateBullet();
	UpdateEnemyBullet();
	UpdateEnemyAtc();
	UpdateBossBullet();
	UpdateEnemyBossBullet();

	// �p�[�e�B�N���̍X�V����
	UpdateParticle();
	UpdateParticleSphere();
	UpdateParticleAtc();
	UpdateParticleEffect();

	// �e�̍X�V����
	UpdateShadow();

	// �����蔻��
	CheckHit();

	// �X�R�A�̍X�V����
	UpdateUi();

	// ��̍X�V����
	UpdateSky();
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	// 3D�̕���`�悷�鏈��
	
	// ��̕`�揈��
	DrawSky();

	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �e�̕`�揈��
	DrawShadow();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �ǂ̕`�揈��
	DrawMeshWall();

	// �؂̕`�揈��
	DrawTree();

	// �p�[�e�B�N���̕`�揈��
	DrawParticle();
	DrawParticleSphere();
	DrawParticleEffect();

#ifndef _DEBUG

	DrawParticleAtc();

#endif 
	// �e�̕`�揈��
	DrawBullet();
	DrawBossBullet();
	//DrawEnemyAtc();
	DrawEnemyBullet();
	DrawEnemyBossBullet();

	// 2D�̕���`�悷�鏈��
	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	// �X�R�A�̕`�揈��
	DrawUi();

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// Z��r����
	SetDepthEnable(TRUE);


}

void DrawGame(void)
{
	CAMERA* cam = GetCamera();
	PLAYER* player = GetPlayer();
	XMFLOAT3 pos;


#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// TPS���_
	{	// �v���C���[�̒����_�̎擾
		// ���[���h�}�g���b�N�X�̏�����
		// �����s��̏�����
		XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
		mtxWorld = XMMatrixIdentity();

		// �A�t�B���ϊ�
		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(PERSPECTIVE_X, PERSPECTIVE_Y, PERSPECTIVE_Z);// �E�x�N�g��,
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(cam->rot.x, cam->rot.y, cam->rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, mtxWorld);

		// �x�N�g��
		XMFLOAT3 vector;
		// �s��̗v�f�����o���ăx�N�g���ɑ���
		vector.x = result._41;
		vector.y = result._42;
		vector.z = result._43;

		// �v���C���[�̍��W�𑫂�
		vector.x += player->pos.x;
		vector.y += player->pos.y;
		vector.z += player->pos.z;

		SetCameraAT(vector);
	}

	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		//SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		//SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}
}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	// �L�����N�^�[�֌W
	ENEMY *enemy = GetEnemy();						// �G�l�~�[�̃|�C���^�[��������
	ENEMYBOSS* enemyboss = GetEnemyBoss();			// �{�X�̃|�C���^�[�̏�����
	PLAYER *player = GetPlayer();					// �v���C���[�̃|�C���^�[��������

	// �e�֌W
	BULLET *bullet = GetBullet();					// �e�̃|�C���^�[��������
	ENEMYBULLET* enemybullet = GetEnemyBullet();	// �G�l�~�[�̒e�̃|�C���^�[��������
	ENEMYATC* enemyatc = GetEnemyAtc();				// �G�l�~�[�A�^�b�N�̃|�C���^�[�̏�����
	BOSSBULLET* bossbullet = GetBossBullet();		// �x�W�F�Ȑ��o���b�g�̏�����


	//�v���C���[�̒e�ƓG
	{
		for (int i = 0; i < MAX_BULLET; i++)
		{
			//�e�̗L���t���O���`�F�b�N����
			if (bullet[i].use == FALSE)
				continue;

			XMFLOAT3 epos = enemy[1].pos;
			epos.y *= 15.0f;

			//BC�̓����蔻��(�G�l�~�[1�Ԗ�)
			if (CollisionBC(bullet[i].pos, epos, bullet[i].fWidth, enemy[1].size))
			{

				// �G�t�F�N�g�p���W
				for (int p = 0; p < 20; p++)
				{
					XMFLOAT3 move = CalcParticleSparkMove(bullet[i].pos);
					SetParticleEffect(bullet[i].pos, move, PARTICLE_SPARK_LIFE, TRUE);
				}

				// ���ʉ�
				PlaySound(SOUND_LABEL_SE_hit);

				// �����������疢�g�p�ɖ߂�
				bullet[i].use = FALSE;
				ReleaseShadow(bullet[i].shadowIdx);

				// �G�l�~�[�̃��C�t�����炷
				enemy[1].hp -= 1.0f;
				enemy[1].dissolve.threshold += 0.1f;

				if (enemy[1].hp <= 0.0f)
				{
					// �G�L�����N�^�[�͓|�����
					enemy[1].use = FALSE;
					ReleaseShadow(enemy[1].shadowIdx);
				}
			}

			// �G�l�~�[1�Ԗڂ��|���ꂽ��0�ԖڂƓ����蔻��
			if (enemy[1].use == FALSE)
			{
				//BC�̓����蔻��
				if (CollisionBC(bullet[i].pos, enemy[0].pos, bullet[i].fWidth, enemy[0].size))
				{
					// �����������疢�g�p�ɖ߂�
					bullet[i].use = FALSE;
					ReleaseShadow(bullet[i].shadowIdx);

					// ���ʉ�
					PlaySound(SOUND_LABEL_SE_hit);

					// �G�t�F�N�g�p���W
					for (int p = 0; p < 20; p++)
					{
						XMFLOAT3 move = CalcParticleSparkMove(bullet[i].pos);
						SetParticleEffect(bullet[i].pos, move, PARTICLE_SPARK_LIFE, TRUE);
					}

					// �G�l�~�[�̃��C�t�����炷
					enemy[0].hp -= 1.0f;
					enemy[0].dissolve.threshold += 0.1f;
					enemy[0].ans = TRUE;

					if (enemy[0].hp <= 0.0f)
					{
						// �G�L�����N�^�[�͓|�����
						enemy[0].use = FALSE;
						ReleaseShadow(enemy[0].shadowIdx);
					}
				}
			}

			// �{�X�ƃv���C���[�̋��̔���
			//BC�̓����蔻��
			if (enemyboss->use == TRUE)
			{
				if (CollisionBC(bullet[i].pos, enemyboss->pos, bullet[i].fWidth, enemyboss->size))
				{
					// �����������疢�g�p�ɖ߂�
					bullet[i].use = FALSE;
					ReleaseShadow(bullet[i].shadowIdx);

					// ���ʉ�
					PlaySound(SOUND_LABEL_SE_hit);

					// �G�t�F�N�g�p���W
					for (int p = 0; p < 20; p++)
					{
						XMFLOAT3 move = CalcParticleSparkMove(bullet[i].pos);
						SetParticleEffect(bullet[i].pos, move, PARTICLE_SPARK_LIFE, TRUE);
					}

					// �{�X�̃��C�t�����炵�ă_���[�W���J�E���g�����߂�
					enemyboss->hp -= 1.0f;
					enemyboss->dmgcnt += 1.0f;
					if (enemyboss->hp <= 0.0f)
					{
						// �G�L�����N�^�[�͓|�����
						enemyboss->use = FALSE;
						ReleaseShadow(enemyboss->shadowIdx);
					}

					if (enemyboss->hp <= 0.0f)
					{
						SetFadeMode(FADE_OUT, MODE_CLEAR_RESULT);
					}
				}

			}
		}
	}

	// EnemyBullet�ƃv���C���[�̓����蔻��
	{
		for (int i = 0; i < MAX_ENEMYBULLET; i++)
		{
			//�e�̗L���t���O���`�F�b�N����
			if (enemybullet[i].use == FALSE)
				continue;

			//�v���C���[�̗L���t���O���`�F�b�N����
			if (player[0].use == FALSE)
				continue;

			//BC�̓����蔻��
			if (CollisionBC(enemybullet[i].pos, player[0].pos, enemybullet[i].fWidth, player[0].size))
			{
				// �����������疢�g�p�ɖ߂�
				enemybullet[i].use = FALSE;
				ReleaseShadow(enemybullet[i].shadowIdx);
				// �v���C���[�̐F�ύX
				player[0].ans = TRUE;


				// �G�l�~�[�̃��C�t�����炷
				player[0].hp -= 1.0f;

				if (player[0].hp <= 0.0f)
				{
					// �G�L�����N�^�[�͓|�����
					player[0].use = FALSE;
					ReleaseShadow(player[0].shadowIdx);
				}
			}
		}

		// �v���C���[���S�����S�������ԑJ��
		int player_count = 0;
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			if (player[0].use == FALSE) continue;
			player_count++;
		}

		if (player_count == 0)
		{
			SetFadeMode(FADE_OUT, MODE_GAMEOVER_RESULT);
		}

	}

	// EnemyAtc�ƃv���C���[�̓����蔻��
	{
		for (int i = 0; i < ENEMYATC_MAX_BULLET; i++)
		{
			//�e�̗L���t���O���`�F�b�N����
			if (enemyatc[i].use == FALSE)
				continue;

			//�G�̗L���t���O���`�F�b�N����
			if (player[0].use == FALSE)
				continue;

			//BC�̓����蔻��
			if (CollisionBC(enemyatc[i].pos, player[0].pos, enemyatc[i].fWidth, player[0].size))
			{
				// �����������疢�g�p�ɖ߂�
				enemyatc[i].use = FALSE;
				ReleaseShadow(enemyatc[i].shadowIdx);
				// �v���C���[�̐F�ύX
				player[0].ans = TRUE;

				// �G�l�~�[�̃��C�t�����炷
				player[0].hp -= 1.0f;

				if (player[0].hp <= 0.0f)
				{
					// �G�L�����N�^�[�͓|�����
					player[0].use = FALSE;
					ReleaseShadow(player[0].shadowIdx);
				}
			}
		}

		// �v���C���[���S�����S�������ԑJ��
		int player_count = 0;
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			if (player[0].use == FALSE) continue;
			player_count++;
		}

		if (player_count == 0)
		{
			SetFadeMode(FADE_OUT, MODE_GAMEOVER_RESULT);
		}
	}

	// BossBullet�ƃv���C���[�̓����蔻��
	{
		for (int i = 0; i < ENEMYATC_MAX_BULLET; i++)
		{
			//�e�̗L���t���O���`�F�b�N����
			if (bossbullet[i].use == FALSE)
				continue;

			//�G�̗L���t���O���`�F�b�N����
			if (player[0].use == FALSE)
				continue;

			//BC�̓����蔻��
			if (CollisionBC(bossbullet[i].pos, player[0].pos, bossbullet[i].fWidth, player[0].size))
			{
				// �����������疢�g�p�ɖ߂�
				bossbullet[i].use = FALSE;
				ReleaseShadow(enemyatc[i].shadowIdx);
				// �v���C���[�̐F�ύX
				player[0].ans = TRUE;

				// �G�l�~�[�̃��C�t�����炷
				player[0].hp -= 0.1f;

				if (player[0].hp <= 0.0f)
				{
					// �G�L�����N�^�[�͓|�����
					player[0].use = FALSE;
					ReleaseShadow(player[0].shadowIdx);
				}
			}
		}

		// �v���C���[���S�����S�������ԑJ��
		int player_count = 0;
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			if (player[0].use == FALSE) continue;
			player_count++;
		}

		if (player_count == 0)
		{
			SetFadeMode(FADE_OUT, MODE_GAMEOVER_RESULT);
		}
	}
}


