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
#include "tutorialenemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************


// �e�N�X�`���̐�
#define TUTORIAL_TEXTURE_MAX						(5)		

// �`���[�g���A���G�l�~�[ �L�����T�C�Y
#define ENEMY_TEXTURE_WIDTH							(100)	
#define ENEMY_TEXTURE_HEIGHT						(100)	


// �`���[�g���A���G�l�~�[
#define TUTORIAL_TEXTURE_PATTERN_DIVIDE_X			(4)																		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TUTORIAL_TEXTURE_PATTERN_DIVIDE_Y			(1)																		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define TUTORIAL_ANIM_PATTERN_NUM					(TUTORIAL_TEXTURE_PATTERN_DIVIDE_X * TUTORIAL_TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define TUTORIAL_ANIM_WAIT							(4)																		// �A�j���[�V�����̐؂�ւ��Wait�l

// ���C�t
#define TUTORIAL_ENEMY_LIFE							(100)

// ��_���[�W�ɐF��ς��Ă��鎞��
#define TEXTURE_COLOR_CHANGE_TIME					(200)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

// 
enum 
{
	tutorial00,
	tutorial01,
	tutorial02,
	tutorial03,

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



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;						// ���_���
static ID3D11ShaderResourceView	*g_Texture[TUTORIAL_TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

// 5
static char *g_TexturName[TUTORIAL_TEXTURE_MAX] = {
	"data/TEXTURE/tutorial00.png",
	"data/TEXTURE/tutorial01.png",
	"data/TEXTURE/tutorial02.png",
	"data/TEXTURE/tutorial03.png",
	"data/TEXTURE/tutorial05.png",

};

// ���������s�������̃t���O
static BOOL		g_Load = FALSE;		

// �G�l�~�[�̐��𐔂���
int		g_tutorialCnt;

// �`���[�g���A���G�l�~�[�\����
static	TUTORIAL_ENEMY  g_tutorial[TUTORIAL_ENEMY_MAX];





//=============================================================================
// ����������
//=============================================================================
HRESULT  InitTutorial(void)
{
	ID3D11Device *pDevice = GetDevice();


	//�e�N�X�`������
	for (int i = 0; i < TUTORIAL_TEXTURE_MAX; i++)
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
			
	// �`���[�g���A���\���̂̏�����
	g_tutorialCnt = 0;

	for (int i = 0; i < TUTORIAL_ENEMY_MAX; i++)
	{
		g_tutorial[i].use				= TRUE;
		g_tutorial[i].ans				= FALSE;
		g_tutorial[i].pos				= XMFLOAT3(500.0f + (i * 150.0f), 585.0f, 0.0f);		// ���S�_����\��

		g_tutorial[i].rot				= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_tutorial[i].scl				= XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_tutorial[i].w					= ENEMY_TEXTURE_WIDTH;									// �e�N�X�`���[�̕�
		g_tutorial[i].h					= ENEMY_TEXTURE_HEIGHT;									// �e�N�X�`���[�̍���
		g_tutorial[i].countAnim			= 0;
		g_tutorial[i].patternAnim		= 0;
		g_tutorial[i].dir				= ENEMY_DIR_LEFT;										// �G�l�~�[�̌���(0:X�}�C�i�X�A��		1:X�v���X�A�E)
		g_tutorial[i].receive			= FALSE;												// �U�����󂯂����ǂ����̔���
		g_tutorial[i].id				= i;													// �Ǘ��ԍ�(ID)
		g_tutorial[i].move			    = XMFLOAT3(4.0f, 0.0f, 0.0f);							// �ړ���
		g_tutorial[i].life				= 100;													// ���C�t

		g_tutorial[i].time				= 0.0f;													// ���`��ԗp�̃^�C�}�[���N���A
		g_tutorial[i].tblNo				= 0;													// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_tutorial[i].tblMax			= 0;													// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
		g_tutorial[i].receivewait		= 0;													// ��_���[�W���ɐF��ς��鎞��
	}



	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTutorial(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TUTORIAL_TEXTURE_MAX; i++)
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
void UpdatTutorial(void)
{
	TUTORIAL_ENEMY_UPDATE();

	
#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorial(void)
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

	TUTORIAL_ENEMY_DRAW();

}


//=============================================================================
// �G�l�~�[�p�֐��ƃ{�X�֐�
//=============================================================================
void TUTORIAL_ENEMY_UPDATE(void)
{
	BG* bg = GetBG();

	if (bg->pose == FALSE)
	{
		// 1�X�e�[�W��(���̌�{�X��)
		for (int i = 0; i < TUTORIAL_ENEMY_MAX; i++)
		{
			// �����Ă�G�l�~�[��������������
			if (g_tutorial[i].use == TRUE)
			{
				// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
				XMFLOAT3 pos_old = g_tutorial[i].pos;

				// �A�j���[�V����  
				g_tutorial[i].countAnim += 1.0f;
				if (g_tutorial[i].countAnim > TUTORIAL_ANIM_WAIT)
				{
					g_tutorial[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					g_tutorial[i].patternAnim = (g_tutorial[i].patternAnim + 1) % TUTORIAL_ANIM_PATTERN_NUM;
				}


				// �{�X�̌����̒���
				PLAYER* player = GetPlayer();

				// �{�X���v���C���[��X���W���E�ɂ�����,���Ɍ�������
				if (player[0].pos.x > g_tutorial[i].pos.x)
				{
					g_tutorial[i].dir = ENEMY_DIR_RIGHT;
				}

				// �����{�X���v���C���[��X���W��荶�ɂ�����A�E�Ɍ�������
				if (player[0].pos.x < g_tutorial[i].pos.x)
				{
					g_tutorial[i].dir = ENEMY_DIR_LEFT;
				}

				// �v���C���[�Ƃ̓����蔻��A�G�l�~�[�̍U��
				{
					PLAYER* player = GetPlayer();

					// �G�l�~�[�̐������肷��
					for (int j = 0; j < TUTORIAL_ENEMY_MAX; j++)
					{
						// �v���C���[�Ɠ����蔻�������
						if (player[j].use == TRUE && player[j].invincibleCnt >= PLAYER_INVINCIBLE_FRAME || player[j].invincibleCnt == 0)
						{
							g_tutorial[i].ans = CollisionBB(g_tutorial[i].pos, g_tutorial[i].w, g_tutorial[i].h,
								player[j].pos, player[j].w, player[j].h);

							// �������Ă���H
							if (g_tutorial[i].ans == TRUE)
							{
								// �����������̏���

								SetDamage(10);

								player[i].receive = TRUE;

								player[j].isDamaged = TRUE;

							}
						}

					}

				}

				//g_tutorialCnt++;
			}


		}

	}


	// �G�l�~�[��S���|������A�`���[�g���A���I���
	if (g_tutorial[0].use == FALSE && g_tutorial[1].use == FALSE && g_tutorial[2].use == FALSE && g_tutorial[3].use == FALSE)
	{
		SetFadeMode(FADE_OUT, MODE_GAME);
	}


}



void TUTORIAL_ENEMY_DRAW(void)
{

	float scrollX = GetScrollX();
	float scrollY = GetScrollY();

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

	//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = 800.0f  - scrollX;								// �G�l�~�[�̕\���ʒuX
	float py = 300.0f  - scrollY;								// �G�l�~�[�̕\���ʒuY
	float pw = 800.0f;								// �G�l�~�[�̕\����
	float ph = 400.0f;								// �G�l�~�[�̕\������

	 //�A�j���[�V�����p
	float tw = 1.0f;								// �e�N�X�`���̕�
	float th = 1.0f;								// �e�N�X�`���̍���
	float tx = 0.0f;								// �e�N�X�`���̍���X���W
	float ty = 0.0f;								// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	for (int i = 0; i < TUTORIAL_ENEMY_MAX; i++)
	{
		if (g_tutorial[i].use == TRUE)
		{
			// �G�l�~�[
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_tutorial[i].pos.x - scrollX;	// �G�l�~�[�̕\���ʒuX
			float py = g_tutorial[i].pos.y - scrollY;	// �G�l�~�[�̕\���ʒuY
			float pw = g_tutorial[i].w;					// �G�l�~�[�̕\����
			float ph = g_tutorial[i].h;					// �G�l�~�[�̕\������

			 //�A�j���[�V�����p
			float tw = -1 * (1.0f / TUTORIAL_TEXTURE_PATTERN_DIVIDE_X);									// �e�N�X�`���̕�
			float th = 1.0f / TUTORIAL_TEXTURE_PATTERN_DIVIDE_Y;									// �e�N�X�`���̍���
			float tx = (float)(g_tutorial[i].patternAnim % TUTORIAL_TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_tutorial[i].patternAnim / TUTORIAL_TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �����v���C���[�̍��ɂ����甽�]������
			if (g_tutorial[i].dir == ENEMY_DIR_LEFT)
			{
				// -1��������Ɣ��]
				tw *= -1;

				// �\���ʒu������Ȃ��悤�ɂ��Ă���
				tx += 1.0f / TUTORIAL_TEXTURE_PATTERN_DIVIDE_X;

			}

			// �U�����󂯂���F��ς���
			
			// �ʏ펞
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_tutorial[i].rot.z);


			g_tutorial[i].receivewait++;

			// �����A�U�����󂯂���ԐF�ɂ���
			if (g_tutorial[i].receive == TRUE)
			{
				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
					g_tutorial[i].rot.z);
				
				// �����J���[�`�F���W�^�C���𒴂�����F��߂�
				if (g_tutorial[i].receivewait >= TEXTURE_COLOR_CHANGE_TIME)
				{
					// ���̐F�ɖ߂�
					g_tutorial[i].receive = FALSE;
					g_tutorial[i].receivewait += 0;

				}
			}

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);




		}
	}





}







//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
TUTORIAL_ENEMY* GetTutorial(void)
{
	return &g_tutorial[0];
}


