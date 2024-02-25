//=============================================================================
//
// ���f������ [player.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "bullet.h"
#include "debugproc.h"
#include "meshfield.h"
#include "collision.h"
#include "enemy.h"
#include "fade.h"
#include "sound.h"
#include "keyframe.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

// ���f����
#define	MODEL_PLAYER_BODY		"data/MODEL/PLAYER/Player/player_body.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_HEAD		"data/MODEL/PLAYER/Player/player_head.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_ARM_L		"data/MODEL/PLAYER/Player/player_arm_l.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_ARM_R		"data/MODEL/PLAYER/Player/player_arm_r.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_THINGS_L	"data/MODEL/PLAYER/Player/player_things_l.obj"	// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_THINGS_R	"data/MODEL/PLAYER/Player/player_things_r.obj"	// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LEG_L		"data/MODEL/PLAYER/Player/player_leg_l.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LEG_R		"data/MODEL/PLAYER/Player/player_leg_r.obj"		// �ǂݍ��ރ��f����

//--------------------------------------------------------------------------------------
// �L�[�t���[���ύX����
// �A�j���[�V�����̃f�B���N�g��
// �ҋ@���
#define ANIM_STAY_BODY			"data/ANIMDATA/Player_Stay/Player_Body.csv"
#define ANIM_STAY_HEAD			"data/ANIMDATA/Player_Stay/Player_Head.csv"
#define ANIM_STAY_LARM			"data/ANIMDATA/Player_Stay/Player_Arm_L.csv"
#define ANIM_STAY_RARM			"data/ANIMDATA/Player_Stay/Player_Arm_R.csv"
#define ANIM_STAY_LTHINGS		"data/ANIMDATA/Player_Stay/Player_Things_L.csv"
#define ANIM_STAY_RTHINGS		"data/ANIMDATA/Player_Stay/Player_Things_R.csv"
#define ANIM_STAY_LLEG			"data/ANIMDATA/Player_Stay/Player_Leg_L.csv"
#define ANIM_STAY_RLEG			"data/ANIMDATA/Player_Stay/Player_Leg_R.csv"

// �ړ����
#define ANIM_Walk_BODY			"data/ANIMDATA/Player_Walk/Player_Body.csv"
#define ANIM_Walk_HEAD			"data/ANIMDATA/Player_Walk/Player_Head.csv"
#define ANIM_Walk_LARM			"data/ANIMDATA/Player_Walk/Player_Arm_L.csv"
#define ANIM_Walk_RARM			"data/ANIMDATA/Player_Walk/Player_Arm_R.csv"
#define ANIM_Walk_LTHINGS		"data/ANIMDATA/Player_Walk/Player_Things_L.csv"
#define ANIM_Walk_RTHINGS		"data/ANIMDATA/Player_Walk/Player_Things_R.csv"
#define ANIM_Walk_LLEG			"data/ANIMDATA/Player_Walk/Player_Leg_L.csv"
#define ANIM_Walk_RLEG			"data/ANIMDATA/Player_Walk/Player_Leg_R.csv"

// �U�����
#define ANIM_ATK_BODY			"data/ANIMDATA/Player_Atc/Player_Body.csv"
#define ANIM_ATK_HEAD			"data/ANIMDATA/Player_Atc/Player_Head.csv"
#define ANIM_ATK_LARM			"data/ANIMDATA/Player_Atc/Player_Arm_L.csv"
#define ANIM_ATK_RARM			"data/ANIMDATA/Player_Atc/Player_Arm_R.csv"
#define ANIM_ATK_LTHINGS		"data/ANIMDATA/Player_Atc/Player_Things_L.csv"
#define ANIM_ATK_RTHINGS		"data/ANIMDATA/Player_Atc/Player_Things_R.csv"
#define ANIM_ATK_LLEG			"data/ANIMDATA/Player_Atc/Player_Leg_L.csv"
#define ANIM_ATK_RLEG			"data/ANIMDATA/Player_Atc/Player_Leg_R.csv"

// ���ǉ����������͂����ɒǉ�
//--------------------------------------------------------------------------------------

#define	VALUE_MOVE					(7.0f)				// �ړ���
#define	VALUE_ROTATE				(D3DX_PI * 0.02f)	// ��]��

#define PLAYER_SHADOW_SIZE			(0.4f)				// �e�̑傫��
#define PLAYER_OFFSET_Y				(30.0f)				// �v���C���[�̑��������킹��

#define PLAYER_PARTS_MAX			(8)					// �v���C���[�̃p�[�c�̐�

#define GRAVITY_PLUS				(10000)				// �d�͔���
#define GRAVITY_MINUS				(-100.0)			// �d�͔���
#define GRAVITY						(15.0)

#define PLAYER_PARTS_SIZE			(2.0f)				// �v���C���[�̃p�[�c�̃T�C�Y

// �W�����v����
#define	PLAYER_JUMP_CNT_MAX			(30)				// 30�t���[���Œ��n����
#define	PLAYER_JUMP_Y_MAX			(30.0f)				// �W�����v�̍���

// �e�̔���
#define PLAYER_BULLET				(10.0f)				// �e�̔��˗p

// ��_���[�W
#define PLAYER_RECEIVEWAIT			(180)				// ��_���[�W���̐F�ύX����

// �Q�[���J�n�ʒu
#define START_POS_X_1				(  22.0f)
#define START_POS_X_2				( -60.0f)
#define START_POS_Z_1				( 200.0f)
#define START_POS_Z_2				(-450.0f)

//--------------------------------------------------------------------------------------
// �L�[�t���[���ύX����
// �A�j���[�V�����̃��X�g
enum ANIM_LIST
{
	// �ҋ@���
	STAY_BODY,
	STAY_HEAD,
	STAY_LARM,
	STAY_RARM,
	STAY_LTHINGS,
	STAY_RTHINGS,
	STAY_LLEG,
	STAY_RLEG,

	// �ړ����
	WALK_BODY,
	WALK_HEAD,
	WALK_LARM,
	WALK_RARM,
	WALK_LTHINGS,
	WALK_RTHINGS,
	WALK_LLEG,
	WALK_RLEG,

	// �U�����
	ATK_BODY,
	ATK_HEAD,
	ATK_LARM,
	ATK_RARM,
	ATK_LTHINGS,
	ATK_RTHINGS,
	ATK_LLEG,
	ATK_RLEG,

	// ���ǉ����������͂����ɒǉ�

	ANIM_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InputWalkPlayer(void);
void InputAttackPlayer(void);

void WalkPlayer(void);

void AttackPlayer(void);

void SetTblStay(void);
void SetTblWalk(void);
void SetTblAttack(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player[MAX_PLAYER];					// �v���C���[

static PLAYER		g_Parts[PLAYER_PARTS_MAX];				// �v���C���[�̃p�[�c�p

static PLAYER_MODEL g_PartModel[PLAYER_PARTS_MAX];			// �v���C���[���f��

static char g_ModelName[PLAYER_PARTS_MAX][256]
{
	MODEL_PLAYER_BODY,
	MODEL_PLAYER_HEAD,
	MODEL_PLAYER_ARM_L,
	MODEL_PLAYER_ARM_R,
	MODEL_PLAYER_THINGS_L,
	MODEL_PLAYER_THINGS_R,
	MODEL_PLAYER_LEG_L,
	MODEL_PLAYER_LEG_R,
};

static float		roty = 0.0f;

static LIGHT		g_Light;

// �W�����v�֌W
static int      g_jumpCnt = 0;


// �v���C���[�̊K�w�A�j���[�V�����f�[�^
static char* g_AnimDir[] = 
{
	// �ҋ@���
	ANIM_STAY_BODY,
	ANIM_STAY_HEAD,
	ANIM_STAY_LARM,
	ANIM_STAY_RARM,
	ANIM_STAY_LTHINGS,
	ANIM_STAY_RTHINGS,
	ANIM_STAY_LLEG,
	ANIM_STAY_RLEG,

	// �ړ����
	ANIM_Walk_BODY,
	ANIM_Walk_HEAD,
	ANIM_Walk_LARM,
	ANIM_Walk_RARM,
	ANIM_Walk_LTHINGS,
	ANIM_Walk_RTHINGS,
	ANIM_Walk_LLEG,
	ANIM_Walk_RLEG,

	// �U�����
	ANIM_ATK_BODY,
	ANIM_ATK_HEAD,
	ANIM_ATK_LARM,
	ANIM_ATK_RARM,
	ANIM_ATK_LTHINGS,
	ANIM_ATK_RTHINGS,
	ANIM_ATK_LLEG,
	ANIM_ATK_RLEG,
};

//--------------------------------------------------------------------------------------
// �L�[�t���[���ύX����
// �v���C���[�̊K�w�A�j���[�V�����f�[�^
// �ҋ@���
static std::vector<INTERPOLATION_DATA> stay_body;
static std::vector<INTERPOLATION_DATA> stay_head;
static std::vector<INTERPOLATION_DATA> stay_larm;
static std::vector<INTERPOLATION_DATA> stay_rarm;
static std::vector<INTERPOLATION_DATA> stay_lthings;
static std::vector<INTERPOLATION_DATA> stay_rthings;
static std::vector<INTERPOLATION_DATA> stay_lleg;
static std::vector<INTERPOLATION_DATA> stay_rleg;

// �ړ����
static std::vector<INTERPOLATION_DATA> walk_body;
static std::vector<INTERPOLATION_DATA> walk_head;
static std::vector<INTERPOLATION_DATA> walk_larm;
static std::vector<INTERPOLATION_DATA> walk_rarm;
static std::vector<INTERPOLATION_DATA> walk_lthings;
static std::vector<INTERPOLATION_DATA> walk_rthings;
static std::vector<INTERPOLATION_DATA> walk_lleg;
static std::vector<INTERPOLATION_DATA> walk_rleg;

// �U�����
static std::vector<INTERPOLATION_DATA> atk_body;
static std::vector<INTERPOLATION_DATA> atk_head;
static std::vector<INTERPOLATION_DATA> atk_larm;
static std::vector<INTERPOLATION_DATA> atk_rarm;
static std::vector<INTERPOLATION_DATA> atk_lthings;
static std::vector<INTERPOLATION_DATA> atk_rthings;
static std::vector<INTERPOLATION_DATA> atk_lleg;
static std::vector<INTERPOLATION_DATA> atk_rleg;



//--------------------------------------------------------------------------------------
// �L�[�t���[���ύX����
// �v���C���[�̊K�w�A�j���[�V�����f�[�^�̔z��
static std::vector<INTERPOLATION_DATA>* g_TblAdr[] =
{
	// �ҋ@���
	&stay_body,
	&stay_head,
	&stay_larm,
	&stay_rarm,
	&stay_lthings,
	&stay_rthings,
	&stay_lleg,
	&stay_rleg,

	// �ړ����
	 &walk_body,
	 &walk_head,
	 &walk_larm,
	 &walk_rarm,
	 &walk_lthings,
	 &walk_rthings,
	 &walk_lleg,
	 &walk_rleg,
	
	// �U�����
	&atk_body,
	&atk_head,
	&atk_larm,
	&atk_rarm,
	&atk_lthings,
	&atk_rthings,
	&atk_lleg,
	&atk_rleg,

};



//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	//--------------------------------------------------------------------------------------
	// �L�[�t���[���ύX����
	// �A�j���[�V�����f�[�^���t�@�C������ǂݍ��� 
	for (int i = 0; i < ANIM_MAX; i++)
	{
		LoadAnimDataCSV(g_AnimDir[i], *g_TblAdr[i]);
	}
	//--------------------------------------------------------------------------------------

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		//g_Player[i].load = TRUE;
		//LoadModel(MODEL_PLAYER, &g_Player[i].model);

		g_Player[i].pos = XMFLOAT3(22.0f, PLAYER_OFFSET_Y, -600.0f);
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Player[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Player[i].ans = FALSE;		// �����蔻��i�_���[�W���󂯂����ǂ����j
		g_Player[i].use = TRUE;			// TRUE:�����Ă�
		g_Player[i].size = PLAYER_SIZE;	// �����蔻��̑傫��

		// �W�����v�̏�����
		g_Player[i].jump		= FALSE;
		g_Player[i].jumpCnt		= 0;
		g_Player[i].jumpY		= 0.0f;
		g_Player[i].jumpYMax	= PLAYER_JUMP_Y_MAX;
		g_Player[i].jump_pos	= 0.0f;

		// �o���b�g
		g_Player[i].bulletX = 0.0f;

		// �v���C���[�̃��C�t
		g_Player[i].hp = PLAYER_HP;

		// ��_���[�W���̐F�ύX����
		g_Player[i].receivewait = 0;

		// �Q�[���J�n�ʒu����
		g_Player[i].chek = FALSE;

		// �A�j���[�V����
		g_Player[i].state  = STATE_STAY;
		g_Player[i].setTbl = STATE_STAY;

		// �e�̎c�e��
		g_Player[i].reload = PLAYER_RELOAD;

		// ���f���C���f�b�N�X
		g_Player[i].ModelIdx = NULL;


		// �����Ńv���C���[�p�̉e���쐬���Ă���
		XMFLOAT3 pos = g_Player[i].pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		g_Player[i].shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
		//          ��
		//        ���̃����o�[�ϐ������������e��Index�ԍ�

		// �L�[�����������̃v���C���[�̌���
		roty = 0.0f;

		g_Player[i].parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

		 //�K�w�A�j���[�V�����̏�����
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			g_Parts[j].use = FALSE;

			// �ʒu�E��]�E�X�P�[���̏����ݒ�
			g_Parts[j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// �e�q�֌W
			g_Parts[j].parent = &g_Player[i];		// �� �����ɐe�̃A�h���X������
		//	g_Parts[�r].parent= &g_Player;		// �r��������e�͖{�́i�v���C���[�j
		//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

			// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
			g_Parts[j].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
			g_Parts[j].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Parts[j].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

			// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
			g_PartModel[j].load = FALSE;
		}

		// �L�[�t���[���ύX����
		g_Parts[0].use = TRUE;
		g_Parts[0].parent = &g_Player[0];	// �e���Z�b�g
		g_Parts[0].tblNo = 0;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[0].tblMax = (int)g_TblAdr[g_Parts[0].tblNo]->size();	// �� �ύX����
		//LoadModel(MODEL_PLAYER_BODY, &g_PartModel[0].model);

		// �L�[�t���[���ύX����
		g_Parts[1].use = TRUE;
		g_Parts[1].parent = &g_Parts[0];	// �e���Z�b�g
		g_Parts[1].tblNo = 1;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[1].tblMax = (int)g_TblAdr[g_Parts[1].tblNo]->size();	// �� �ύX����
		//LoadModel(MODEL_PLAYER_HEAD, &g_PartModel[1].model);

		// �L�[�t���[���ύX����
		g_Parts[2].use = TRUE;
		g_Parts[2].parent = &g_Parts[0];	// �e���Z�b�g
		g_Parts[2].tblNo = 2;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[2].tblMax = (int)g_TblAdr[g_Parts[2].tblNo]->size();	// �� �ύX����
		//LoadModel(MODEL_PLAYER_ARM_L, &g_PartModel[2].model);

		// �L�[�t���[���ύX����
		g_Parts[3].use = TRUE;
		g_Parts[3].parent = &g_Parts[0];	// �e���Z�b�g
		g_Parts[3].tblNo = 3;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[3].tblMax = (int)g_TblAdr[g_Parts[3].tblNo]->size();	// �� �ύX����
		//LoadModel(MODEL_PLAYER_ARM_R, &g_PartModel[3].model);

		// �L�[�t���[���ύX����
		g_Parts[4].use = TRUE;
		g_Parts[4].parent = &g_Parts[0];	// �e���Z�b�g
		g_Parts[4].tblNo = 4;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[4].tblMax = (int)g_TblAdr[g_Parts[4].tblNo]->size();	// �� �ύX����
		//LoadModel(MODEL_PLAYER_THINGS_L, &g_PartModel[4].model);

		// �L�[�t���[���ύX����
		g_Parts[5].use = TRUE;
		g_Parts[5].parent = &g_Parts[0];	// �e���Z�b�g
		g_Parts[5].tblNo = 5;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[5].tblMax = (int)g_TblAdr[g_Parts[5].tblNo]->size();	// �� �ύX����
		//LoadModel(MODEL_PLAYER_THINGS_R, &g_PartModel[5].model);

		// �L�[�t���[���ύX����
		g_Parts[6].use = TRUE;
		g_Parts[6].parent = &g_Parts[4];	// �e���Z�b�g
		g_Parts[6].tblNo = 6;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[6].tblMax = (int)g_TblAdr[g_Parts[6].tblNo]->size();	// �� �ύX����
		//LoadModel(MODEL_PLAYER_LEG_L, &g_PartModel[6].model);

		// �L�[�t���[���ύX����
		g_Parts[7].use = TRUE;
		g_Parts[7].parent = &g_Parts[5];	// �e���Z�b�g
		g_Parts[7].tblNo = 7;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[7].tblMax = (int)g_TblAdr[g_Parts[7].tblNo]->size();	// �� �ύX����
		//LoadModel(MODEL_PLAYER_LEG_R, &g_PartModel[7].model);

		// ���f���̓ǂݍ���
		for (int m = 0; m < PLAYER_PARTS_MAX; m++)
		{
			LoadModel(g_ModelName[m], &g_PartModel[m].model);
			g_PartModel[m].load = TRUE;
		}

		// �N�H�[�^�j�I���̏�����
		XMStoreFloat4(&g_Player[i].Quaternion, XMQuaternionIdentity());
	}
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
		// �p�[�c�̉������
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_PartModel[i].load == TRUE)
			{
				// �p�[�c�̉������
				UnloadModel(&g_PartModel[i].model);
				g_PartModel[i].load = FALSE;
			}
		}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();
	ENEMY* enemy = GetEnemy();


	switch (g_Player[0].state)
	{
	case STATE_STAY:
		InputWalkPlayer();
		InputAttackPlayer();
		SetTblWalk();
		SetTblStay();
		break;

	case STATE_WALK:
		InputWalkPlayer();
		WalkPlayer();
		SetTblWalk();
		break;

	case STATE_ATK:
		AttackPlayer();
		SetTblAttack();
		break;

	default:
		break;
	}


	for (int i = 0; i < MAX_PLAYER; i++)
	{
		// ���C�L���X�g���đ����̍��������߂�E�d�͔���
		XMFLOAT3 HitPosition;		// ��_ �n�ʂ̓��������Ƃ���
		XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
		BOOL ans = RayHitField(g_Player[i].pos, &HitPosition, &Normal);	// �������Ă�����
		BOOL gravity = TRUE;		// �d�͂����邩�ǂ���
		float distance = 0.0f;		// �n�ʂƃv���C���[�̍�

		if (ans)
		{
			distance = g_Player[i].pos.y - HitPosition.y;	// �v���C���[�ƒn�ʂ̍����̍����o��

			// �n�ʂɖ��܂�Ȃ�����
			if (distance <= GRAVITY_PLUS && distance >= GRAVITY_MINUS)
			{
				// �d�͂��Ȃ���
				gravity = FALSE;
			}
		}

		if (gravity == TRUE)
		{
			// �d�͂�t����
			g_Player[i].pos.y -= GRAVITY;

		}

		// �����n�ʂ�艺�Ȃ猳�̈ʒu�ɖ߂�
		if (distance < 0.0f && ans == TRUE)
		{
			g_Player[i].pos.y -= distance - PLAYER_OFFSET_Y;
		}

		// �W�����v����
		//{
		//	if (g_Player[i].jump == TRUE)
		//	{
		//		// �J�E���g���}�b�N�X�̎��ɁuXM_PI / 2�v�ɂȂ�悤�ɂ���
		//		float angle = (XM_PI / (PLAYER_JUMP_CNT_MAX * 2)) * g_Player[i].jumpCnt;
		//		// �W�����v�}�b�N�X��0~1�������Ă���
		//		// cosf�̒l���P�ɂȂ�悤�ɂ���
		//		float y = g_Player[i].jumpYMax * cosf(XM_PI / 2 + angle);
		//		g_Player[i].jumpY = y;

		//		// ���̈ʒu����W�����vY�������Ă���(���ǂ̗]�n����)
		//		g_Player[i].pos.y = g_Player[i].jump_pos - g_Player[i].jumpY;

		//		g_Player[i].jumpCnt++;
		//		if (g_Player[i].jumpCnt > PLAYER_JUMP_CNT_MAX)
		//		{
		//			g_Player[i].jump = FALSE;
		//			g_Player[i].jumpCnt = 0;
		//			g_Player[i].jumpY = 0.0f;
		//		}
		//	}
		//	else if ((g_Player[i].jump == FALSE) && (MoveJumpKeyboard()))
		//	{// �W�����v�{�^���������H
		//		g_Player[i].jump = TRUE;
		//		g_Player[i].jumpCnt = 0;
		//		g_Player[i].jumpY = 0.0f;
		//		g_Player[i].jump_pos = g_Player[i].pos.y;
		//	}
		//	else // �W�����v���͏d�͂������Ȃ�
		//	{
		//		if (gravity == TRUE)
		//		{
		//			// �d�͂�t����
		//			g_Player[i].pos.y -= GRAVITY;

		//		}
		//	}
		//}

		// �Q�[���J�n
		if (START_POS_X_2 <= g_Player[i].pos.x &&
			g_Player[i].pos.x <= START_POS_X_1 &&
			START_POS_Z_2 <= g_Player[i].pos.z &&
			g_Player[i].pos.z <= START_POS_Z_1)
		{
			// �Q�[�����X�^�[�g������
			g_Player[i].chek = TRUE;
		}

		// �A�j���[�V����
		HierarchyAnimation(PLAYER_PARTS_MAX);

		// �e���v���C���[�̈ʒu�ɍ��킹��
		XMFLOAT3 pos = g_Player[i].pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		SetPositionShadow(g_Player[i].shadowIdx, pos);

		// �|�C���g���C�g�̃e�X�g
		{
			LIGHT* light = GetLightData(1);
			XMFLOAT3 pos = g_Player[i].pos;
			//pos.y += 20.0f;

			light->Position = pos;
			light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			light->Type = LIGHT_TYPE_POINT;
			light->Enable = TRUE;
			SetLightData(1, light);
		}

		//////////////////////////////////////////////////////////////////////
		// �p������
		//////////////////////////////////////////////////////////////////////

		XMVECTOR vx, nvx, up;
		XMVECTOR quat;
		float len, angle;


		g_Player[i].UpVector = Normal;
		up = { 0.0f, 1.0f, 0.0f, 0.0f };
		vx = XMVector3Cross(up, XMLoadFloat3(&g_Player[i].UpVector));

		//�����̃x�N�g�������߂Ă���
		nvx = XMVector3Length(vx);
		XMStoreFloat(&len, nvx);
		nvx = XMVector3Normalize(vx);
		//nvx = vx / len;
		angle = asinf(len);

		//quat = XMQuaternionIdentity();

		//	quat = XMQuaternionRotationAxis(nvx, angle);
		quat = XMQuaternionRotationNormal(nvx, angle);


		quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player[i].Quaternion), quat, 0.05f);
		XMStoreFloat4(&g_Player[i].Quaternion, quat);



#ifdef _DEBUG

		// �e�X�g
		if (GetKeyboardPress(DIK_3))
		{
			g_Player[i].pos.y += 0.1f;
		}

		if (GetKeyboardPress(DIK_4))
		{
			g_Player[i].pos.y -= 0.1f;
		}

		if (GetKeyboardPress(DIK_5))
		{
			SetFadeMode(FADE_OUT, MODE_CLEAR_RESULT);
		}

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

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].use == FALSE) return;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Player[i].scl.x * PLAYER_PARTS_SIZE, g_Player[i].scl.y * PLAYER_PARTS_SIZE, g_Player[i].scl.z * PLAYER_PARTS_SIZE);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �N�H�[�^�j�I���𔽉f
		quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player[i].Quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);
		

		// �ύX���Ԃ𑫂�
		g_Player[i].receivewait++;
		if (g_Player[i].ans == TRUE)
		{
			// �����̐ݒ�
			SetFuchi(1);

			// �����Areceivewait����莞�Ԃ������猳�ɖ߂�
			if (g_Player[i].receivewait >= PLAYER_RECEIVEWAIT)
			{
				SetFuchi(0);
				g_Player[i].ans = FALSE;
				g_Player[i].receivewait = 0;
			}
		}

		// �J�����O�ݒ��߂�
		SetCullingMode(CULL_MODE_BACK);

		// �K�w�A�j���[�V����
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Parts[j].scl.x, g_Parts[j].scl.y, g_Parts[j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[j].rot.x, g_Parts[j].rot.y, g_Parts[j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Parts[j].pos.x, g_Parts[j].pos.y, g_Parts[j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[j].parent != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[j].parent->mtxWorld));
				// ��
				// g_Player[i].mtxWorld���w���Ă���
			}

			XMStoreFloat4x4(&g_Parts[j].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������
			if (g_Parts[j].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);
				
			DrawModel(&g_PartModel[j].model);
		}

		SetFuchi(0);

	}
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
		return &g_Player[0];

}

BOOL AnyKeyPress(void)
{
	// �ǂꂩ�������ꂽ��
	return (MoveLeftKeyboard() || MoveRightKeyboard() || MoveUpKeyboard() || MoveDownKeyboard());

}

// ���ړ�
BOOL MoveLeftKeyboard(void)
{
	return GetKeyboardPress(DIK_LEFT);
}

// �E�ړ�
BOOL MoveRightKeyboard(void)
{
	return GetKeyboardPress(DIK_RIGHT);

}

// ��ړ�
BOOL MoveUpKeyboard(void)
{
	return GetKeyboardPress(DIK_UP);
}

// ���ړ�
BOOL MoveDownKeyboard(void)
{
	return GetKeyboardPress(DIK_DOWN);
}

// �W�����v
BOOL MoveJumpKeyboard(void)
{
	return GetKeyboardTrigger(DIK_SPACE);
}


//=============================================================================
// ���X�e�B�b�N�̓��͏���
//=============================================================================

// �ǂꂩ�������ꂽ��
BOOL AnyStickpress(void)
{
	return (MoveLeftStick() || MoveRightStick() || MoveUpStick() || MoveDownStick());

}

// ���ړ�
BOOL MoveLeftStick(void)
{
	return IsButtonPressed(0, L_STICK_LEFT);
}

// �E�ړ�
BOOL MoveRightStick(void)
{
	return IsButtonPressed(0, L_STICK_RIGHT);
}

// ��ړ�
BOOL MoveUpStick(void)
{
	return IsButtonPressed(0, L_STICK_UP);
}

// ���ړ�
BOOL MoveDownStick(void)
{
	return IsButtonPressed(0, L_STICK_DOWN);
}

//=============================================================================
// �E�X�e�B�b�N�̓��͏���
//=============================================================================

// �ǂꂩ�������ꂽ��
BOOL CameraAnyStickpress(void)
{
	return (CameraLeftStick() || CameraRightStick() || CameraUpStick() || CameraDownStick());

}

// ���ړ�
BOOL CameraLeftStick(void)
{
	return IsButtonPressed(0, R_STICK_LEFT);
}

// �E�ړ�
BOOL CameraRightStick(void)
{
	return IsButtonPressed(0, R_STICK_RIGHT);
}

// ��ړ�
BOOL CameraUpStick(void)
{
	return IsButtonPressed(0, R_STICK_UP);
}

// ���ړ�
BOOL CameraDownStick(void)
{
	return IsButtonPressed(0, R_STICK_DOWN);
}


// �q�̓����蔻��(�p�[�c�̔ԍ��A�p�[�c���a�A�G�l�~�[�̍��W�A�G�l�~�[�̔��a)
BOOL ChildCollision(int index, float r, XMFLOAT3 epos, float er)
{
	// �p�[�c�̎q�̕t�����̍��W�����߂Ă���
	XMMATRIX mtx = XMLoadFloat4x4(&g_Parts[index].mtxWorld);

	// ���߂����W�����Ă�
	XMFLOAT3 pos = XMFLOAT3(mtx.r[3].m128_f32[0], mtx.r[3].m128_f32[1], mtx.r[3].m128_f32[2]);

	//BC�̓����蔻��̌��ʂ�Ԃ�
	return CollisionBC(pos, epos, r, er);
}

 //�K�w�A�j���[�V����
void HierarchyAnimation(int partmax)
{
	for (int j = 0; j < partmax; j++)
	{
		// �g���Ă���Ȃ珈������
		if ((g_Parts[j].use == TRUE) && (g_Parts[j].tblMax > 0))
		{	// ���`��Ԃ̏���
			int nowNo = (int)g_Parts[j].time;			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = g_Parts[j].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
			//INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Parts[j].tblNo];	// �s���e�[�u���̃A�h���X���擾

			//--------------------------------------------------------------------------------------
			// �L�[�t���[���ύX����
			std::vector<INTERPOLATION_DATA> tbl = *g_TblAdr[g_Parts[j].tblNo];
			//--------------------------------------------------------------------------------------

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

			float nowTime = g_Parts[j].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

			Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
			Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
			Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

			// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
			XMStoreFloat3(&g_Parts[j].pos, nowPos + Pos);

			// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[j].rot, nowRot + Rot);

			// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[j].scl, nowScl + Scl);

			// frame���g�Ď��Ԍo�ߏ���������
			g_Parts[j].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
			if ((int)g_Parts[j].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				g_Parts[j].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���

				//--------------------------------------------------------------------------------------
				// �L�[�t���[���ύX����
				switch (g_Player[0].setTbl)
				{
				case STATE_ATK:
					g_Parts[j].tblMax = -1;
					break;
				}
				//--------------------------------------------------------------------------------------

			}
		}
	}
}

// �L�[����
void InputWalkPlayer(void)
{
	// ���W�̋L��
	g_Player[0].oldpos = g_Player[0].pos;

	// -----�L�[�{�[�h---------------
	if (AnyKeyPress())
	{
		g_Player[0].spd = VALUE_MOVE;
		g_Player[0].state = STATE_WALK;

		// �΂ߕ����Ɉړ�����
		float x = 0.0f;
		float z = 0.0f;

		if (MoveLeftKeyboard())  x = -1.0f;
		if (MoveRightKeyboard()) x = 1.0f;
		if (MoveUpKeyboard())	 z = -1.0f;
		if (MoveDownKeyboard())  z = 1.0f;

		float angle = atan2f(x, z) * -1;
		g_Player[0].dir = angle;

	}

	// -----�X�e�B�b�N---------------
	if (AnyStickpress())
	{
		g_Player[0].spd = VALUE_MOVE;
		g_Player[0].state = STATE_WALK;

		//// �L�[���͂��擾���Ă���
		DIJOYSTATE2		dijs = GetDeviceState();

		// �v���C���[��360�x�񂵂Ă���
		float angle = atan2f((float)dijs.lX, (float)dijs.lY) * -1;

		g_Player[0].dir = angle;

	}

}

// �ړ�����
void WalkPlayer(void)
{
	// Key���͂���������ړ���������
	CAMERA* cam = GetCamera();

	switch (g_Player[0].state)
	{
	case STATE_WALK:
		if (g_Player[0].spd < VALUE_MOVE) g_Player[0].state = STATE_STAY;
		break;

	default:
		break;
	}

	if (g_Player[0].spd > 0.0f)
	{
		g_Player[0].rot.y = g_Player[0].dir + cam->rot.y;

		// ���͂̂����������փv���C���[���������Ĉړ�������
		g_Player[0].pos.x -= sinf(g_Player[0].rot.y) * g_Player[0].spd;
		g_Player[0].pos.z -= cosf(g_Player[0].rot.y) * g_Player[0].spd;

		// ����������Ă���
		g_Player[0].spd *= 0.9f;

	}
}

// �U������
void InputAttackPlayer(void)
{
	CAMERA* cam = GetCamera();

	// �e���ˏ���
	if ( (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_R) ) && g_Player->reload != 0)
	{
		g_Player[0].rot.y = cam->rot.y + XM_PI;

		// �c�e�������炷
		g_Player[0].reload--;

		// ���[���h�}�g���b�N�X�̏�����
		// �����s��̏�����
		XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
		mtxWorld = XMMatrixIdentity();

		// �A�t�B���ϊ�
		// �ړ��𔽉f
		float rot = g_Player[0].rot.z;
		mtxTranslate = XMMatrixTranslation(0, 0, 500); // �x�N�g���̋���(�v���C���[��Z����)

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

		vector.x += cam->pos.x;
		vector.y += cam->pos.y;
		vector.z += cam->pos.z;

		XMFLOAT3 move = {0.0f,0.0f,0.0f};
		XMFLOAT3 vPos = g_Player[0].pos;
		vPos.y += PLAYER_BULLET;

		// �v���C���[�ƃJ�����̍���
		float ofset_y = vPos.y - vector.y;

		// �v���C���[�̓���ɃJ�����̍����𔽓]������
		vector.y += ofset_y * 3;

		// �J����
		XMVECTOR vA = XMLoadFloat3(&vector);
		// �v���C���[�̍��W
		XMVECTOR vB = XMLoadFloat3(&vPos);

		// vAB�̐��K��
		// ���������߂āA���K�����Ă���(A,B)
		XMVECTOR vAB = XMVector3Normalize(vA - vB);

		XMStoreFloat3(&move, vAB);
		SetBullet(vPos, move);

		g_Player[0].state = STATE_ATK;

		// �J�����V�F�C�N
		// �ړ�����b�����J�E���g����
		//cam->cnt++;
		//if (cam->cnt == 50)
		//{
		//	cam->cnt = 0;
		//	g_Enemy[i].spd *= -1.0f;
		//	g_Enemy[i].rot.y += XM_PI;
		//}
		//// �ړ�����
		//g_Enemy[i].pos.z -= g_Enemy[i].spd;


		// �J�����V�F�C�N
		// �ړ�����b�����J�E���g����
		//cam->pos.y -= cam->spd;
		//cam->cnt++;
		//if (cam->cnt == 5)
		//{
		//	cam->cnt = 0;
		//	cam->spd *= -1.0f;
		//}
		//// �ړ�����
		//cam->pos.y -= cam->spd;

		// ���ʉ�
		PlaySound(SOUND_LABEL_SE_player_atc);
	}

	// �����[�h
	if ( (GetKeyboardTrigger(DIK_TAB) || IsButtonTriggered(0, BUTTON_L) ) && g_Player[0].reload <= 5)
	{
		g_Player[0].reload = PLAYER_RELOAD;
	}

}

void AttackPlayer(void)
{
	// ��ԑJ��
	{
		BOOL ans = TRUE;

		// ���ׂẴp�[�c�̍ő�e�[�u������ -1 �ɂȂ��Ă���ꍇ(���A�j���[�V�������I�����Ă���ꍇ)�A��ԑJ�ڂ�����
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_Parts[i].tblMax != -1) ans = FALSE;
		}

		if (ans == TRUE) g_Player[0].state = STATE_STAY;
	}
}


void SetTblStay(void)
{
	switch (g_Player[0].setTbl)
	{
	case STATE_STAY:
		break;

	default:
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_Parts[i].use == FALSE) continue;

			g_Parts[i].time = 0.0f;
			g_Parts[i].tblNo = i + STAY_BODY;
			g_Parts[i].tblMax = (int)g_TblAdr[g_Parts[i].tblNo]->size();

		}
		g_Player[0].setTbl = STATE_STAY;
		break;
	}
}

void SetTblWalk(void)
{
	switch (g_Player[0].setTbl)
	{
	case STATE_WALK:
		break;

	default:
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_Parts[i].use == FALSE) continue;

			g_Parts[i].time = 0.0f;
			g_Parts[i].tblNo = i + WALK_BODY;
			g_Parts[i].tblMax = (int)g_TblAdr[g_Parts[i].tblNo]->size();

		}
		g_Player[0].setTbl = STATE_WALK;
		break;
	}
}

void SetTblAttack(void)
{
	switch (g_Player[0].setTbl)
	{
	case STATE_ATK:
		break;

	default:
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_Parts[i].use == FALSE) continue;

			g_Parts[i].time = 0.0f;
			g_Parts[i].tblNo = i + ATK_BODY;
			g_Parts[i].tblMax = (int)g_TblAdr[g_Parts[i].tblNo]->size();

		}
		g_Player[0].setTbl = STATE_ATK;
		break;
	}
}