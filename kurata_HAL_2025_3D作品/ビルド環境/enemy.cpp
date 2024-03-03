//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "shadow.h"
#include "player.h"
#include "collision.h"
#include "fade.h"
#include "bullet.h"
#include "enemybullet.h"
#include "enemyatc.h"
#include "particle.h"
#include "particlesphere.h"
#include "particleatc.h"
#include "sound.h"
#include "keyframe.h"
#include "bossbullet.h"
#include "enemybossbullet .h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// ���f���̓ǂݍ���
#define MODEL_ENEMY			"data/MODEL/ENEMY/enemy/enemy01.obj"
#define MODEL_TEMPLE		"data/MODEL/ENEMY/enemy/temple.obj"

// �{�X
#define MODEL_ENEMYBOSS		"data/MODEL/ENEMY/boss/ENEMY_BOSS.obj"
#define MODEL_BOSSRING		"data/MODEL/ENEMY/boss/ENEMY_RING.obj"


// �G�l�~�[
#define	VALUE_MOVE					(2.0f)				// �ړ���
#define	VALUE_ROTATE				(XM_PI * 0.02f)		// ��]��
#define VALUE_SIZE					(5.0f)				// �X�P�[��
#define ENEMY_SHADOW_SIZE			(0.4f)				// �e�̑傫��
#define ENEMY_OFFSET_Y				(30.0f)				// �G�l�~�[�̑��������킹��
// �U���J�E���g
#define ENEMY_USUALLY_CNTLOWER		(1000)				// �ʏ�U���̃J�E���g
#define ENEMY_USUALLY_CNTUPPER		(2000)				// �ʏ�U���̃J�E���g
// ��_���[�W
#define ENEMY_RECEIVEWAIT			(180)				// ��_���[�W���̐F�ύX����
// dissolve
#define ENEMY_DISSOLVE				(1.0f)

// �{�X
#define	VALUE_BOSSMOVE				(2.0f)				// �ړ���
#define	VALUE_BOSSROTATE			(XM_PI * 0.02f)		// ��]��
#define VALUE_BOSSSIZE				(5.0f)				// �X�P�[��
#define ENEMYBOSS_SHADOW_SIZE		(0.4f)				// �e�̑傫��
#define ENEMYBOSS_SPEED				(3.0f)				// �{�X�̈ړ��X�s�[�h
#define ENEMYBOSS_OFFSET_Y			(7.0f)				// �G�l�~�[�̑��������킹��
#define ENEMYBOSS_PARTS_MAX			(2)					// �v���C���[�̃p�[�c�̐�
// �U���J�E���g
#define ENEMYBOSS_USUALLY_CNTLOWER	(1000)			// �ʏ�U���̃J�E���g
#define ENEMYBOSS_USUALLY_CNTUPPER	(2000)			// �ʏ�U���̃J�E���g
// ��_���[�W
#define ENEMBOSS_RECEIVEWAIT		(180)			// ��_���[�W���̐F�ύX����
#define ENEMYBOSS_DMGCNT			(10.0f)			// �_���[�W�J�E���g
// dissolve
#define ENEMYBOSS_DISSOLVE			(1.0f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// �G�l�~�[�����֐�
void INITENEMY(void);
void UPDATEENEMY(void);
void DRAWENEMY(void);

// �{�X�����֐�
void INITBOSS(void);
void UPDATEBOSS(void);
void DRAWBOSS(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];						// �G�l�~�[

static ENEMYBOSS		g_EnemyBoss;							// �G�l�~�[�{�X
static ENEMYBOSS		g_BossParts[ENEMYBOSS_PARTS_MAX];		// �{�X�̃p�[�c�p

int g_Enemy_load = 0;
int g_EnemyBoss_load = 0;

int bosscnt = 0;
BOOL check = 0;

//�@�G�l�~�[
static INTERPOLATION_DATA move_tbl_enemy[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,  30.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,		   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  40.0f, 400.0f), XMFLOAT3(0.0f, XM_PI / 4,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  50.0f, 400.0f), XMFLOAT3(0.0f, XM_PI / 2,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  60.0f, 400.0f), XMFLOAT3(0.0f, XM_PI ,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  70.0f, 400.0f), XMFLOAT3(0.0f, XM_PI * 1.25f,0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  80.0f, 400.0f), XMFLOAT3(0.0f, XM_PI * 1.5f, 0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  30.0f, 400.0f), XMFLOAT3(0.0f, XM_2PI,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE),   1 },
};

static INTERPOLATION_DATA move_tbl_temple[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,		  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_PI / 2,	  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_PI,		  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_PI * 1.5f,  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_2PI,	      0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f),   1 },
};

// �{�X
// �o����
static INTERPOLATION_DATA move_tbl_enemyboss0[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,   10.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,	   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE), 60 },

};

// 1��ڂ̍U��
static INTERPOLATION_DATA move_tbl_enemyboss1[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,   100.0f,  400.0f), XMFLOAT3(0.0f, 0.0f,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(-400.0f, 100.0f, -400.0f), XMFLOAT3(0.0f, XM_PIDIV2,	   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(-750.0f,  50.0f, -750.0f), XMFLOAT3(0.0f, XM_PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3( 750.0f, 50.0f, -750.0f), XMFLOAT3(0.0f, XM_PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3( 400.0f, 50.0f, -400.0f), XMFLOAT3(0.0f, XM_PI * 1.5f,  0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(0.0f,   100.0f,  400.0f), XMFLOAT3(0.0f, XM_2PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(0.0f,    100.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),    1 },

};

// 2��ڂ̍U��
static INTERPOLATION_DATA move_tbl_enemyboss2[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,   100.0f,  400.0f), XMFLOAT3(0.0f, 0.0f,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(400.0f, 100.0f, -400.0f), XMFLOAT3(0.0f, XM_PIDIV2,	   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(750.0f,  50.0f, -750.0f), XMFLOAT3(0.0f, XM_PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(-750.0f, 50.0f, -750.0f), XMFLOAT3(0.0f, XM_PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(-400.0f, 50.0f, -400.0f), XMFLOAT3(0.0f, XM_PI * 1.5f,  0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(0.0f,   100.0f,  400.0f), XMFLOAT3(0.0f, XM_2PI,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),  600 },
	{ XMFLOAT3(0.0f,    100.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,		   0.0f),   XMFLOAT3(VALUE_BOSSSIZE, VALUE_BOSSSIZE, VALUE_BOSSSIZE),    1 },
};														 

static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_enemy,
	move_tbl_temple,
	move_tbl_enemyboss0,
	move_tbl_enemyboss1,
	move_tbl_enemyboss2,

};

enum 
{
	tbl_enemy,
	tbl_temple,
	tbl_enemyboss0,
	tbl_enemyboss1,
	tbl_enemyboss2,
};

static INTERPOLATION_DATA move_tbl_boss[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },


};

static INTERPOLATION_DATA move_tbl_ring1[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI,         0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f,         XM_PI, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI,         0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f,        XM_2PI, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI  * 3.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI,         0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_2PI * 2.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f,          0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f),   1 },
};


static INTERPOLATION_DATA* g_AnimTblAdr[] =
{
	move_tbl_boss,
	move_tbl_ring1,
};

enum ANIM
{
	tbl_boss,
	tbl_ring1,
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{

	// �G�l�~�[
	INITENEMY();

	// �{�X
	INITBOSS();

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}
	// �p�[�c�̉������
	for (int i = 0; i < ENEMYBOSS_PARTS_MAX; i++)
	{
		if (g_BossParts[i].load == TRUE)
		{
			// �p�[�c�̉������
			UnloadModel(&g_BossParts[i].model);
			g_BossParts[i].load = FALSE;
		}
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{

	// �G�l�~�[
	UPDATEENEMY();

	if (g_Enemy[0].chenge == TRUE)
	{
		// �{�X
		UPDATEBOSS();
	}

#ifdef _DEBUG
	
		if (GetKeyboardTrigger(DIK_P))
		{
			// ���f���̐F��ύX�ł����I�������ɂ��ł����B
			for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
			{
				SetModelDiffuse(&g_Enemy[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));
			}
		}

		if (GetKeyboardTrigger(DIK_L))
		{
			// ���f���̐F�����ɖ߂��Ă���
			for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
			{
				SetModelDiffuse(&g_Enemy[0].model, j, g_Enemy[0].diffuse[j]);
			}
		}
#endif

#ifdef _DEBUG
		// �f�o�b�O�\��
		PrintDebugProc("Enemy00:%f,Enemy01:%f\n", g_Enemy[0].hp, g_Enemy[1].hp);
		PrintDebugProc("ENEMYBOSS pos.x:%f,pos.y:%f,pos.z:%f\n", g_EnemyBoss.pos.x, g_EnemyBoss.pos.y, g_EnemyBoss.pos.z);
#endif
	
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{


	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// �G�l�~�[
	DRAWENEMY();

	if (g_Enemy[0].chenge == TRUE)
	{
		// �{�X
		DRAWBOSS();
	}

	SetFuchi(0);

}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

//=============================================================================
// �{�X�̎擾
//=============================================================================
ENEMYBOSS* GetEnemyBoss()
{
	return &g_EnemyBoss;
}


//=============================================================================
// �G�l�~�[�̏����֐�
//=============================================================================
void INITENEMY(void)
{
	LoadModel(MODEL_ENEMY, &g_Enemy[0].model);
	LoadModel(MODEL_TEMPLE, &g_Enemy[1].model);

	for (int i = 0; i < MAX_ENEMY; i++)
	{

		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 0.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		//GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

		g_Enemy[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Enemy[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		g_Enemy[i].use = TRUE;			// TRUE:�����Ă�
		g_Enemy[i].chek = FALSE;		// 
		g_Enemy[i].cnt = 0;				// �U���񐔂��J�E���g����

		// ��_���[�W���̃_���[�W���̕ύX����
		g_Enemy[i].receivewait = 0.0f;

		//// �����蔻��̃`�F�b�N
		//g_Enemy[i].ans = FALSE;

		// dissolve
		g_Enemy[i].dissolve.Enable = TRUE;
		g_Enemy[i].dissolve.threshold = 1.0f;
	}

	// �������W
	g_Enemy[0].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 400.0f);
	g_Enemy[1].pos = XMFLOAT3(0.0f, 1.0f, 400.0f);

	// �X�P�[��
	g_Enemy[0].scl = XMFLOAT3(2.0f, 2.0f, 2.0f);
	g_Enemy[1].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	// ���C�t
	g_Enemy[0].hp = ENEMY_HP;
	g_Enemy[1].hp = TEMPLE_HP;

	// �{�X��ڍs
	g_Enemy[0].chenge = FALSE;


	// �e
	XMFLOAT3 pos = g_Enemy[0].pos;
	pos.y -= (ENEMY_OFFSET_Y - 0.1f);
	g_Enemy[0].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

	//// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[0].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[0].tblNo = tbl_enemy;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[0].tblMax = sizeof(move_tbl_enemy) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	//// 1�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[1].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[1].tblNo = tbl_temple;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[1].tblMax = sizeof(move_tbl_temple) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

}

void UPDATEENEMY(void)
{
	BULLET* bullet = GetBullet();
	ENEMYATC* enemyatc = GetEnemyAtc();
	PLAYER* player = GetPlayer();

	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// �v���C���[���J�n�ʒu�ɂ�����o�ꂳ����
		if (player->chek == FALSE) continue;

		// dissolve
		g_Enemy[i].dissolve.threshold -= 0.01f;
		g_EnemyBoss.dissolve.threshold -= 0.01f;
		if (g_Enemy[i].dissolve.threshold <= 0.0f && g_EnemyBoss.dissolve.threshold <= 0.0f)
		{
			g_Enemy[i].chek = TRUE;
		}

		// dissolve�ŏo��������
		if (g_Enemy[i].chek == FALSE)	continue;

		if (g_Enemy[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes

			// �ړ�����
			if (g_Enemy[i].tblMax > 0)	// ���`��Ԃ����s����H
			{	// ���`��Ԃ̏���
				int nowNo = (int)g_Enemy[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Enemy[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Enemy[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);

				// frame���g�Ď��Ԍo�ߏ���������
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Enemy[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Enemy[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

			}

			// �e���v���C���[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);

			// �ʏ�U������

			XMFLOAT3 epos = g_Enemy[0].pos;
			XMFLOAT3 erot = g_Enemy[0].rot;

			SetEnemyBullet(epos, erot);

			//// �G�l�~�[1�Ԗڂ��|����邩�A�U���J�E���g����萔�̊Ԏ��s����
			//g_Enemy[0].cnt++;
			//if (g_Enemy[1].use == FALSE
			//	&& ENEMY_USUALLY_CNTLOWER <= g_Enemy[0].cnt
			//	&& g_Enemy[0].cnt <= ENEMY_USUALLY_CNTUPPER)
			//{
			//	// �K�E�Z
			//	// �p�[�e�B�N��
			//	SetParticleSphere();

			//	XMFLOAT3 bpos = g_Enemy[0].pos;
			//	XMFLOAT3 brot = g_Enemy[0].rot;

			//	SetEnemyAtc(bpos, brot);

			//}

			//// �U���J�E���g�̏�����
			//if (g_Enemy[0].cnt > ENEMY_USUALLY_CNTUPPER)
			//{
			//	g_Enemy[0].cnt = 0;
			//}

		}
	}

	// �G�l�~�[��|������{�X���
	if (g_Enemy[0].hp <= 0.0f)
	{
		g_Enemy[0].chenge = TRUE;
	}

}

void DRAWENEMY(void)
{
	PLAYER* player = GetPlayer();
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//// dissolve
		SetDissolve(&g_Enemy[i].dissolve);
		SetAlphaTestEnable(g_Enemy[i].dissolve.Enable);

		// �v���C���[���J�n�ʒu�ɂ�����o�ꂳ����
		if (player->chek == FALSE) continue;


		if (g_Enemy[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		//// ���f���`��
		DrawModel(&g_Enemy[i].model);

	}

	//// dissolve �`����FALSE��
	SetDissolveEnable(FALSE);
	SetAlphaTestEnable(g_Enemy[0].dissolve.Enable);
	SetAlphaTestEnable(g_Enemy[1].dissolve.Enable);


}


// �{�X�����֐�
void INITBOSS(void)
{

	g_EnemyBoss.pos = XMFLOAT3(0.0f, ENEMYBOSS_OFFSET_Y, 400.0f);
	g_EnemyBoss.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_EnemyBoss.hp = ENEMYBOSS_HP;
	g_EnemyBoss.spd = 0.0f;				// �ړ��X�s�[�h�N���A
	g_EnemyBoss.size = ENEMYBOSS_SIZE;	// �����蔻��̑傫��
	g_EnemyBoss.use = FALSE;			// TRUE:�����Ă�

	g_EnemyBoss.time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_EnemyBoss.tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
	g_EnemyBoss.tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

	g_EnemyBoss.chek = FALSE;		// 
	g_EnemyBoss.cnt = 0;			// �U���񐔂��J�E���g����
	g_EnemyBoss.dmgcnt = 0.0f;		// �_���[�W���J�E���g

	// ��_���[�W���̃_���[�W���̕ύX����
	g_EnemyBoss.receivewait = 0.0f;

	//// �����蔻��̃`�F�b�N
	//g_EnemyBoss[i].ans = FALSE;

	// dissolve
	g_EnemyBoss.dissolve.Enable = TRUE;
	g_EnemyBoss.dissolve.threshold = 1.0f;

	//// 0�Ԃ������`��Ԃœ������Ă݂�
	g_EnemyBoss.time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_EnemyBoss.tblNo = tbl_enemyboss0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_EnemyBoss.tblMax = sizeof(move_tbl_enemyboss0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g


	//�K�w�A�j���[�V�����̏�����
	for (int j = 0; j < ENEMYBOSS_PARTS_MAX; j++)
	{
		g_BossParts[j].use = FALSE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_BossParts[j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BossParts[j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BossParts[j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// �e�q�֌W
		g_BossParts[j].parent = &g_EnemyBoss;		// �� �����ɐe�̃A�h���X������
	//	g_BossParts[�r].parent= &g_Player;		// �r��������e�͖{�́i�v���C���[�j
	//	g_BossParts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

		// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		g_BossParts[j].time   = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_BossParts[j].tblNo  = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_BossParts[j].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
		g_BossParts[j].load = FALSE;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		//GetModelDiffuse(&g_BossParts[j].model, &g_BossParts[j].diffuse[0]);

	}

	// �L�[�t���[���ύX����
	g_BossParts[0].use = TRUE;
	g_BossParts[0].parent = &g_EnemyBoss;	// �e���Z�b�g
	g_BossParts[0].tblNo = tbl_boss;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_BossParts[0].tblMax = sizeof(move_tbl_boss) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	//g_BossParts[0].tblMax = (int)g_TblAdr[g_BossParts[0].tblNo]->size();	// �� �ύX����
	g_BossParts[0].load = TRUE;
	LoadModel(MODEL_ENEMYBOSS, &g_BossParts[0].model);

	// �L�[�t���[���ύX����
	g_BossParts[1].use = TRUE;
	g_BossParts[1].parent = &g_BossParts[0];	// �e���Z�b�g
	g_BossParts[1].tblNo = tbl_ring1;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_BossParts[1].tblMax = sizeof(move_tbl_ring1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	//g_BossParts[1].tblMax = (int)g_TblAdr[g_BossParts[1].tblNo]->size();	// �� �ύX����
	g_BossParts[1].load = TRUE;
	LoadModel(MODEL_BOSSRING, &g_BossParts[1].model);

}

void UPDATEBOSS(void)
{
	PLAYER* player = GetPlayer();

	// TRUE:�����Ă�
	g_EnemyBoss.use = TRUE;	

	g_EnemyBoss.oldpos = g_EnemyBoss.pos;

	XMFLOAT3 changepos = XMFLOAT3(0.0f,100.0f,400.0f);


	// �G�l�~�[�̍s���p�^�[��
	if (   g_EnemyBoss.pos.x == changepos.x
		&& g_EnemyBoss.pos.y == changepos.y
		&& g_EnemyBoss.pos.z == changepos.z)
	{
		SetSepia(0);

		bosscnt += 2;
		check = bosscnt % 2;
		// �_���[�W���J�E���g��������
		g_EnemyBoss.dmgcnt = 0.0f;
	}

	switch (check)
	{
	case 0:
		g_EnemyBoss.tblNo = tbl_enemyboss2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_EnemyBoss.tblMax = sizeof(move_tbl_enemyboss2) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// �x�W�F�Ȑ�
		XMFLOAT3 bpos = g_EnemyBoss.pos;
		bpos.y += 5.0f;
		SetBossBullet(bpos, g_EnemyBoss.rot, player->pos);

		SetSepia(1);

		// �͎��_���[�W�J�E���g���������Ȃ�{�X�̍U�����~�߂�
		if (g_EnemyBoss.dmgcnt >= ENEMYBOSS_DMGCNT)
		{
			break;
		}

		break;

	case 1:
		g_EnemyBoss.tblNo  = tbl_enemyboss1;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_EnemyBoss.tblMax = sizeof(move_tbl_enemyboss1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// �x�W�F�Ȑ�
		bpos = g_EnemyBoss.pos;
		bpos.y += 5.0f;
		SetBossBullet(bpos, g_EnemyBoss.rot, player->pos);

		// �͎��_���[�W�J�E���g���������Ȃ�{�X�̍U�����~�߂�
		if (g_EnemyBoss.dmgcnt >= ENEMYBOSS_DMGCNT)
		{
			break;
		}

		break;

	default:
		break;
	}

	// �ړ�����
	if (g_EnemyBoss.tblMax > 0)	// ���`��Ԃ����s����H
	{	// ���`��Ԃ̏���
		int nowNo = (int)g_EnemyBoss.time;			// �������ł���e�[�u���ԍ������o���Ă���
		int maxNo = g_EnemyBoss.tblMax;				// �o�^�e�[�u�����𐔂��Ă���
		int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
		INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_EnemyBoss.tblNo];	// �s���e�[�u���̃A�h���X���擾

		XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
		XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
		XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

		XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
		XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
		XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

		float nowTime = g_EnemyBoss.time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

		Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
		Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
		Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

		// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
		XMStoreFloat3(&g_EnemyBoss.pos, nowPos + Pos);

		// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
		XMStoreFloat3(&g_EnemyBoss.rot, nowRot + Rot);

		// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
		XMStoreFloat3(&g_EnemyBoss.scl, nowScl + Scl);

		// frame���g�Ď��Ԍo�ߏ���������
		g_EnemyBoss.time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
		if ((int)g_EnemyBoss.time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
		{
			g_EnemyBoss.time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
		}
	}

	// �K�w�A�j���[�V����
	for (int j = 0; j < ENEMYBOSS_PARTS_MAX; j++)
	{
		// �g���Ă���Ȃ珈������
		if ((g_BossParts[j].use == TRUE) && (g_BossParts[j].tblMax > 0))
		{	// ���`��Ԃ̏���
			int nowNo = (int)g_BossParts[j].time;			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = g_BossParts[j].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
			INTERPOLATION_DATA* tbl = g_AnimTblAdr[g_BossParts[j].tblNo];	// �s���e�[�u���̃A�h���X���擾

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

			float nowTime = g_BossParts[j].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

			Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
			Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
			Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

			// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
			XMStoreFloat3(&g_BossParts[j].pos, nowPos + Pos);

			// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_BossParts[j].rot, nowRot + Rot);

			// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_BossParts[j].scl, nowScl + Scl);

			// frame���g�Ď��Ԍo�ߏ���������
			g_BossParts[j].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
			if ((int)g_BossParts[j].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				g_BossParts[j].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���

			}
		}
	}
}

void DRAWBOSS(void)
{
	if (g_EnemyBoss.use == FALSE) return;
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// dissolve
	SetDissolve(&g_EnemyBoss.dissolve);
	SetAlphaTestEnable(g_EnemyBoss.dissolve.Enable);

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_EnemyBoss.scl.x, g_EnemyBoss.scl.y, g_EnemyBoss.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_EnemyBoss.rot.x, g_EnemyBoss.rot.y + XM_PI, g_EnemyBoss.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//// �N�H�[�^�j�I���𔽉f
	//quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_EnemyBoss.Quaternion));
	//mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_EnemyBoss.pos.x, g_EnemyBoss.pos.y, g_EnemyBoss.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_EnemyBoss.mtxWorld, mtxWorld);

	// ���f���`��
	DrawModel(&g_EnemyBoss.model);

	// dissolve �`����FALSE��
	SetDissolveEnable(FALSE);
	SetAlphaTestEnable(g_EnemyBoss.dissolve.Enable);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);


	// �K�w�A�j���[�V����
	for (int i = 0; i < ENEMYBOSS_PARTS_MAX; i++)
	{
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_BossParts[i].scl.x, g_BossParts[i].scl.y, g_BossParts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_BossParts[i].rot.x, g_BossParts[i].rot.y, g_BossParts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_BossParts[i].pos.x, g_BossParts[i].pos.y, g_BossParts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_BossParts[i].parent != NULL)	// �q����������e�ƌ�������
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_BossParts[i].parent->mtxWorld));
			// ��
			// g_Player.mtxWorld���w���Ă���
		}

		XMStoreFloat4x4(&g_BossParts[i].mtxWorld, mtxWorld);

		// �g���Ă���Ȃ珈������
		if (g_BossParts[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// ���f���`��
		DrawModel(&g_BossParts[i].model);
	}
}

