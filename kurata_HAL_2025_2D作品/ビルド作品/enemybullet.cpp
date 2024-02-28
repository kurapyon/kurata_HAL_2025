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
#include "bosbullet.h"
#include "fade.h"
#include "enemybullet.h"
#include "input.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ENEMY_BULLET_TEXTURE_WIDTH								(100)																			// �L�����T�C�Y
#define ENEMY_BULLET_TEXTURE_HEIGHT								(100)																			// 
#define ENEMY_BULLET_TEXTURE_MAX								(2)																				// �e�N�X�`���̐�
#define ENEMY_BULLET_SPEED										(10.0f)																			// �X�s�[�h

// �e����������
#define ENEMY_BULLET_TIME										(ENEMY_WAIT)

// �ʃo���b�g
#define TEXTURE_PATTERN_DIVIDE_X								(1)																				// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y								(1)																				// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM										(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)								// �A�j���[�V�����p�^�[����
#define ANIM_WAIT												(4)																				// �A�j���[�V�����̐؂�ւ��Wait�l

// �G�l�~�[�o���b�g
#define ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_X					(4)																				// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_Y					(1)																				// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ENEMY_BULLET_ANIM_PATTERN_NUM							(ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_X * ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ENEMY_BULLET_ANIM_WAIT									(4)																				// �A�j���[�V�����̐؂�ւ��Wait�l

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
static ID3D11ShaderResourceView	*g_Texture[ENEMY_BULLET_TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/enemy_bullet00.png",
	"data/TEXTURE/enemy_bullet01.png",
};

static BOOL		g_Load = FALSE;							// ���������s�������̃t���O
static ENEMYBULLETE	g_Bulletenemy[ENEMY_BULLETE_MAX];	// �o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < ENEMY_BULLET_TEXTURE_MAX; i++)
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
	for (int i = 0; i < ENEMY_BULLETE_MAX; i++)
	{
		g_Bulletenemy[i].use		 = FALSE;									// ���g�p�i���˂���Ă��Ȃ��e�j
		g_Bulletenemy[i].w			 = ENEMY_BULLET_TEXTURE_WIDTH;
		g_Bulletenemy[i].h			 = ENEMY_BULLET_TEXTURE_HEIGHT;
		g_Bulletenemy[i].pos		 = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bulletenemy[i].rot		 = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bulletenemy[i].texNo		 = 0;
		g_Bulletenemy[i].countAnim	 = 0;
		g_Bulletenemy[i].patternAnim = 0;
		g_Bulletenemy[i].move		 = XMFLOAT3(0.0f, 0.0f, 0.0f);				// �ړ��ʂ�������(�o���b�g�̔�ԕ���)
		g_Bulletenemy[i].hit		 = 0;										// �ŏ��̓v���C���[
		g_Bulletenemy[i].tex		 = 0;										// �e�N�X�`���[�̎���
		g_Bulletenemy[i].time		 = 0;										// �e�����˂���Ă���̎���

	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyBullet(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < ENEMY_BULLET_TEXTURE_MAX; i++)
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
void UpdateEnemyBullet(void)
{
	int bulletCount = 0;				// ���������o���b�g�̐�

	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();


	for (int i = 0; i < ENEMY_BULLETE_MAX; i++)
	{
		// �o���b�g�̃X�s�[�h�̏�����
		//(�����ŏ��������Ȃ��ƒe���i�X�����Ȃ��Ă��܂�����)
		g_Bulletenemy[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);

		if (g_Bulletenemy[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{									// Yes

			// �e�N�X�`���[�̎���
			if (g_Bulletenemy == 0)
			{
				// �A�j���[�V����  
				g_Bulletenemy[i].countAnim++;
				if ((g_Bulletenemy[i].countAnim % ANIM_WAIT) == 0)
				{
					// �p�^�[���̐؂�ւ�
					g_Bulletenemy[i].patternAnim = (g_Bulletenemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}

			}

			if (g_Bulletenemy[i].tex == 1)
			{
				// �A�j���[�V����  
				g_Bulletenemy[i].countAnim++;
				if ((g_Bulletenemy[i].countAnim % ENEMY_BULLET_ANIM_WAIT) == 0)
				{
					// �p�^�[���̐؂�ւ�
					g_Bulletenemy[i].patternAnim = (g_Bulletenemy[i].patternAnim + 1) % ENEMY_BULLET_ANIM_PATTERN_NUM;
				}

			}

			for(int homing = 0; homing <= 60; homing++)
			{
				// �o���b�g�̂������肵���z�[�~���O

				PLAYER* player = GetPlayer();

				//�v�Z����������ϊ����āA�Ō�Ɍ��ɖ߂��B
				XMVECTOR epos = XMLoadFloat3(&g_Bulletenemy[i].pos);

				// ����(x.y.z)���v�Z�ł���B�����ƃv���C���[�̍������߂Ă���
				XMVECTOR vec = XMLoadFloat3(&player[0].pos_old) - epos;

				// �p�x�����߂Ă���
				float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);

				// �X�s�[�h�����߂Ă���
				float speed = ENEMY_BULLET_SPEED;

				// �R�T�C����X�A�T�C����Y
				g_Bulletenemy[i].pos.x += cosf(angle) * speed;
				g_Bulletenemy[i].pos.y += sinf(angle) * speed;

				// ��]�����Ă���
				g_Bulletenemy[i].rot.z = angle / 4;

				// �o���b�g�̈ړ�����
				XMVECTOR pos = XMLoadFloat3(&g_Bulletenemy[i].pos);
				XMVECTOR move = XMLoadFloat3(&g_Bulletenemy[i].move);
				pos += move;
				XMStoreFloat3(&g_Bulletenemy[i].pos, pos);

			}




			// ��ʊO�܂Ői�񂾁H
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			if (g_Bulletenemy[i].pos.y < (-g_Bulletenemy[i].h/2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{	
				g_Bulletenemy[i].use = false;
			}	
			if (g_Bulletenemy[i].pos.y > (FIELD_HEIGHT + g_Bulletenemy[i].h/2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{	
				g_Bulletenemy[i].use = false;
			}


				// �U������
				// �v���C���[�̐��������蔻����s��
				for (int p = 0; p < ENEMY_MAX; p++)
				{
					// �����Ă�v���C���[�Ɠ����蔻�������
					if (player[p].use == TRUE)
					{
						// �����蔻��
						BOOL ans = CollisionBB(player[p].pos, player[p].w, player[p].h, g_Bulletenemy[i].pos, g_Bulletenemy[i].w, g_Bulletenemy[i].h);

						// �������Ă���H
						if (ans == TRUE)
						{
							// �v���C���[������Ԃł͂Ȃ����A���G���ԊO�Ȃ�_���[�W��^����
							if (player[p].defense == FALSE && (player[p].invincibleCnt >= PLAYER_INVINCIBLE_FRAME || player[p].invincibleCnt == 0))
							{

								// �����������̏���(�_���[�W��^����)
								SetDamage(10);

								// �v���C���[�ɓ���������TRUE�ɂ���
								player[p].receive = TRUE;

								// �o���b�g�𖢎g�p�ɂ���
								g_Bulletenemy[i].use = FALSE;

								// �G�t�F�N�g����
								SetEnemyEffect(player[p].pos.x, player[p].pos.y, 30, TEXTURE_EFFECT00);
							}

							// �v���C���[������Ԃ̎��ɓ���������FALSE�ɂ���
							if (player[p].defense == TRUE)
							{
								g_Bulletenemy[i].use = FALSE;
							}
						}
					}
				}

				// ������莞�ԉ߂�����FALSE����
				g_Bulletenemy[i].time++;

				if (g_Bulletenemy[i].time >= ENEMY_BULLET_TIME)
				{
					g_Bulletenemy[i].use = FALSE;

					g_Bulletenemy[i].time = 0;
				}

			bulletCount++;
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemyBullet(void)
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

	for (int i = 0; i < ENEMY_BULLETE_MAX; i++)
	{
		if (g_Bulletenemy[i].tex == 0)
		{

			// �O�ԂȂ�
			if (g_Bulletenemy[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
			{									// Yes
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy_bullet00]);

				//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Bulletenemy[i].pos.x - scrollX;	// �o���b�g�̕\���ʒuX
				float py = g_Bulletenemy[i].pos.y - scrollY;	// �o���b�g�̕\���ʒuY
				float pw = g_Bulletenemy[i].w;		// �o���b�g�̕\����
				float ph = g_Bulletenemy[i].h;		// �o���b�g�̕\������

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				float tx = (float)(g_Bulletenemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_Bulletenemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Bulletenemy[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

		}

		// �P�ԂȂ�
		if (g_Bulletenemy[i].tex == 1)
		{
			if (g_Bulletenemy[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
			{									// Yes
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy_bullet01]);

				//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Bulletenemy[i].pos.x - scrollX;	// �o���b�g�̕\���ʒuX
				float py = g_Bulletenemy[i].pos.y - scrollY;	// �o���b�g�̕\���ʒuY
				float pw = g_Bulletenemy[i].w;		// �o���b�g�̕\����
				float ph = g_Bulletenemy[i].h;		// �o���b�g�̕\������

				float tw = 1.0f / ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				float th = 1.0f / ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				float tx = (float)(g_Bulletenemy[i].patternAnim % ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_Bulletenemy[i].patternAnim / ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Bulletenemy[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

		}

	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMYBULLETE *GetEnemyBullet(void)
{
	return &g_Bulletenemy[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetEnemyBullet(XMFLOAT3 enemypos, BOOL tex)
{

	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < ENEMY_BULLETE_MAX; i++)
	{

		// ���g�p��Ԃ̃o���b�g��������
		if (g_Bulletenemy[i].use == FALSE)			
		{
			g_Bulletenemy[i].tex = tex;				// �e�N�X�`���[�̎���
			g_Bulletenemy[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_Bulletenemy[i].pos = enemypos;		// ���W���Z�b�g
			return;									// 1���Z�b�g�����̂ŏI������
		}
	}
}

