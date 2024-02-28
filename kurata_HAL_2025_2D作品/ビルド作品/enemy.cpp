//=============================================================================
//
// �G�l�~�[���� [enemy.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "enemy.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "map.h"
#include "bullet.h"
#include "bosbullet.h"
#include "fade.h"
#include "enemyeffect.h"
#include "enemybullet.h"
#include "input.h"
#include "bossatc.h"
#include "sound.h"
#include "strikebullet.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************

// �{�X�̃}�b�N�X
#define BOSS_ENEMY_MAX				(1)

// �e�N�X�`���̐�
#define ENEMY_TEXTURE_MAX			(6)		

// �G�l�~�[ �L�����T�C�Y
#define ENEMY_TEXTURE_WIDTH			(600)	
#define ENEMY_TEXTURE_HEIGHT		(800)	

#define WEPON_TEXTURE_WIDTH			(200)	
#define WEPON_TEXTURE_HEIGHT		(400)	

// �{�X�p �L�����T�C�Y
#define BOSS_TEXTURE_WIDTH			(400)	
#define BOSS_TEXTURE_HEIGHT			(700)	

// ��_���[�W�ɐF��ς��Ă��鎞��
#define TEXTURE_COLOR_CHANGE_TIME	(200)

// �G�l�~�[�̗̑͊֌W
// Max�l
#define ENEMY_MAX_HP				(1000)

// 1��ڂ̕K�E�Z
#define ENEMY_HP_1					(500)
#define ENEMY_HP_2					(400)

// 2���
#define ENEMY_HP_3					(200)
#define ENEMY_HP_4					(100)


// �{�X�̗̑͊֌W
// ���[�h�P(MAX�l)
#define BOSS_HP_X1					(5000)

// ���[�h�Q
#define BOSS_HP_X2					(4000)

// ���[�h3
#define BOSS_HP_X3					(3000)

// ���[�h4
#define BOSS_HP_X4					(2000)

// ���[�h5
#define BOSS_HP_X5					(1000)

//// �����Q�[���I�[�o�[
//#define DEATHBLOW_TIME				(1800)

// �A�j���[�V����
// �G�l�~�[
#define ENEMY_TEXTURE_PATTERN_DIVIDE_X				(4)																	// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define ENEMY_TEXTURE_PATTERN_DIVIDE_Y				(1)																	// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM							(ENEMY_TEXTURE_PATTERN_DIVIDE_X * ENEMY_TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT									(5)																	// �A�j���[�V�����̐؂�ւ��Wait�l


// �E�F�|��
#define WEPON_TEXTURE_PATTERN_DIVIDE_X				(1)																	// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define WEPON_TEXTURE_PATTERN_DIVIDE_Y				(1)																	// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define WEPON_ANIM_PATTERN_NUM						(WEPON_TEXTURE_PATTERN_DIVIDE_X * WEPON_TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define WEPON_ANIM_WAIT								(5)																	// �A�j���[�V�����̐؂�ւ��Wait�l

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

// �G�l�~�[�̕\��
enum 
{
	enemy,
	wepon01,
	wepon02,
};
// �G�l�~�[�̌���
enum
{
	ENEMY_DIR_LEFT,		// ��
	ENEMY_DIR_RIGHT,	// �E

};

// �G�l�~�[�̃o���b�g�̃e�N�X�`���[
enum
{
	enemy_bullet00,
	enemy_bullet01,
};

// �{�X�̃A�j���[�V����
enum 
{
	boss_state_walk,
	boss_state_atc,
	boss_state_fly,
};

// �ǂ����Ŏg�������ʂ���
enum 
{
	enemy_check,
	boss_check,
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[ENEMY_TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

// 5
static char *g_TexturName[ENEMY_TEXTURE_MAX] = {
	"data/TEXTURE/Boss00.png",
	"data/TEXTURE/Wepon00.png",
	"data/TEXTURE/Wepon01.png",
	"data/TEXTURE/Boss01.png",
	"data/TEXTURE/Boss02.png",
	"data/TEXTURE/Boss03.png",

};

// ���������s�������̃t���O
static BOOL		g_Load = FALSE;		

// �G�l�~�[�\����
static ENEMY	g_Enemy[ENEMY_MAX];		

// �{�X�\����
static BOSS		g_Boss[BOSS_MAX];		

// �{�X�̃A�j���[�V����
static BOSS_TEX		 g_texState[3]
{
	//  id,  x, y,MAX, w,      h,    wait  ���Ԓʂ�ɏ�������
	{   3,   4, 1, 4, 400.0f, 700.0f, 4},
	{   4,  11, 1,11, 400.0f, 700.0f, 4},
	{   5,   8, 1, 8, 400.0f, 700.0f, 4},

};

// �A�j���[�V�����p�^�[��
int g_state;

// �����_���ōU��
static int		g_atc;

// �{�X��Ɉڍs����
BOOL			g_round;

// �G�l�~�[�̐��𐔂���
static int		g_EnemyCnt = ENEMY_MAX;


static INTERPOLATION_DATA g_MoveTbl0[] = {
	//���W									��]��							�g�嗦				����
	{ XMFLOAT3(200.0f,   300.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	500 },
	{ XMFLOAT3(1600.0f,  300.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	500 },
};



// 1��
static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,	
};



//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device *pDevice = GetDevice();


	//�e�N�X�`������
	for (int i = 0; i < ENEMY_TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);
		
	// �����_���ōU��������
	g_atc = 0;
	
	// �{�X��Ɉڍs����
	g_round = FALSE;

	// �G�l�~�[�\���̂̏�����(�{�X�A�E�F�|��)
	g_EnemyCnt = 0;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].use					= TRUE;
		g_Enemy[i].ans					= FALSE;
		g_Enemy[i].pos					= XMFLOAT3(0.0f ,0.0f, 0.0f);				// ���S�_����\��
		g_Enemy[i].rot					= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl					= XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].w					= ENEMY_TEXTURE_WIDTH;						// �e�N�X�`���[�̕�
		g_Enemy[i].h					= ENEMY_TEXTURE_HEIGHT;						// �e�N�X�`���[�̍���
		g_Enemy[i].countAnim			= 0;
		g_Enemy[i].patternAnim			= 0;
		g_Enemy[i].dir					= ENEMY_DIR_LEFT;							// �G�l�~�[�̌���(0:X�}�C�i�X�A��		1:X�v���X�A�E)
		g_Enemy[i].receive				= FALSE;									// �U�����󂯂����ǂ����̔���
		g_Enemy[i].id					= 0;										// �Ǘ��ԍ�(ID)
		g_Enemy[i].move					= XMFLOAT3(4.0f, 0.0f, 0.0f);				// �ړ���

		g_Enemy[i].time					= 0.0f;										// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tblNo				= 0;										// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Enemy[i].tblMax				= 0;										// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
		g_Enemy[i].receivewait			= 0;										// ��_���[�W���ɐF��ς��鎞��

		g_EnemyCnt++;
	}

	g_Enemy[0].life = ENEMY_MAX_HP;							// �G�l�~�[�̃��C�t
	g_Enemy[1].life = 0;								// �G�l�~�[�̃��C�t
	g_Enemy[2].life = 0;								// �G�l�~�[�̃��C�t

	g_Enemy[0].pos = XMFLOAT3(1312.0f, 500.0f, 0.0f);	// �������W
	g_Enemy[1].pos = XMFLOAT3(1000.0f, 200.0f, 0.0f);   // �������W
	g_Enemy[2].pos = XMFLOAT3(1500.0f, 200.0f, 0.0f);  	// �������W

	// �{�X�̃A�j���[�V�����̏�����
	g_state = boss_state_walk;


	// �{�X�\���̂̏�����
	for (int i = 0; i < BOSS_MAX; i++)
	{
		
		g_Boss[i].boss_use					= FALSE;
		g_Boss[i].boss_ans					= FALSE;
		g_Boss[i].boss_atcposition			= FALSE;								// �U���|�W�V�����ɂ������ǂ����̔���
		g_Boss[i].boss_pos					= XMFLOAT3(200.0f, 300.0f, 0.0f);		// ���S�_����\��
		g_Boss[i].boss_rot					= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].bosss_cl					= XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Boss[i].boss_w					= BOSS_TEXTURE_WIDTH;					// �e�N�X�`���[�̕�
		g_Boss[i].boss_h					= BOSS_TEXTURE_HEIGHT;					// �e�N�X�`���[�̍���
		g_Boss[i].boss_life					= BOSS_HP_X1;								// �G�l�~�[�̃��C�t
		g_Boss[i].boss_countanim			= 0;
		g_Boss[i].boss_patternAnim			= 0;
		g_Boss[i].boss_dir					= ENEMY_DIR_RIGHT;						// �G�l�~�[�̌���
		g_Boss[i].boss_receive				= FALSE;								// �U�����󂯂����ǂ����̔���
		g_Boss[i].boss_id					= 0;									// �Ǘ��ԍ�(ID)
		g_Boss[i].boss_move					= XMFLOAT3(4.0f, 0.0f, 0.0f);			// �ړ���
		g_Boss[i].boss_wait					= 0;									// �{�X�E�F�C�g
		g_Boss[i].boss_time					= 0.0f;									// ���`��ԗp�̃^�C�}�[���N���A
		g_Boss[i].boss_tblNo				= 0;									// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Boss[i].boss_tblMax				= 0;									// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
		g_Boss[i].boss_receivewait			= 0;									// ��_���[�W���ɐF��ς��鎞��
		g_Boss[i].boss_check				= FALSE;								// �K�E�Z��ł���
		g_Boss[i].boss_deathblow			= 0.0f;									// �K�E�Z��ł�
	}
	

		
	//  0�Ԃ������`��Ԃœ������Ă݂�
	g_Boss[0].boss_time		 =0.0f;													// ���`��ԗp�̃^�C�}�[���N���A
	g_Boss[0].boss_tblNo	 =0;													// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Boss[0].boss_tblMax	 = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < ENEMY_TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	// �`���[�g���A��
	BG* bg = GetBG();

	// pose���j���[���\�����ꂽ��A�������~�߂�
	if (bg->pose == FALSE)
	{
		// �G�l�~�[��
		if (g_round == FALSE)
		{

			ENEMY_UPDATE();
		}

		// �{�X
		if (g_round == TRUE)
		{


			BOSS_UPDATE();

		}

	}
	
#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	PLAYER* player = GetPlayer();
	// �G�l�~�[�p
	if (g_round == FALSE)
	{
		ENEMY_DRAW();
	}

	// �{�X��
	if (g_round == TRUE)
	{
		BOSS_DRAW();
	}
}


//=============================================================================
// �G�l�~�[�֐��ƃ{�X�֐�
//=============================================================================
void ENEMY_UPDATE(void)
{


	g_EnemyCnt = 0;	// �����Ă�G�l�~�[�̐�

		// 1�X�e�[�W��(���̌�{�X��)
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		// �����Ă�G�l�~�[��������������
		if (g_Enemy[i].use == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Enemy[i].pos;

			// �A�j���[�V����  
			g_Enemy[i].countAnim += 1.0f;
			if (g_Enemy[i].countAnim > ANIM_WAIT)
			{
				g_Enemy[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}


			// �{�X�̌����̒���
			PLAYER* player = GetPlayer();

			// �{�X���v���C���[��X���W���E�ɂ�����,���Ɍ�������
			if (player[0].pos.x > g_Enemy[i].pos.x)
			{
				g_Enemy[i].dir = ENEMY_DIR_RIGHT;
			}

			// �����{�X���v���C���[��X���W��荶�ɂ�����A�E�Ɍ�������
			if (player[0].pos.x < g_Enemy[i].pos.x)
			{
				g_Enemy[i].dir = ENEMY_DIR_LEFT;
			}

				// �펞�U��������
				g_Enemy[i].wait++;
				if (g_Enemy[i].wait >= ENEMY_WAIT)
				{

						// �㉺�W�����ɍU��
						XMFLOAT3 enemypos = g_Enemy[1].pos;
						enemypos.y += enemypos.y / 2;

						SetStrikeBullet(enemypos, enemy_bullet00);

				}

				// ���̗̑͂ɂȂ�����U��������
				{
					// 1���
					if (ENEMY_HP_1 >= g_Enemy[0].life && g_Enemy[0].life >= ENEMY_HP_2)
					{
						//// �x�W�F�Ȑ�
						XMFLOAT3 bosspos = g_Enemy[2].pos;
						SetBulletBos(enemy_check, 0);
					}

					// 2���
					if (ENEMY_HP_3 >= g_Enemy[0].life && g_Enemy[0].life >= ENEMY_HP_4)
					{
						//// �x�W�F�Ȑ�
						XMFLOAT3 bosspos = g_Enemy[2].pos;
						SetBulletBos(enemy_check, 0);
					}
				}
		}



		PLAYER* player = GetPlayer();

		// �G�l�~�[���|�ꂽ��A���ɍs��
		if (g_Enemy[i].life <= 0)
		{
			g_EnemyCnt++;	// ���񂾐�
		}

		// �G�l�~�[��|������A�{�X���[�h�ɍs��
		if (g_EnemyCnt == ENEMY_MAX)
		{
			// �G�t�F�N�g����
			SetEnemyEffect(g_Enemy[0].pos.x, g_Enemy[0].pos.y, 30, TEXTURE_EFFECT);

			// �{�X��TRUE�ɂ��āA�{�X���o��������
			g_Boss[0].boss_use = TRUE;
			g_round = TRUE;

			// �Ȃ��~�߂�
			StopSound(SOUND_LABEL_BGM_boss01);
			// �Ȃ𗬂�
			PlaySound(SOUND_LABEL_BGM_enemy00);


		}
	}


}


void BOSS_UPDATE(void)
{

	for (int i = 0; i < BOSS_ENEMY_MAX; i++)
	{
		// �����Ă�G�l�~�[��������������
		if (g_Boss[i].boss_use == TRUE)
		{

			
			PLAYER* player = GetPlayer();


			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 boss_pos_old = g_Boss[i].boss_pos;

			// �A�j���[�V����  
			g_Boss[i].boss_countanim += 1.0f;
			if (g_Boss[i].boss_countanim > ANIM_WAIT)
			{

				g_Boss[i].boss_countanim = 0.0f;

				// �A�j���[�V�����p�^�[�����I���^�C�~���O
				if ((g_Boss[i].boss_patternAnim + 1) >= g_texState[g_state].animemax)
				{
					// �U�����[�V��������s�U����������ʏ�p�̃A�j���[�V�����ɂ���
					if (g_state == boss_state_atc || g_state == boss_state_fly)
					{
						g_state = boss_state_walk;
					}
				}


				// �p�^�[���̐؂�ւ�
				g_Boss[i].boss_patternAnim = (g_Boss[i].boss_patternAnim + 1) % g_texState[g_state].animemax;
			}



			// �{�X�̌����̒���

			// �{�X���v���C���[��X���W���E�ɂ�����,���Ɍ�������
			if (player[0].pos.x > g_Boss[i].boss_pos.x)
			{
				g_Boss[i].boss_dir = ENEMY_DIR_LEFT;
			}

			// �����{�X���v���C���[��X���W��荶�ɂ�����A�E�Ɍ�������
			if (player[0].pos.x < g_Boss[i].boss_pos.x)
			{
				g_Boss[i].boss_dir = ENEMY_DIR_RIGHT;
			}



			// �{�X�̍U�������i���C�t�̎c��ɂ���āA�U���̎�ނ𑝂₵�ċ������Ă���)
			g_Boss[0].boss_wait += 1;


			// �z�[�~���O�U���ƈړ��̂�
			if ( (BOSS_HP_X1 >= g_Boss[i].boss_life) && (g_Boss[i].boss_life > BOSS_HP_X2) )
			{

				XMFLOAT3 bosspos = g_Boss[0].boss_pos;
				bosspos.y -= g_Boss[0].boss_pos.y / 2;

				// �펞�U��������(8�����U��)
				SetStrikeBullet(bosspos, enemy_bullet01);

			}

			// �K�E�Z���o��(�x�W�F�Ȑ�)
			if ( (BOSS_HP_X2 >= g_Boss[i].boss_life) && (g_Boss[i].boss_life > BOSS_HP_X3) )
			{
				// �ړ������Ȃ�
				g_Boss[i].boss_atcposition = TRUE;

				// �A�j���V������ς���
				g_state = boss_state_atc;

				PlaySound(SOUND_LABEL_bossatc);

				XMFLOAT3 bosspos = g_Boss[0].boss_pos;
				bosspos.y -= g_Boss[0].boss_pos.y / 2;

				// �펞�U��������(8�����U��)
				SetStrikeBullet(bosspos, enemy_bullet01);

				// �K�E�Z(�x�W�F�Ȑ�)
				SetBulletBos(boss_check, 0);

			}

			// �������U���̒���

			// �z�[�~���O�U���Əォ��̍U���̂�
			if ( (BOSS_HP_X3 > g_Boss[i].boss_life) && (g_Boss[i].boss_life > BOSS_HP_X4) )
			{
				// �ړ������Ȃ�
				g_Boss[i].boss_atcposition = TRUE;

				XMFLOAT3 bosspos = g_Boss[0].boss_pos;
				bosspos.y -= g_Boss[0].boss_pos.y / 2;

				// �펞�U��������(8�����U��)
				SetStrikeBullet(bosspos, enemy_bullet01);


				// �{�X�E�F�C�g�̎��Ԃ�葽���Ȃ�����U��������(�ォ��̍U��)
				if (g_Boss[0].boss_wait >= BOSS_WAIT)
				{
					g_state = boss_state_fly;
					SetBulletAtc(enemy_bullet01);
					g_Boss[0].boss_wait = 0;

				}

			}

			// �z�[�~���O�U���Əォ��̍U���ƈړ��U��
			if ( (BOSS_HP_X4 > g_Boss[i].boss_life) && (g_Boss[i].boss_life > 0) )
			{

				// �A�j���V������ς���
				g_state = boss_state_atc;

				// �ړ�������
				g_Boss[i].boss_atcposition = FALSE;

				// �펞�U��������(8�����U��)
				XMFLOAT3 bosspos = g_Boss[0].boss_pos;
				bosspos.y -= g_Boss[0].boss_pos.y / 2;

				SetStrikeBullet(bosspos, enemy_bullet01);


				// �ォ��̍U��
				g_state = boss_state_fly;
				SetBulletAtc(enemy_bullet01);
				g_Boss[0].boss_wait = 0;

			}


			if (g_Boss[i].boss_atcposition == FALSE)
			{
				// ���`�⊮ �ړ�����
				if (g_Boss[i].boss_tblMax > 0)								// ���`��Ԃ����s����H
				{	// ���`��Ԃ̏���
					int nowNo = (int)g_Boss[i].boss_time;					// �������ł���e�[�u���ԍ������o���Ă���
					int maxNo = g_Boss[i].boss_tblMax;						// �o�^�e�[�u�����𐔂��Ă���
					int nextNo = (nowNo + 1) % maxNo;						// �ړ���e�[�u���̔ԍ������߂Ă���
					INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Boss[i].boss_tblNo];	// �s���e�[�u���̃A�h���X���擾

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);		// XMVECTOR�֕ϊ�
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);		// XMVECTOR�֕ϊ�
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);		// XMVECTOR�֕ϊ�

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

					float nowTime = g_Boss[i].boss_time - nowNo;			// ���ԕ����ł��鏭�������o���Ă���

					Pos *= nowTime;											// ���݂̈ړ��ʂ��v�Z���Ă���
					Rot *= nowTime;											// ���݂̉�]�ʂ��v�Z���Ă���
					Scl *= nowTime;											// ���݂̊g�嗦���v�Z���Ă���

					// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
					XMStoreFloat3(&g_Boss[i].boss_pos, nowPos + Pos);

					// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_Boss[i].boss_rot, nowRot + Rot);

					// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_Boss[i].bosss_cl, nowScl + Scl);
					g_Boss[i].boss_w = ENEMY_TEXTURE_WIDTH * g_Boss[i].bosss_cl.x;
					g_Boss[i].boss_h = ENEMY_TEXTURE_HEIGHT * g_Boss[i].bosss_cl.y;

					// frame���g�Ď��Ԍo�ߏ���������
					g_Boss[i].boss_time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
					if ((int)g_Boss[i].boss_time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
					{
						g_Boss[i].boss_time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
					}

				}

			}
		}
	}

	// �{�X��|������N���A��ʂɍs��
	if (g_Boss[0].boss_life <= 0)
	{
		// �G�t�F�N�g����
		SetEnemyEffect(g_Boss[0].boss_pos.x, g_Boss[0].boss_pos.y, 30, TEXTURE_EFFECT);

		SetFadeMode(FADE_OUT, MODE_CLEAR_RESULT);
	}

}

void ENEMY_DRAW(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)
		{
			// �G�l�~�[
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Enemy[enemy].pos.x - scrollX;	// �G�l�~�[�̕\���ʒuX
			float py = g_Enemy[enemy].pos.y - scrollY;	// �G�l�~�[�̕\���ʒuY
			float pw = g_Enemy[enemy].w;				// �G�l�~�[�̕\����
			float ph = g_Enemy[enemy].h;				// �G�l�~�[�̕\������

			 //�A�j���[�V�����p
			float tw = 1.0f / ENEMY_TEXTURE_PATTERN_DIVIDE_X;									// �e�N�X�`���̕�
			float th = 1.0f / ENEMY_TEXTURE_PATTERN_DIVIDE_Y;									// �e�N�X�`���̍���
			float tx = (float)(g_Enemy[enemy].patternAnim % ENEMY_TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Enemy[enemy].patternAnim / ENEMY_TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �����v���C���[�̍��ɂ����甽�]������
			if (g_Enemy[enemy].dir == ENEMY_DIR_LEFT)
			{
				// -1��������Ɣ��]
				tw *= -1;

				// �\���ʒu������Ȃ��悤�ɂ��Ă���
				tx += 1.0f / g_texState[g_state].animex;

			}

			// �U�����󂯂���F��ς���
			
			// �ʏ펞
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[0].rot.z);


			g_Enemy[enemy].receivewait++;

			// �����A�U�����󂯂���ԐF�ɂ���
			if (g_Enemy[enemy].receive == TRUE)
			{
				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
					g_Enemy[0].rot.z);
				
				// �����J���[�`�F���W�^�C���𒴂�����F��߂�
				if (g_Enemy[enemy].receivewait >= TEXTURE_COLOR_CHANGE_TIME)
				{
					// ���̐F�ɖ߂�
					g_Enemy[enemy].receive = FALSE;
					g_Enemy[enemy].receivewait += 0;

				}
			}

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);



			// �E�F�|��01
			
			 scrollX = GetScrollX();
			 scrollY = GetScrollY();

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[wepon01]);

			g_Enemy[wepon01].w = WEPON_TEXTURE_WIDTH;
			g_Enemy[wepon01].h = WEPON_TEXTURE_HEIGHT;

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			 px = g_Enemy[wepon01].pos.x - scrollX;	// �G�l�~�[�̕\���ʒuX
			 py = g_Enemy[wepon01].pos.y - scrollY;	// �G�l�~�[�̕\���ʒuY
			 pw = g_Enemy[wepon01].w;				// �G�l�~�[�̕\����
			 ph = g_Enemy[wepon01].h;				// �G�l�~�[�̕\������

			 //�A�j���[�V�����p
			 tw = 1.0f / WEPON_TEXTURE_PATTERN_DIVIDE_X;										// �e�N�X�`���̕�
			 th = 1.0f / WEPON_TEXTURE_PATTERN_DIVIDE_Y;										// �e�N�X�`���̍���
			 tx = (float)(g_Enemy[wepon01].patternAnim % WEPON_TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			 ty = (float)(g_Enemy[wepon01].patternAnim / WEPON_TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[1].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);



			// �E�F�|��02
						
			// �G�l�~�[
			 scrollX = GetScrollX();
			 scrollY = GetScrollY();

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[wepon02]);

			g_Enemy[wepon02].w = WEPON_TEXTURE_WIDTH;
			g_Enemy[wepon02].h = WEPON_TEXTURE_HEIGHT;

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			 px = g_Enemy[wepon02].pos.x - scrollX;	// �G�l�~�[�̕\���ʒuX
			 py = g_Enemy[wepon02].pos.y - scrollY;	// �G�l�~�[�̕\���ʒuY
			 pw = g_Enemy[wepon02].w;				// �G�l�~�[�̕\����
			 ph = g_Enemy[wepon02].h;				// �G�l�~�[�̕\������

			 //�A�j���[�V�����p
			 tw = 1.0f / WEPON_TEXTURE_PATTERN_DIVIDE_X;									// �e�N�X�`���̕�
			 th = 1.0f / WEPON_TEXTURE_PATTERN_DIVIDE_Y;									// �e�N�X�`���̍���
			 tx = (float)(g_Enemy[wepon02].patternAnim % WEPON_TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			 ty = (float)(g_Enemy[wepon02].patternAnim / WEPON_TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[wepon02].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);


		}
	}

}

void BOSS_DRAW(void)
{
	if (g_Boss[0].boss_use == TRUE)
	{

		float scrollX = GetScrollX();
		float scrollY = GetScrollY();

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_texState[g_state].id]);

		//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Boss[0].boss_pos.x - scrollX;		// �G�l�~�[�̕\���ʒuX
		float py = g_Boss[0].boss_pos.y - scrollY;		// �G�l�~�[�̕\���ʒuY
		float pw = g_Boss[0].boss_w;					// �G�l�~�[�̕\����
		float ph = g_Boss[0].boss_h;					// �G�l�~�[�̕\������

		 //�A�j���[�V�����p
		float tw = 1.0f / g_texState[g_state].animex;									// �e�N�X�`���̕�
		float th = 1.0f / g_texState[g_state].animey;										// �e�N�X�`���̍���
		float tx = (float)(g_Boss[0].boss_patternAnim % g_texState[g_state].animex) * tw;	// �e�N�X�`���̍���X���W
		float ty = (float)(g_Boss[0].boss_patternAnim / g_texState[g_state].animex) * th;	// �e�N�X�`���̍���Y���W

		// �����v���C���[�̍��ɂ����甽�]������
		if (g_Boss[0].boss_dir == ENEMY_DIR_LEFT)
		{
			// -1��������Ɣ��]
			tw *= -1;

			// �\���ʒu������Ȃ��悤�ɂ��Ă���
			tx += 1.0f / g_texState[g_state].animex;

		}

		// �U�����󂯂���F��ς���
		 
		// �ʏ펞
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			g_Boss[0].boss_rot.z);

		// �U�����󂯂���ԐF�ɂ���
		
		g_Boss[0].boss_receivewait++;

		// �����U�����󂯂���F��Ԃɕς���
		if (g_Boss[0].boss_receive == TRUE)
		{
			// ��
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
				g_Boss[0].boss_rot.z);

			// �����J���[�`�F���W�^�C���ȏゾ�����猳�̐F�ɖ߂�
			if (g_Boss[0].boss_receivewait >= TEXTURE_COLOR_CHANGE_TIME)
			{
				// ���̐F�ɖ߂�
				g_Boss[0].boss_receive = FALSE;
				g_Boss[0].boss_receivewait += 0;
			}
		}

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


	}
}






//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}


BOSS* GetBoss(void)
{
	return &g_Boss[0];
}

