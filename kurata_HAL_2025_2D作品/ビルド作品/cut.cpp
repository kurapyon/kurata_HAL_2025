//=============================================================================
//
// �o���b�g���� [bullet.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "Level.h"
#include "bg.h"
#include "playereffect.h"
#include "player.h"
#include "cut.h"
#include "tutorialenemy.h"
#include "map.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// �a���T�C�Y
#define TEXTURE_HEIGHT				(100)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(5)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l
#define COOLTIME					(25)	// �U���̊Ԋu

#define ATCPAWER					(2)	// �U����


enum
{
	CUTTING_DIR_UP,
	CUTTING_DIR_RIGHT,
	CUTTING_DIR_DOWN,
	CUTTING_DIR_LEFT,
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/cutting.png"
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static CUTTING	g_Cutting[CUTTING_MAX];	// �o���b�g�\����

int cooltime = COOLTIME;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitCutting(void)
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


	// �o���b�g�\���̂̏�����
	for (int i = 0; i < CUTTING_MAX; i++)
	{
		g_Cutting[i].use   = FALSE;			// ���g�p�i�g���Ă��Ȃ��a���j
		g_Cutting[i].w     = TEXTURE_WIDTH;
		g_Cutting[i].h     = TEXTURE_HEIGHT;
		g_Cutting[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_Cutting[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Cutting[i].texNo = 1;

		g_Cutting[i].countAnim = 0;
		g_Cutting[i].patternAnim = 0;

		g_Cutting[i].move = XMFLOAT3(0.0f, -CUTTING_SPEED, 0.0f);	// �ړ��ʂ�������
		g_Cutting[i].xmove = XMFLOAT3(-CUTTING_SPEED, 0.0f, 0.0f);	//�a���������p�̈ړ��ʏ�����
		g_Cutting[i].bulletlife = 0;
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitCutting(void)
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

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateCutting(void)
{
	if (cooltime < COOLTIME)
	{
		cooltime ++;
	}


	PLAYER* player = GetPlayer();
	CUTTING* cutting = GetCutting();

	int cuttingCount = 0;				// ���������o���b�g�̐�

	for (int i = 0; i < CUTTING_MAX; i++)
	{
		if (g_Cutting[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{				
			
				g_Cutting[i].bulletlife--;
				if (g_Cutting[i].bulletlife == 0)
				{
					g_Cutting[i].use = FALSE;
				}

			

			
			// Yes
			// �A�j���[�V����  
			g_Cutting[i].countAnim++;
			if ((g_Cutting[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_Cutting[i].patternAnim = (g_Cutting[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			if (g_Cutting[i].dir == CUTTING_DIR_DOWN)
			{
				// �o���b�g�̈ړ�����
				XMVECTOR pos = XMLoadFloat3(&g_Cutting[i].pos) ;

				if (g_Cutting[i].bulletmode == 1)
				{
					g_Cutting[i].move = XMFLOAT3(0.0f,CUTTING_SPEED,0.0f);
					XMVECTOR move = XMLoadFloat3(&g_Cutting[i].move);
					pos += move;
				}

				XMStoreFloat3(&g_Cutting[i].pos, pos);
			}
			else if (g_Cutting[i].dir == CUTTING_DIR_UP)
			{
				// �o���b�g�̈ړ�����
				XMVECTOR pos = XMLoadFloat3(&g_Cutting[i].pos);
				//XMVECTOR move = XMLoadFloat3(&g_Cutting[i].move);
				//pos += move;


				if (g_Cutting[i].bulletmode == 1)
				{
					g_Cutting[i].move = XMFLOAT3(0.0f, -CUTTING_SPEED, 0.0f);
					XMVECTOR move = XMLoadFloat3(&g_Cutting[i].move);
					pos += move;
				}


				XMStoreFloat3(&g_Cutting[i].pos, pos);
			}
			else if (g_Cutting[i].dir == CUTTING_DIR_LEFT)
			{
				XMVECTOR pos = XMLoadFloat3(&g_Cutting[i].pos);
				//XMVECTOR xmove = XMLoadFloat3(&g_Cutting[i].xmove);
				//pos += xmove;
				


				if (g_Cutting[i].bulletmode == 1)
				{
					g_Cutting[i].move = XMFLOAT3(-CUTTING_SPEED,1.0f, 0.0f);
					XMVECTOR move = XMLoadFloat3(&g_Cutting[i].move);
					pos += move;
				}

				XMStoreFloat3(&g_Cutting[i].pos, pos);
			}
			else if (g_Cutting[i].dir == CUTTING_DIR_RIGHT)
			{
				XMVECTOR pos = XMLoadFloat3(&g_Cutting[i].pos);
				//XMVECTOR xmove = XMLoadFloat3(&g_Cutting[i].xmove);
				//
				//pos -= xmove;


				if (g_Cutting[i].bulletmode == 1)
				{
					g_Cutting[i].move = XMFLOAT3(CUTTING_SPEED, 1.0f, 0.0f);
					XMVECTOR move = XMLoadFloat3(&g_Cutting[i].move);
					pos += move;
				}

				XMStoreFloat3(&g_Cutting[i].pos, pos);
			}



			// ��ʊO�܂Ői�񂾁H
			BG* bg = GetBG();
			if (g_Cutting[i].pos.y < (-g_Cutting[i].h/2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Cutting[i].use = FALSE;
			}
			if (g_Cutting[i].pos.y > (bg->h + g_Cutting[i].h/2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Cutting[i].use = FALSE;
			}




			// �����蔻�菈��
			{
				ENEMY* enemy = GetEnemy();

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (enemy[j].use == TRUE)
					{
							BOOL ans = CollisionBB(g_Cutting[i].pos, g_Cutting[i].w, g_Cutting[i].h,
										enemy[j].pos, enemy[j].w, enemy[j].h);

							if (ans == TRUE)
							{
								// �U����
								PlaySound(SOUND_LABEL_cut01);

								// �����������̏���(�G�l�~�[�̃��C�t�����炷,�U�����󂯂��t���O�𗧂Ă�)
								enemy[j].life -= ATCPAWER;

								// �G�l�~�[�̐F��ς���
								enemy[j].receive = TRUE;

								// �o���l��������
								GuagePlus(1);

							}

							// �����A�G�l�~�[�̃��C�t��0�Ȃ�G�l�~�[������
							if (enemy[j].life <= 0)
							{
								enemy[j].use = FALSE;

							}


						//}
					}
				}

				BOSS* boss = GetBoss();
					// �����Ă�{�X�Ɠ����蔻�������
					if (boss[0].boss_use == TRUE)
					{

							BOOL ans_boss = CollisionBB(g_Cutting[i].pos, g_Cutting[i].w, g_Cutting[i].h,
								boss[0].boss_pos, boss[0].boss_w, boss[0].boss_h);

							if (ans_boss == TRUE)
							{
								PlaySound(SOUND_LABEL_cut01);

								// �����������̏���
								boss[0].boss_life -= ATCPAWER;

								// �{�X�̐F��ς���
								boss[0].boss_receive = TRUE;

								// �o���l�𑝂₷
								GuagePlus(1);


							}

							// �����A�G�l�~�[�̃��C�t��0�Ȃ�G�l�~�[������
							if (boss[0].boss_life <= 0)
							{
								boss[0].boss_use = FALSE;

							}


					}

					TUTORIAL_ENEMY* tutorial = GetTutorial();
					for (int t = 0; t < TUTORIAL_ENEMY_MAX; t++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if (tutorial[t].use == TRUE)
						{
							BOOL ans_tutorial = CollisionBB(g_Cutting[i].pos, g_Cutting[i].w, g_Cutting[i].h,
								tutorial[t].pos, tutorial[t].w, tutorial[t].h);

							// �������Ă���H
							if (ans_tutorial == TRUE)
							{
								PlaySound(SOUND_LABEL_cut01);

								// �����������̏���(�G�l�~�[�̃��C�t�����炷,�U�����󂯂��t���O���グ��)
								tutorial[t].life -= ATCPAWER;

								tutorial[t].receive = TRUE;

								// �����A�G�l�~�[�̃��C�t��t�Ȃ�G�l�~�[������
								if (tutorial[t].life <= 0)
								{
									tutorial[t].use = FALSE;

								}

							}
						}

					}


			}


			cuttingCount++;
		}
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawCutting(void)	// �J�b�g���o�̓A�j���[�V�������ōs���B�G�t�F�N�g�͕t����
{
	// �Ȃ�
}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
CUTTING *GetCutting(void)
{
	return &g_Cutting[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetCutting(XMFLOAT3 pos)
{
	PLAYER* player = GetPlayer();

	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < CUTTING_MAX; i++)
	{
		if (g_Cutting[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			if (cooltime != COOLTIME)
			{
				return;
			}

				g_Cutting[i].bulletlife = 20;
			cooltime = 0;
			g_Cutting[i].use = TRUE;		// �g�p��Ԃ֕ύX����
			g_Cutting[i].dir = CUTTING_DIR_UP;

			if (player[0].dir == PLAYER_DIR_UP)	// �v���C���[�̌����ɂ���ĕς���
			{

				g_Cutting[i].pos = pos;			// ���W���Z�b�g
				g_Cutting[i].dir = CUTTING_DIR_UP;
				g_Cutting[i].rot = XMFLOAT3(0.0f, 0.0f, 3.14f * 0.5f);

			}
			else if (player[0].dir == PLAYER_DIR_DOWN)
			{

				g_Cutting[i].pos = pos;			// ���W���Z�b�g
				g_Cutting[i].dir = CUTTING_DIR_DOWN;
				g_Cutting[i].rot = XMFLOAT3(0.0f, 0.0f, 3.14f * 1.5f);

			}
			else if (player[0].dir == PLAYER_DIR_LEFT)
			{
				pos.x -= TEXTURE_WIDTH / 2;
				g_Cutting[i].pos = pos;			// ���W���Z�b�g
				g_Cutting[i].dir = CUTTING_DIR_LEFT;
				g_Cutting[i].rot = XMFLOAT3(0.0f, 0.0f,0.0f);

			}
			else if (player[0].dir == PLAYER_DIR_RIGHT)
			{
				pos.x += TEXTURE_WIDTH / 2;
				g_Cutting[i].pos = pos;			// ���W���Z�b�g
				g_Cutting[i].dir = CUTTING_DIR_RIGHT;
				g_Cutting[i].rot = XMFLOAT3(0.0f, 0.0f, 3.14f);

			}
			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}

