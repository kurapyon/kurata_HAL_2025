//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "player.h"
#include "input.h"
#include "bg.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "Level.h"
#include "file.h"
#include "map.h"
#include "fade.h"
#include "cut.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(150/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(150/2)	// �L�����T�C�Y
#define TEXTURE_MAX					(10)	// �e�N�X�`���̐�
#define PLAYER_TEXTURE_MAX			(4)		// �v���C���[�̃e�N�X�`���[�̐�

#define TEXTURE_PATTERN_DIVIDE_X	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X * TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l

// �v���C���[�̉�ʓ��z�u���W
#define PLAYER_DISP_X				(SCREEN_WIDTH / 2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT / 2 + TEXTURE_HEIGHT)

// �W�����v����
#define	PLAYER_JUMP_CNT_MAX			(40)		// 40�t���[���Œ��n����
#define	PLAYER_JUMP_Y_MAX			(300.0f)	// �W�����v�̍���

// �v���C���[��HP
#define MAX_HP						(5000)

// �v���C���[�̃X�^�~�i
#define MAX_ENERGY					(50)

// �d��
#define PLAYER_GRAVITY				(8.0f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
//void DrawPlayerOffset(int no);
enum
{
	// ��l��
	TEXTURE_WALK,
	TEXTURE_CUT,
	TEXTURE_JUMP,
	TEXTURE_DEFENSE,

	//	�e
	TEXTURE_SHADOW000,

	// ���C�t
	TEXTURE_HEART_MAIN10,
	TEXTURE_HEARTBASE10,

	// �X�^�~�i�Q�[�W
	TEXTURE_ENERGY01,
	TEXTURE_ENERGY,
	TEXTURE_BRACh,
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/shujinnkou_walk.png",		// �v���C���[(����)
	"data/TEXTURE/shujinnkou_cut.png",		// �v���C���[(�U��)
	"data/TEXTURE/shujinnkou_jump.png",		// �v���C���[(�W�����v)
	"data/TEXTURE/shujinnkou_defense.png",	// �v���C���[(���)

	"data/TEXTURE/shadow.jpg",				// �ۉe

	"data/TEXTURE/heart_main10.png",		// HP�o�[
	"data/TEXTURE/heartbase10.png",			// HP�o�[(�O�g)

	"data/TEXTURE/energy01.png",			// �X�^�~�i�Q�[�W�̘g�g��
	"data/TEXTURE/energy.png",				// �X�^�~�i�Q�[�W
	"data/TEXTURE/brach.png",				// �X�^�~�i�Q�[�W
};


static BOOL		g_Load = FALSE;				// ���������s�������̃t���O
static PLAYER	g_Player[PLAYER_MAX];		// �v���C���[�\����
int				g_PlayerCnt;				// ���񂾃v���C���[�𐔂���
static float	g_jumppowr = 0;				// ����

// �v���C���[�̃A�j���[�V����
static TEX_PLAYER		 g_PlayerTexState[PLAYER_TEXTURE_MAX]
{
	//  no,  x, y, MAX,   w,     h,    wait		���Ԓʂ�ɏ�������

	{   0,   3, 1,  3, 100.0f, 100.0f,	4},// TEXTURE_WALK
	{   1,   4, 1,  4, 100.0f, 100.0f,	4},// TEXTURE_CUT
	{   2,   1, 1,  1, 100.0f, 100.0f,	4},// TEXTURE_JUMP
	{   3,   5, 2, 10, 100.0f, 100.0f,	4},// TEXTURE_DEFENSE
};

// �A�j���[�V�����p�^�[��
int g_playerstate;

// HP�E�X�^�~�i�o�[�̕`�撲��
float pw_log = 0.0f;
float tw_log = 0.0f;

// pose���j���[�̐؂�ւ�


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
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

	g_PlayerCnt = 0;		// ���񂾃v���C���[�𐔂���
	
	// �v���C���[�\���̂̏�����
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].use = TRUE;
		g_Player[i].pos = XMFLOAT3(150.0f, 500.0f, 0.0f);	// ���S�_����\��
		g_Player[i].pos_old = g_Player[i].pos;				// �v���C���[�̍��W�̕ۑ�

		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;

		g_Player[i].move = XMFLOAT3(4.0f, PLAYER_ACCEL_Y_DOWN, 0.0f);		// �ړ���

		g_Player[i].dir = PLAYER_DIR_DOWN;					// �������ɂ��Ƃ���
		g_Player[i].moving = FALSE;							// �ړ����t���O
		g_Player[i].pos_old = XMFLOAT3(0.0f, 0.0f, 0.0f);	// ���W�̕ۑ�

		// ���t���O
		g_Player[i].defense = FALSE;

		// �U�����󂯂����ǂ����̔���
		g_Player[i].receive = FALSE;

		// �`�F���W�^�C���̏�����
		g_Player[i].receivewait = 0;

		// �W�����v�̏�����
		g_Player[i].jump = FALSE;
		g_Player[i].jumpCnt = 0;
		g_Player[i].jumpY = 0.0f;
		g_Player[i].jumpYMax = PLAYER_JUMP_Y_MAX;

		// �v���C���[�̖��G����
		g_Player[i].invincibleCnt= 0;

		// �_���[�W���󂯂��疳�G��ԂɂȂ�
		g_Player[i].isDamaged = FALSE;

		// �v���C���[���C�t
		g_Player[i].life = MAX_HP;

		// �v���C���[�̃X�^�~�i
		g_Player[i].energy = MAX_ENERGY;

		// �v���C���[�̃��x���A�b�v
		g_Player[i].levelup = FALSE;

		// ����ԂɂȂ��Ă��牽�b�o�߂�����������
		g_Player[i].defense_time = 0.0f;
	}

	// �A�j���[�V�����p�^�[��
	g_playerstate = TEXTURE_WALK;

	// �|�[�Y���j���[�̐؂�ւ�

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
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
void UpdatePlayer(void)
{
	BG* bg = GetBG();



	
		PlayerUpdate();
	


	// ������Z�[�u����
	if (GetKeyboardTrigger(DIK_S))
	{
		SaveData();
	}


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{

	BG* bg = GetBG();

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

	float scrollX = GetScrollX();
	float scrollY = GetScrollY();


	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// ���̃v���C���[���g���Ă���H
		{									// Yes

			{	// �e�\��
				SetBlendState(BLEND_MODE_SUBTRACT);	// ���Z����

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SHADOW000]);

				float px = g_Player[i].pos.x - scrollX;	// �v���C���[�̕\���ʒuX
				float py = g_Player[i].pos.y - scrollY;	// �v���C���[�̕\���ʒuY
				float pw = g_Player[i].w;				// �v���C���[�̕\����
				float ph = g_Player[i].h/4;				// �v���C���[�̕\������
				py += 50.0f;							// �����ɕ\��

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				SetBlendState(BLEND_MODE_ALPHABLEND);	// ���������������ɖ߂�

			}

			// �v���C���[�{�l�̕`��
			
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_PlayerTexState[g_playerstate].id]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Player[i].pos.x - scrollX;						// �v���C���[�̕\���ʒuX
			float py = g_Player[i].pos.y - scrollY;						// �v���C���[�̕\���ʒuY
			float pw = g_PlayerTexState[g_playerstate].w;				// �v���C���[�̕\����
			float ph = g_PlayerTexState[g_playerstate].h;				// �v���C���[�̕\������

			py += g_Player[i].jumpY;									// �W�����v���̍����𑫂�

			// �A�j���[�V�����p
			float tw = 1.0f / g_PlayerTexState[g_playerstate].animex;										// �e�N�X�`���̕�
			float th = 1.0f / g_PlayerTexState[g_playerstate].animey;										// �e�N�X�`���̍���
			float tx = (float)(g_Player[i].patternAnim % g_PlayerTexState[g_playerstate].animex) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Player[i].patternAnim / g_PlayerTexState[g_playerstate].animex) * th;	// �e�N�X�`���̍���Y���W

			// ���]����
			if (g_Player[i].dir == PLAYER_DIR_LEFT)
			{
				// -1��������Ɣ��]
				tw *= -1;

				// �\���ʒu������Ȃ��悤�ɂ��Ă���
				tx += 1.0f / g_PlayerTexState[g_playerstate].animex;

			}

			// �U�����󂯂���F��ς���
			 
			// �ʏ펞
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Player[i].rot.z);

			g_Player[i].receivewait++;

			// �����U�����󂯂���
			if (g_Player[i].receive == TRUE)
			{
					// �F�ɕς���
					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
						g_Player[i].rot.z);

					// ���ԉ߂����猳�̐F�ɖ߂�
					if (g_Player[i].receivewait >= PLAYER_COLOR_CHANGE_TIME)
					{
						g_Player[i].receive = FALSE;
						g_Player[i].receivewait = 0;

					}

			}

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);


			// HP�Q�[�W
			SetHpGauge();


			// �X�^�~�i�Q�[�W
			SetStaminaGauge();

		}
	}


}


//=============================================================================
// Player�\���̂̐擪�A�h���X���擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}

// ���x���A�b�v
void SetLevelUP(void) {

	g_Player[0].levelup = TRUE;

}

// �_���[�W
void SetDamage(int damage) {

	g_Player[0].life -= damage;

}

// PlayerUpdate�֐�
void PlayerUpdate(void)
{

	BG* bg = GetBG();
	LEVEL* level = GetLevel();

	for (int i = 0; i < PLAYER_MAX; i++)
	{

		// �����Ă�v���C���[��������������
		if (g_Player[i].use == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			g_Player[0].pos_old = g_Player[0].pos;


			// �A�j���[�V����  
			if (!(g_Player[i].moving == FALSE && g_playerstate == TEXTURE_WALK))
			{
				g_Player[i].countAnim += 1.0f;
				if (g_Player[i].countAnim > ANIM_WAIT)
				{
					g_Player[i].countAnim = 0.0f;

					// �A�j���V�������I���^�C�~���O
					if ((g_Player[i].patternAnim + 1) >= g_PlayerTexState[g_playerstate].animemax)
					{
						// �U�����[�V�������W�����v��������
						if (g_playerstate == TEXTURE_CUT || g_playerstate == TEXTURE_JUMP)
						{
							g_playerstate = TEXTURE_WALK;
						}
					}
					//// �p�^�[���̐؂�ւ�
					g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % g_PlayerTexState[g_playerstate].animemax;

				}
			}


			// �v���C���[����

			// �v���C���[�̃X�s�[�h
			float speed = g_Player[i].move.x;

			g_Player[i].moving = FALSE;

			// �_�b�V������
			if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, BUTTON_R))
			{
				speed *= 2;
			}

			// �W�����v����
			// �W�����v��
			if (g_Player[i].defense == FALSE)
			{
				if (g_Player[i].jump == TRUE)
				{

					g_Player[i].pos.y -= g_jumppowr;

					if (g_jumppowr >= 2.0f)
					{
						g_jumppowr -= 1.0f;
					}
					else
					{
						g_jumppowr = 0.0f;
					}
				}
				else if ((g_Player[i].jump == FALSE) && (GetKeyboardTrigger(DIK_F) || IsButtonTriggered(0, BUTTON_A)))
				{
					// �W�����v���Ă��Ȃ��Ƃ�(�܂��n�ʂɂ���)
					g_playerstate = TEXTURE_JUMP;

					g_Player[i].jump = TRUE;
					g_jumppowr = 28.0f;
				}


			}


			// ���E�ړ�
			// ��D��Ԃł͂Ȃ��Ƃ��̂݁A�������Ƃ��ł���
			if (g_Player[i].defense == FALSE)
			{
				// �L�[�{�[�h
				if (GetKeyboardPress(DIK_RIGHT))
				{
					// �����A�j���[�V�����ɖ߂�
					g_playerstate = TEXTURE_WALK;

					g_Player[i].pos.x += speed;
					g_Player[i].dir = PLAYER_DIR_RIGHT;
					g_Player[i].moving = TRUE;

				}
				else if (GetKeyboardPress(DIK_LEFT))
				{
					// �����A�j���[�V�����ɖ߂�
					g_playerstate = TEXTURE_WALK;

					g_Player[i].pos.x -= speed;
					g_Player[i].dir = PLAYER_DIR_LEFT;
					g_Player[i].moving = TRUE;

				}

				//(�Q�[���p�b�h)
				if (IsButtonPressed(0, BUTTON_RIGHT))
				{
					// �����A�j���[�V�����ɖ߂�
					g_playerstate = TEXTURE_WALK;

					g_Player[i].pos.x += speed;
					g_Player[i].dir = PLAYER_DIR_RIGHT;
					g_Player[i].moving = TRUE;
				}
				else if (IsButtonPressed(0, BUTTON_LEFT))
				{
					// �����A�j���[�V�����ɖ߂�
					g_playerstate = TEXTURE_WALK;

					g_Player[i].pos.x -= speed;
					g_Player[i].dir = PLAYER_DIR_LEFT;
					g_Player[i].moving = TRUE;
				}

			}



			// �ߋ����U�� (�L�[���́C�Q�[���p�b�h)
			// ����Ԃł͂Ȃ��Ƃ��̂݁A�U���ł���
			if (g_Player[i].defense == FALSE)
			{
				if (GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_B))
				{
					g_playerstate = TEXTURE_CUT;

					XMFLOAT3 pos = g_Player[i].pos;
					pos.y += g_Player[i].jumpY;

					SetCutting(pos);

				}
			}

			// �������U��
			// ����Ԃł͂Ȃ��Ƃ��̂݁A�U���ł���
			if (g_Player[i].defense == FALSE)
			{
				if (level[i].level >= 5)
				{
					if (g_Player[i].energy >= 0)
					{
						if (GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_L))
						{
							XMFLOAT3 bullet_pos = g_Player[i].pos;
							bullet_pos.y += g_Player[i].jumpY;

							SetBullet(bullet_pos);

							g_Player[i].energy -= 10.0f;

						}
					}
				}
			}

			// ��菈��(��������ԂȂ�U�����󂯂Ȃ��B�������A�G�l���M�[�͏����)
			if (g_Player[i].energy >= 0)
			{
				g_Player[i].defense_time++;

				// A�L�[�������́AR2�{�^�����������Ƃ��Ɏ���ԂɂȂ�
				if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_R2))
				{
					g_playerstate = TEXTURE_DEFENSE;

					g_Player[i].defense = TRUE;

					g_Player[i].energy -= 0.1f;
				}

				// A�L�[�������́AR2�{�^���𗣂��������Ԃ���������
				if (GetKeyboardRelease(DIK_A) || IsButtonRelease(0, BUTTON_R2) || g_Player[i].energy <= 0)
				{
					g_Player[i].defense = FALSE;

					g_playerstate = TEXTURE_WALK;

				}
			}


			// �����A�G�l���M�[���}�b�N�X�G�l���M�[��菭�Ȃ�������
			if (g_Player[i].energy < MAX_ENERGY)
			{
				// �l�̕������₷
				g_Player[i].energy += 0.1f;

			}



			// �����蔻��
			// ���
			{
				float y = g_Player[i].pos.y - g_Player[i].h / 2;

				float sx = g_Player[i].pos.x - g_Player[i].w / 2 + 10.0f;
				XMFLOAT3 spos = XMFLOAT3(sx, y, 0.0f);

				float ex = g_Player[i].pos.x + g_Player[i].w / 2 - 10.0f;
				XMFLOAT3 epos = XMFLOAT3(ex, y, 0.0f);

				if (CollisionEdgeAndMapChip(spos, epos, UPPER))
				{
					g_Player[i].pos.y = g_Player[i].pos_old.y;
					//g_Player[i].move.y = 0.0f;
					g_Player[i].jump = FALSE;

				}
				else
				{
					//g_Player[i].move.y = PLAYER_ACCEL_Y_DOWN;
					g_Player[i].jump = TRUE;

				}
			}
			//����
			{
				float y = g_Player[i].pos.y + g_Player[i].h / 2;

				float sx = g_Player[i].pos.x - g_Player[i].w / 2 + 10.0f;
				XMFLOAT3 spos = XMFLOAT3(sx, y, 0.0f);

				float ex = g_Player[i].pos.x + g_Player[i].w / 2 - 10.0f;
				XMFLOAT3 epos = XMFLOAT3(ex, y, 0.0f);

				if (CollisionEdgeAndMapChip(spos, epos, LOWER))
				{
					if (g_Player[i].pos.y > g_Player[i].pos_old.y)
					{
						g_Player[i].pos.y = g_Player[i].pos_old.y;
					}

					// �W�����v�����s�����t���[���ŃW�����v�t���O���I�t�ɂ��Ȃ�����
					// �W�����v�͂�ς�����������O���r�e�B�ƃW�����v�p���[��ς���
					if (g_jumppowr <= 0.0f)
					{
						g_Player[i].jump = FALSE;
					}

				}
				else
				{
					g_Player[i].pos.y += PLAYER_GRAVITY;
				}
			}
			//����
			{
				float x = g_Player[i].pos.x - g_Player[i].w / 2;

				float sy = g_Player[i].pos.y - g_Player[i].h / 2 + 5.0f;
				XMFLOAT3 spos = XMFLOAT3(x, sy, 0.0f);

				float ey = g_Player[i].pos.y + g_Player[i].h / 2 - 10.0f;
				XMFLOAT3 epos = XMFLOAT3(x, ey, 0.0f);

				if (CollisionEdgeAndMapChip(spos, epos, LEFT))
				{
					g_Player[i].pos.x = g_Player[i].pos_old.x;
				}
			}
			//�E��
			{
				float x = g_Player[i].pos.x + g_Player[i].w / 2;

				float sy = g_Player[i].pos.y - g_Player[i].h / 2 + 5.0f;
				XMFLOAT3 spos = XMFLOAT3(x, sy, 0.0f);

				float ey = g_Player[i].pos.y + g_Player[i].h / 2 - 10.0f;
				XMFLOAT3 epos = XMFLOAT3(x, ey, 0.0f);

				if (CollisionEdgeAndMapChip(spos, epos, RIGHT))
				{
					g_Player[i].pos.x = g_Player[i].pos_old.x;

				}
			}




			// MAP�O�`�F�b�N

			if (g_Player[i].pos.x < 0.0f)
			{
				g_Player[i].pos.x = 0.0f;
			}

			if (g_Player[i].pos.x > FIELD_WIDTH)
			{
				g_Player[i].pos.x = FIELD_WIDTH;
			}

			if (g_Player[i].pos.y < 0.0f)
			{
				g_Player[i].pos.y = 0.0f;
			}

			if (g_Player[i].pos.y > FIELD_HEIGHT)
			{
				g_Player[i].pos.y = FIELD_HEIGHT;
			}

			// �v���C���[�̗����ʒu����MAP�̃X�N���[�����W���v�Z����
			bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
			if (bg->pos.x < 0) bg->pos.x = 0;
			if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

			bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
			if (bg->pos.y < 0) bg->pos.y = 0;
			if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;



			LEVEL* level = GetLevel();

		}

		// �_���[�W���󂯂��疳�G���
		if (g_Player[i].isDamaged == TRUE)
		{
			g_Player[i].invincibleCnt++;
		}

		// ���G���Ԃ̏�����
		if (g_Player[i].invincibleCnt >= PLAYER_INVINCIBLE_FRAME)
		{
			g_Player[i].invincibleCnt = 0;
			g_Player[i].isDamaged = FALSE;
		}

		// �����v���C���[�̗̑͂�0�Ȃ�Q�[���I�[�o�[
		if (g_Player[i].life <= 0)
		{
			g_PlayerCnt++;
		}

		// �v���C���[���S�����񂾂�
		if (g_PlayerCnt == PLAYER_MAX)
		{
			g_Player[i].use = FALSE;
			SetFadeMode(FADE_OUT, MODE_GAMEOVER_RESULT);
		}

	}


}



// HP�Q�[�W
void SetHpGauge(void)
{

		// ���~���̃Q�[�W�i�g�I�ȕ��j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_HEARTBASE10]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 10.0f;	// �Q�[�W�̕\���ʒuX
		float py = 10.0f;	// �Q�[�W�̕\���ʒuY
		float pw = 300.0f;// �Q�[�W�̕\����
		float ph = 30.0f;	// �Q�[�W�̕\������
			  
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
			  
	   // �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// ���C�t
		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		 px = 10.0f;	// �Q�[�W�̕\���ʒuX
		 py = 10.0f;	// �Q�[�W�̕\���ʒuY
		 pw = 300.0f;	// �Q�[�W�̕\����
		 ph = 30.0f;	// �Q�[�W�̕\������

		 tw = 1.0f;		// �e�N�X�`���̕�
		 th = 1.0f;		// �e�N�X�`���̍���
		 tx = 0.0f;		// �e�N�X�`���̍���X���W
		 ty = 0.0f;		// �e�N�X�`���̍���Y���W



		pw = pw * (float)g_Player[0].life / MAX_HP;
		tw = tw * (float)g_Player[0].life / MAX_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_HEART_MAIN10]);


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
}

// �X�^�~�i�Q�[�W
void SetStaminaGauge(void)
{
	{


		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 20.0f;					// �Q�[�W�̕\���ʒuX
		float py = 60.0f;					// �Q�[�W�̕\���ʒuY
		float pw = 265.0;					// �Q�[�W�̕\����
		float ph = 14.0f;					// �Q�[�W�̕\������

		float tw = 1.0f;					// �e�N�X�`���̕�
		float th = 1.0f;					// �e�N�X�`���̍���
		float tx = 0.0f;					// �e�N�X�`���̍���X���W
		float ty = 0.0f;					// �e�N�X�`���̍���Y���W

		pw = pw * (float)g_Player[0].energy / MAX_ENERGY;
		tw = tw * (float)g_Player[0].energy / MAX_ENERGY;

		//pw_log = (float)(g_Player[0].energy / MAX_ENERGY);
		//tw_log = tw;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_ENERGY]);


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �����w�i
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BRACh]);


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);



		// �g�g��
		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		px = 10.0f;				// �Q�[�W�̕\���ʒuX
		py = 60.0f;				// �Q�[�W�̕\���ʒuY
		pw = 290.0;				// �Q�[�W�̕\����
		ph = 18.0f;				// �Q�[�W�̕\������

		tw = 1.0f;				// �e�N�X�`���̕�
		th = 1.0f;				// �e�N�X�`���̍���
		tx = 0.0f;				// �e�N�X�`���̍���X���W
		ty = 0.0f;				// �e�N�X�`���̍���Y���W



	   // �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_ENERGY01]);


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);



	}

}