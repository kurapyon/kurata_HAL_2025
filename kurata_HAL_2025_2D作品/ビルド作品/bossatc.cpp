//=============================================================================
//
// �o���b�g���� [bullet.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "player.h"
#include "enemy.h"
#include "collision.h"
#include "Level.h"
#include "bg.h"
#include "enemyeffect.h"
#include "map.h"
#include "bossatc.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(100)	// 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(8)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

enum
{
	bossbullet,
	enemy_bullet00,
};
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bossbullet.png",
	"data/TEXTURE/enemy_bullet00.png",
};

static BOOL			g_Load = FALSE;									// ���������s�������̃t���O
static BULLETATC	g_Bulletatc[ATC_BULLETE_MAX];					// �o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBulletAtc(void)
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
	for (int i = 0; i < ATC_BULLETE_MAX; i++)
	{
		g_Bulletatc[i].use   = FALSE;								// ���g�p�i���˂���Ă��Ȃ��e�j
		g_Bulletatc[i].w     = TEXTURE_WIDTH;
		g_Bulletatc[i].h     = TEXTURE_HEIGHT;
		g_Bulletatc[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bulletatc[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bulletatc[i].texNo = 0;
		g_Bulletatc[i].P0	 = XMFLOAT3(0.0f, 0.0f, 0.0f);			// ����_P0
		g_Bulletatc[i].P1	 = XMFLOAT3(1084.0f, 300.0f, 0.0f);		// ����_P0
		g_Bulletatc[i].s = 0.0f;

		g_Bulletatc[i].countAnim = 0;
		g_Bulletatc[i].patternAnim = 0;

		g_Bulletatc[i].move = XMFLOAT3(0.0f, BULLETE_SPEED, 0.0f);	// �ړ��ʂ�������
		
		g_Bulletatc[i].hit = 0;										// �ŏ��̓v���C���[
		g_Bulletatc[i].tex = 0;										// �e�N�X�`���[�̎���
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBulletAtc(void)
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
void UpdateBulletAtc(void)
{
	int bulletCount = 0;				// ���������o���b�g�̐�

	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

	for (int i = 0; i < ATC_BULLETE_MAX; i++)
	{
		if (g_Bulletatc[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			// �A�j���[�V����  
			g_Bulletatc[i].countAnim++;
			if ((g_Bulletatc[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_Bulletatc[i].patternAnim = (g_Bulletatc[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}


			// �o���b�g�̈ړ�����
			XMVECTOR pos  = XMLoadFloat3(&g_Bulletatc[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_Bulletatc[i].move);
			pos += move;
			XMStoreFloat3(&g_Bulletatc[i].pos, pos);

			// ��ʊO�܂Ői�񂾁H
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			if (g_Bulletatc[i].pos.y < (-g_Bulletatc[i].h/2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Bulletatc[i].use = false;
			}
			if (g_Bulletatc[i].pos.y > (FIELD_HEIGHT + g_Bulletatc[i].h/2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Bulletatc[i].use = false;
			}


			// �����蔻�菈��
			{
				PLAYER* player = GetPlayer();

				// �v���C���[�Ƃ������Ă���̂��A�G�l�~�[�Ɠ������Ă���̂����肷�� 
				// �v���C���[�̐��������蔻����s��
				for (int p = 0; p < ENEMY_MAX; p++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (player[p].use == TRUE)
					{
						BOOL ans = CollisionBB(player[p].pos, player[p].w, player[p].h, g_Bulletatc[i].pos, g_Bulletatc[i].w, g_Bulletatc[i].h);

						// �������Ă���H
						if (ans == TRUE)
						{
							// �v���C���[������Ԃł͂Ȃ������A���G���ԊO�Ȃ�_���[�W��^����
							if (player[p].defense == FALSE && (player[p].invincibleCnt >= PLAYER_INVINCIBLE_FRAME || player[p].invincibleCnt == 0))
							{
							
								// �����������̏���
								SetDamage(100);

								player[p].receive = TRUE;

								g_Bulletatc[i].use = FALSE;

								// �G�t�F�N�g����
								SetEnemyEffect(player[p].pos.x, player[p].pos.y, 30, TEXTURE_EFFECT00);
							
							}

							// ����ԂȂ�AFALSE�ɂ���
							if (player[p].defense == TRUE)
							{
								g_Bulletatc[i].use = FALSE;
							}

						}
					}
				}
			}

			bulletCount++;

		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBulletAtc(void)
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

	float scrollX = GetScrollX();
	float scrollY = GetScrollY();
	for (int i = 0; i < ATC_BULLETE_MAX; i++)
	{
		if (g_Bulletatc[i].tex == 0)
		{
			if (g_Bulletatc[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
			{									// Yes
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[bossbullet]);

				//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Bulletatc[i].pos.x - scrollX;	// �o���b�g�̕\���ʒuX
				float py = g_Bulletatc[i].pos.y - scrollY;	// �o���b�g�̕\���ʒuY
				float pw = g_Bulletatc[i].w;		// �o���b�g�̕\����
				float ph = g_Bulletatc[i].h;		// �o���b�g�̕\������

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				float tx = (float)(g_Bulletatc[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_Bulletatc[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Bulletatc[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);



				//// �G�l�~�[�o���b�g
				//// �e�N�X�`���ݒ�
				//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy_bullet00]);

				////�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
				//float px = g_Bulletatc[i].pos.x - scrollX;	// �o���b�g�̕\���ʒuX
				//float py = g_Bulletatc[i].pos.y - scrollY;	// �o���b�g�̕\���ʒuY
				//float pw = g_Bulletatc[i].w;		// �o���b�g�̕\����
				//float ph = g_Bulletatc[i].h;		// �o���b�g�̕\������

				//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				//float tx = (float)(g_Bulletatc[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				//float ty = (float)(g_Bulletatc[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				//// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				//SetSpriteColorRotation(g_VertexBuffer,
				//	px, py, pw, ph,
				//	tx, ty, tw, th,
				//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				//	g_Bulletatc[i].rot.z);

				//// �|���S���`��
				//GetDeviceContext()->Draw(4, 0);

			}

		}


	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLETATC* GetAtcBullet(void)
{
	return &g_Bulletatc[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetBulletAtc(BOOL tex)
{

	ENEMY* enemy = GetEnemy();

	 //�������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < ATC_BULLETE_MAX; i++)
	{
		if (g_Bulletatc[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			XMFLOAT3 pos;
			BOOL poscheck = FALSE;

			while (poscheck == FALSE)
			{

				pos.x = (float)(rand() % (int)FIELD_WIDTH);
				pos.y = 0.0f;
				pos.z = 0.0f;

				if ((pos.x >= 0) && (FIELD_WIDTH >= pos.x))
				{
					poscheck = TRUE;
				}


			}



			g_Bulletatc[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_Bulletatc[i].pos = pos;			// ���W���Z�b�g
			return;							// 1���Z�b�g�����̂ŏI������

		}
	}
}