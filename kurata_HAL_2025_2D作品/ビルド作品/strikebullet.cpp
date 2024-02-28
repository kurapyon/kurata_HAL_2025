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
#include "strikebullet.h"
#include "map.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define STRIKE_TEXTURE_WIDTH				(100)			// �L�����T�C�Y
#define STRIKE_TEXTURE_HEIGHT				(100)			// 
#define STRIKE_TEXTURE_MAX					(2)				// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X			(1)				// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y			(1)				// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define STRIKE_ANIM_PATTERN_NUM				(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����

#define TEXTURE_PATTERN_BULLET_X			(4)				// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_BULLET_Y			(1)				// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define STRIKE_ANIM_BULLET_NUM				(TEXTURE_PATTERN_BULLET_X*TEXTURE_PATTERN_BULLET_Y)	// �A�j���[�V�����p�^�[����

#define STRIKE_ANIM_WAIT					(4)				// �A�j���[�V�����̐؂�ւ��Wait�l

#define BULLET_WAIT							(60)			// �o���b�g�̔��˂̃^�C�~���O�����킹�Ă���
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
enum 
{
	enemy_bullet00,
	enemy_bullet01,
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[STRIKE_TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[STRIKE_TEXTURE_MAX] = {
	"data/TEXTURE/enemy_bullet00.png",
	"data/TEXTURE/enemy_bullet01.png",

};

static BOOL		g_Load = FALSE;					// ���������s�������̃t���O
static STRIKE	g_strike[STRIKE_BULLET_MAX];	// �o���b�g�\����

float			g_bullettime = 0.0f;			// ���Ԍv��

//=============================================================================
// ����������
//=============================================================================
HRESULT InitStrikeBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < STRIKE_TEXTURE_MAX; i++)
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
	for (int i = 0; i < STRIKE_BULLET_MAX; i++)
	{
		g_strike[i].strike_use			= FALSE;								// ���g�p�i���˂���Ă��Ȃ��e�j
		g_strike[i].strike_w			= STRIKE_TEXTURE_WIDTH;
		g_strike[i].strike_h			= STRIKE_TEXTURE_HEIGHT;
		g_strike[i].strike_pos			= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_strike[i].strike_rot			= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_strike[i].strike_P0			= XMFLOAT3(0.0f, 0.0f, 0.0f);			// ����_P0
		g_strike[i].strike_P1			= XMFLOAT3(0.0f, 0.0f, 0.0f);			// ����_P1
		g_strike[i].strike_speed		= 0.0f;
		g_strike[i].strike_countAnim	= 0;
		g_strike[i].strike_patternAnim  = 0;
		g_strike[i].strike_move			= XMFLOAT3(0.0f, - STRIKE_BULLET_SPEED, 0.0f);	// �ړ��ʂ�������
		g_strike[i].strike_hit			= 0;											// �ŏ��̓v���C���[
		g_strike[i].strike_tex			= 0;											// �e�N�X�`���[�̔ԍ�
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitStrikeBullet(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < STRIKE_TEXTURE_MAX; i++)
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
void UpdateStrikeBullet(void)
{
	int bulletCount = 0;				// ���������o���b�g�̐�

	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

	for (int i = 0; i < STRIKE_BULLET_MAX; i++)
	{
		if (g_strike[i].strike_use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			// �e�N�X�`���[�ɂ���ĕ����Ă���
			if (g_strike[i].strike_tex == enemy_bullet00)
			{
				// �A�j���[�V����  
				g_strike[i].strike_countAnim++;
				if ((g_strike[i].strike_countAnim % STRIKE_ANIM_WAIT) == 0)
				{
					// �p�^�[���̐؂�ւ�
					g_strike[i].strike_patternAnim = (g_strike[i].strike_patternAnim + 1) % STRIKE_ANIM_PATTERN_NUM;
				}

			}

			// �e�N�X�`���[�ɂ���ĕ����Ă���
			if (g_strike[i].strike_tex == enemy_bullet01)
			{
				// �A�j���[�V����  
				g_strike[i].strike_countAnim++;
				if ((g_strike[i].strike_countAnim % STRIKE_ANIM_WAIT) == 0)
				{
					// �p�^�[���̐؂�ւ�
					g_strike[i].strike_patternAnim = (g_strike[i].strike_patternAnim + 1) % STRIKE_ANIM_BULLET_NUM;
				}

			}


			XMVECTOR vold_pos = XMLoadFloat3(&g_strike[i].strike_pos);			
			// �o���b�g�̈ړ�����
			XMVECTOR pos = XMLoadFloat3(&g_strike[i].strike_pos);
			XMVECTOR move = XMLoadFloat3(&g_strike[i].strike_move);
			pos += move;
			XMStoreFloat3(&g_strike[i].strike_pos, pos);



			// ��ʊO�܂Ői�񂾁H
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			if (g_strike[i].strike_pos.y < (-g_strike[i].strike_h/2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_strike[i].strike_use = false;
			}
			if (g_strike[i].strike_pos.y > (FIELD_HEIGHT + g_strike[i].strike_h/2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_strike[i].strike_use = false;
			}


			// �����蔻�菈��(�G�l�~�[)
			{
				ENEMY* enemy = GetEnemy();
				PLAYER* player = GetPlayer();

				// �����蔻�菈��
				// �v���C���[�̐��������蔻����s��
				for (int p = 0; p < ENEMY_MAX; p++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (player[p].use == TRUE)
					{
						BOOL ans = CollisionBB(player[p].pos, player[p].w, player[p].h, g_strike[i].strike_pos, g_strike[i].strike_w, g_strike[i].strike_h);

						// �������Ă���H
						if (ans == TRUE)
						{
							// �v���C���[������Ԃł͂Ȃ������A�v���C���[�����G���ԊO�Ȃ�_���[�W��^����
							if (player[p].defense == FALSE && (player[p].invincibleCnt >= PLAYER_INVINCIBLE_FRAME || player[p].invincibleCnt == 0))
							{
								{
									// �����������̏���
									SetDamage(1);

									player[p].receive = TRUE;

									g_strike[i].strike_use = FALSE;

									// �G�t�F�N�g����
									//SetEnemyEffect(player[p].pos.x, player[p].pos.y, 30, 0);
								}
							}

							// �v���C���[������ԂȂ�AFALSE�ɂ���
							if (player[p].defense == TRUE)
							{
								g_strike[i].strike_use = FALSE;
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
void DrawStrikeBullet(void)
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
	float tht;

	for (int i = 0; i < STRIKE_BULLET_MAX; i++)
	{
		if (g_strike[i].strike_use == TRUE)		// ���̃o���b�g���g���Ă���H
		{										// Yes

			if (g_strike[i].strike_tex == enemy_bullet00)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy_bullet00]);

				//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_strike[i].strike_pos.x - scrollX;	// �o���b�g�̕\���ʒuX
				float py = g_strike[i].strike_pos.y - scrollY;	// �o���b�g�̕\���ʒuY
				float pw = g_strike[i].strike_w;				// �o���b�g�̕\����
				float ph = g_strike[i].strike_h;				// �o���b�g�̕\������

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				float tx = (float)(g_strike[i].strike_patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_strike[i].strike_patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �e�N�X�`���[�㉺���]����
				tht = th * -1;

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, tht,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_strike[i].strike_rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

			}

			if (g_strike[i].strike_tex == enemy_bullet01)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy_bullet01]);

				//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_strike[i].strike_pos.x - scrollX;	// �o���b�g�̕\���ʒuX
				float py = g_strike[i].strike_pos.y - scrollY;	// �o���b�g�̕\���ʒuY
				float pw = g_strike[i].strike_w;				// �o���b�g�̕\����
				float ph = g_strike[i].strike_h;				// �o���b�g�̕\������

				float tw = 1.0f / TEXTURE_PATTERN_BULLET_X;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_BULLET_Y;	// �e�N�X�`���̍���
				float tx = (float)(g_strike[i].strike_patternAnim % TEXTURE_PATTERN_BULLET_X) * tw;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_strike[i].strike_patternAnim / TEXTURE_PATTERN_BULLET_X) * th;	// �e�N�X�`���̍���Y���W

				// �e�N�X�`���[�㉺���]����
				/*tht = th * -1;*/

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_strike[i].strike_rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

			}

		}
	}
	
}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
STRIKE *GetStrikeBullet(void)
{
	return &g_strike[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetStrikeBullet(XMFLOAT3 pos, int tex)
{
		// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
		for (int i = 0; i < STRIKE_BULLET_MAX; i++)
		{
			// �X�s�[�h
			g_strike[i].strike_speed = STRIKE_BULLET_SPEED;

			// �e�N�X�`���[�̔ԍ�
			g_strike[i].strike_tex = tex;

			if (g_strike[i].strike_use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
			{
				g_strike[i].strike_use = TRUE;			// �g�p��Ԃ֕ύX����
				g_strike[i].strike_pos = pos;			// ���W���Z�b�g

				// �㉺�W�����ɔ�΂�
				g_strike[i].strike_move.x = sinf(XM_PI / 4 * i) * g_strike[i].strike_speed;
				g_strike[i].strike_move.y = cosf(XM_PI / 4 * i) * g_strike[i].strike_speed;

				return;									// 1���Z�b�g�����̂ŏI������
			}
		}
}


