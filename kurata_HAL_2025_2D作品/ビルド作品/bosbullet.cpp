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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// �L�����T�C�Y(��)
#define TEXTURE_HEIGHT				(100)	// �L�����T�C�Y(��)
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

enum
{
	enemy_bullet00,
};

enum 
{
	enemy,
	boss,

};
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/enemy_bullet00.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BULLETE	g_Bulletbos[BOS_BULLETE_MAX];	// �o���b�g�\����

// �ǂ����Ŏg�����̃`�F�b�N�ϐ�
int g_check = 0;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBulletBos(void)
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
	for (int i = 0; i < BOS_BULLETE_MAX; i++)
	{
		g_Bulletbos[i].use   = FALSE;								// ���g�p�i���˂���Ă��Ȃ��e�j
		g_Bulletbos[i].w     = TEXTURE_WIDTH;
		g_Bulletbos[i].h     = TEXTURE_HEIGHT;
		g_Bulletbos[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bulletbos[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bulletbos[i].texNo = 0;
		g_Bulletbos[i].P0 = XMFLOAT3(0.0f, 0.0f, 0.0f);				// ����_P0
		g_Bulletbos[i].P1 = XMFLOAT3(1084.0f, 300.0f, 0.0f);		// ����_P0
		g_Bulletbos[i].s = 0.0f;

		g_Bulletbos[i].countAnim = 0;
		g_Bulletbos[i].patternAnim = 0;

		g_Bulletbos[i].move = XMFLOAT3(0.0f, -BULLETE_SPEED, 0.0f);	// �ړ��ʂ�������
		
		g_Bulletbos[i].hit = 0;										// �ŏ��̓v���C���[
		g_Bulletbos[i].tex = 0;										// �e�N�X�`���[�̎���
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBulletBos(void)
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
void UpdateBulletBos(void)
{
	int bulletCount = 0;				// ���������o���b�g�̐�

	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

	for (int i = 0; i < BOS_BULLETE_MAX; i++)
	{
		if (g_Bulletbos[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			// �A�j���[�V����  
			g_Bulletbos[i].countAnim++;
			if ((g_Bulletbos[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_Bulletbos[i].patternAnim = (g_Bulletbos[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// �x�W�F�Ȑ��֘A
			g_Bulletbos[i].s += 0.01f;
			if (g_Bulletbos[i].s > 1.0f)
			{
				g_Bulletbos[i].use = FALSE;
				g_Bulletbos[i].s = 0.0f;
			}

			// �o���b�g�̈ړ�����
			XMVECTOR pos  = XMLoadFloat3(&g_Bulletbos[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_Bulletbos[i].move);
			pos += move;
			XMStoreFloat3(&g_Bulletbos[i].pos, pos);

			// ��ʊO�܂Ői�񂾁H
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			if (g_Bulletbos[i].pos.y < (-g_Bulletbos[i].h/2))				// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Bulletbos[i].use = false;
			}
			if (g_Bulletbos[i].pos.y > (FIELD_HEIGHT + g_Bulletbos[i].h/2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Bulletbos[i].use = false;
			}


			XMVECTOR vold_pos = XMLoadFloat3(&g_Bulletbos[i].pos);

			XMFLOAT3 vp = XMFLOAT3(0.0f, 604.0f, 0.0f);

			// �{�X�p�ƃG�l�~�[�p�ŕ����Ă���
			if (enemy)
			{
				// �x�W�F�Ȑ�
				// �o���b�g�̈ړ�����(�|�W�V����)������������Ƃ����
				// ���W�v�Z
				XMVECTOR vP0 = XMLoadFloat3(&enemy[2].pos);
				XMVECTOR vP1 = XMLoadFloat3(&enemy[0].pos);
				XMVECTOR vP2 = XMLoadFloat3(&vp);

				//XMFLOAT3 migishita = XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
				//XMVECTOR vP2 = XMLoadFloat3(&migishita);

				float s = g_Bulletbos[i].s;

				XMVECTOR vAns = QuadraticBezierCurve(vP0, vP1, vP2, s);

				XMStoreFloat3(&g_Bulletbos[i].pos, vAns);

				// �e�̉�]
				XMVECTOR vec = vAns - vold_pos;		// �ړ���ƈړ��O�̍��������߂Ă���
				float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);

				// ��]�����Ă���
				g_Bulletbos[i].rot.z = angle + (3.14 * 2) / 4;


			}

			if (boss)
			{
				// �x�W�F�Ȑ�
				// �o���b�g�̈ړ�����(�|�W�V����)������������Ƃ����
				// ���W�v�Z
				XMVECTOR vP0 = XMLoadFloat3(&boss[0].boss_pos);
				XMVECTOR vP1 = XMLoadFloat3(&player[0].pos);
				XMVECTOR vP2 = XMLoadFloat3(&vp);


				float s = g_Bulletbos[i].s;

				XMVECTOR vAns = QuadraticBezierCurve(vP0, vP1, vP2, s);

				XMStoreFloat3(&g_Bulletbos[i].pos, vAns);

				// �e�̉�]
				XMVECTOR vec = vAns - vold_pos;		// �ړ���ƈړ��O�̍��������߂Ă���
				float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);

				// ��]�����Ă���
				g_Bulletbos[i].rot.z = angle + (3.14 * 2) / 4;


			}

			



			// �����蔻�菈��
			// �v���C���[�̐��������蔻����s��
			for (int p = 0; p < ENEMY_MAX; p++)
			{
				// �����Ă�G�l�~�[�Ɠ����蔻�������
				if (player[p].use == TRUE)
				{
					BOOL ans = CollisionBB(player[p].pos, player[p].w, player[p].h, g_Bulletbos[i].pos, g_Bulletbos[i].w, g_Bulletbos[i].h);

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

								g_Bulletbos[i].use = FALSE;

								// �G�t�F�N�g����
								SetEnemyEffect(player[p].pos.x, player[p].pos.y, 30, TEXTURE_EFFECT00);
							}
						}

						// �v���C���[������ԂȂ�AFALSE�ɂ���
						if (player[p].defense == TRUE)
						{
							g_Bulletbos[i].use = FALSE;
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
void DrawBulletBos(void)
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
	for (int i = 0; i < BOS_BULLETE_MAX; i++)
	{
		if (g_Bulletbos[i].tex == 0)
		{
			if (g_Bulletbos[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
			{									// Yes
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy_bullet00]);

				//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Bulletbos[i].pos.x - scrollX;	// �o���b�g�̕\���ʒuX
				float py = g_Bulletbos[i].pos.y - scrollY;	// �o���b�g�̕\���ʒuY
				float pw = g_Bulletbos[i].w;		// �o���b�g�̕\����
				float ph = g_Bulletbos[i].h;		// �o���b�g�̕\������

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				float tx = (float)(g_Bulletbos[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_Bulletbos[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Bulletbos[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

		}

	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLETE *GetBossBullet(void)
{
	return &g_Bulletbos[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetBulletBos(int check, BOOL tex)
{

	ENEMY* enemy = GetEnemy();

	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BOS_BULLETE_MAX; i++)
	{
		if (g_Bulletbos[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Bulletbos[i].tex = tex;			// �o���b�g�̎���
			g_Bulletbos[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			//g_Bulletbos[i].pos = bosspos;		// ���W���Z�b�g
			g_check = check;

			return;								// 1���Z�b�g�����̂ŏI������

		}
	}
}

//=============================================================================
// �x�W�F�Ȑ��֘A
//=============================================================================

XMVECTOR LinearInterpolation(XMVECTOR P0, XMVECTOR P1, float s)
{
	XMVECTOR A0;
	A0 = P0 + (P1 - P0) * s;
	return A0;
}



XMVECTOR QuadraticBezierCurve(XMVECTOR p0, XMVECTOR p1, XMVECTOR p2, float s)
{
	XMVECTOR a0 = LinearInterpolation(p0, p1, s);
	XMVECTOR a1 = LinearInterpolation(p1, p2, s);
	XMVECTOR b0 = LinearInterpolation(a0, a1, s);

	return b0;
}

