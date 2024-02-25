//=============================================================================
//
// �o���b�g���� [bullet.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "model.h"
#include "renderer.h"
#include "shadow.h"
#include "collision.h"
#include "enemybossbullet .h"
#include "fade.h"
#include "debugproc.h"
#include "camera.h"
#include "light.h"
#include "player.h"
#include "enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(100/2)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�
#define	ENEMYBULLETBULLET_WIDTH		(10.0f)			// ���_�T�C�Y
#define	ENEMYBULLETBULLET_HEIGHT	(10.0f)			// ���_�T�C�Y

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l
#define ENEMYBOSSBULLET_SCL			(5.0f)	// �T�C�Y
#define ENEMYBOSSBULLET_HP			(240.0f)// ���C�t
#define ENEMYBOSSBULLET_START		(1.0f)// �e�̃X�^�[�g�ʒu

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexEnemyBossBullet(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static float					g_fWidthBase = 5.0f;			// ��̕�
static float					g_fHeightBase = 10.0f;			// ��̍���

static char *g_TexturName[] = {
	"data/TEXTURE/blt.png",
};

static BOOL				g_Load = FALSE;		// ���������s�������̃t���O
static ENEMYBOSSBULLET	g_EnemyBossBullet[ENEMYBOSSBULLET_MAX];	// �o���b�g�\����
static BOOL				g_alpaTest;		// �A���t�@�e�X�gON/OFF


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyBossBullet(void)
{
	ID3D11Device *pDevice = GetDevice();
	MakeVertexEnemyBossBullet();
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
	for (int i = 0; i < ENEMYBOSSBULLET_MAX; i++)
	{
		g_EnemyBossBullet[i].use   = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
		g_EnemyBossBullet[i].w     = TEXTURE_WIDTH;
		g_EnemyBossBullet[i].h     = TEXTURE_HEIGHT;
		g_EnemyBossBullet[i].pos   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyBossBullet[i].localpos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyBossBullet[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyBossBullet[i].texNo = 0;

		g_EnemyBossBullet[i].countAnim = 0;
		g_EnemyBossBullet[i].patternAnim = 0;

		g_EnemyBossBullet[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);	// �ړ��ʂ�������
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyBossBullet(void)
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
void UpdateEnemyBossBullet(void)
{

	ENEMYBOSS* enemyboss = GetEnemyBoss();
	XMFLOAT3 enemybosspos = enemyboss->pos;

	int bulletCount = 0;				// ���������o���b�g�̐�

	for (int i = 0; i < ENEMYBOSSBULLET_MAX; i++)
	{
		if (g_EnemyBossBullet[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{										// Yes
				if (g_EnemyBossBullet[i].use)
				{
					// �e�̈ړ�����
					g_EnemyBossBullet[i].localpos.x += g_EnemyBossBullet[i].move.x;
					g_EnemyBossBullet[i].localpos.y += g_EnemyBossBullet[i].move.y;
					g_EnemyBossBullet[i].localpos.z += g_EnemyBossBullet[i].move.z;

					g_EnemyBossBullet[i].pos.x = g_EnemyBossBullet[i].localpos.x + enemybosspos.x;
					g_EnemyBossBullet[i].pos.y = g_EnemyBossBullet[i].localpos.y + enemybosspos.y;
					g_EnemyBossBullet[i].pos.z = g_EnemyBossBullet[i].localpos.z + enemybosspos.z;


					// �e�̈ʒu�ݒ�
					SetPositionShadow(g_EnemyBossBullet[i].shadowIdx, XMFLOAT3(g_EnemyBossBullet[i].pos.x, 0.1f, g_EnemyBossBullet[i].pos.z));

					// ���Ԍo�߂ŏ���
					if (g_EnemyBossBullet[i].hp >= ENEMYBOSSBULLET_HP)
					{
						g_EnemyBossBullet[i].use = FALSE;
						g_EnemyBossBullet[i].hp = 0.0f;
						ReleaseShadow(g_EnemyBossBullet[i].shadowIdx);

					}
				}
			}

			// ��ʊO�܂Ői�񂾁H
			if (g_EnemyBossBullet[i].pos.x < (SCREEN_WIDTH -g_EnemyBossBullet[i].h/2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_EnemyBossBullet[i].use = false;
			}
			if (g_EnemyBossBullet[i].pos.y > (SCREEN_HEIGHT+ g_EnemyBossBullet[i].h/2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_EnemyBossBullet[i].use = false;
			}

			bulletCount++;
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemyBossBullet(void)
{
	// ���e�X�g�ݒ�
	if (g_alpaTest == TRUE)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);
	}

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r����
	SetDepthEnable(FALSE);

	// �t�H�O����
	SetFogEnable(FALSE);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	for (int i = 0; i < ENEMYBOSSBULLET_MAX; i++)
	{
		if (g_EnemyBossBullet[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			// ������������������
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_EnemyBossBullet[i].scl.x * ENEMYBOSSBULLET_SCL, g_EnemyBossBullet[i].scl.y * ENEMYBOSSBULLET_SCL, g_EnemyBossBullet[i].scl.z * ENEMYBOSSBULLET_SCL);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_EnemyBossBullet[i].pos.x, g_EnemyBossBullet[i].pos.y, g_EnemyBossBullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_EnemyBossBullet[i].material);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(TRUE);

	// �t�H�O�L��
	SetFogEnable(GetFogEnable());

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexEnemyBossBullet(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = ENEMYBULLETBULLET_WIDTH;
	float fHeight = ENEMYBULLETBULLET_HEIGHT;

	// ���_���W�̐ݒ�
	vertex[0].Position = { -fWidth / 2.0f,  fHeight / 2.0f , 0.0f };
	vertex[1].Position = { fWidth / 2.0f,  fHeight / 2.0f , 0.0f };
	vertex[2].Position = { -fWidth / 2.0f, -fHeight / 2.0f , 0.0f };
	vertex[3].Position = { fWidth / 2.0f, -fHeight / 2.0f , 0.0f };

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMYBOSSBULLET* GetEnemyBossBullet(void)
{
	return &g_EnemyBossBullet[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetEneyBossBullet(XMFLOAT3 pos, XMFLOAT3 move, float hp, float spd)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < ENEMYBOSSBULLET_MAX; i++)
	{
		if (g_EnemyBossBullet[i].use == FALSE)	// ���g�p��Ԃ̃o���b�g��������
		{
			g_EnemyBossBullet[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_EnemyBossBullet[i].localpos = pos;		// ���W���Z�b�g
			g_EnemyBossBullet[i].move = move;			// �ړ��ʂ��Z�b�g
			g_EnemyBossBullet[i].hp = hp;
			g_EnemyBossBullet[i].spd = spd;
			return;								// 1���Z�b�g�����̂ŏI������
		}
	}
}

//=============================================================================
// �e�̃p�����[�^���Z�b�g
//=============================================================================
int SetBossAttackBullet(void)
{
	ENEMYBOSS* enemyboss = GetEnemyBoss();
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < ENEMYBOSSBULLET_MAX; nCntBullet++)
	{
		if (!g_EnemyBossBullet[nCntBullet].use)
		{

			XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);		// �p�[�e�B�N���̉�](������)
			XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// �p�[�e�B�N���̉�](������)
			XMFLOAT3		move;
			float			fAngleX, fAngleY, fAngleZ, fLength;
			float			nLife;
			float			fSize;


			XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �A�t�B���ϊ�
			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(0, 0, 1);//forward vector
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ��]�𔽉f
			fAngleX = (float)(rand() % 628 - 314) / g_fHeightBase;
			fAngleY = (float)(rand() % 628 - 314) / g_fHeightBase;
			fAngleZ = (float)(rand() % 628 - 314) / g_fHeightBase;

			mtxRot = XMMatrixRotationRollPitchYaw(fAngleX, fAngleY, fAngleZ);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
			XMFLOAT4X4 result;
			XMStoreFloat4x4(&result, mtxWorld);

			// �x�N�g��
			XMFLOAT3 vector;
			vector.x = result._41;
			vector.y = result._42;
			vector.z = result._43;

			move = XMFLOAT3(0, 0, 0);
			move.x = vector.x * 1.0f;
			move.y = vector.y * 1.0f;
			move.z = vector.z * 1.0f;

			// ���W
			pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			pos.x += vector.x * ENEMYBOSSBULLET_START;
			pos.y += vector.y * ENEMYBOSSBULLET_START;
			pos.z += vector.z * ENEMYBOSSBULLET_START;


			// �p�[�e�B�N������
			static float	g_fWidthBase  = ENEMYBULLETBULLET_WIDTH;			// ��̕�
			static float	g_fHeightBase = ENEMYBULLETBULLET_HEIGHT;			// ��̍���

			fLength = rand() % (int)(g_fWidthBase * 200) / g_fHeightBase - g_fWidthBase;

			// �v���C���[�ƃp�[�e�B�N���̍��������߂Ă���
			XMVECTOR vpos = XMLoadFloat3(&pos);
			XMVECTOR vec = XMLoadFloat3(&enemyboss->pos) - vpos;

			// ���K���i�傫�����O�`�P�ɂ���j
			vec = XMVector3Normalize(vec);

			//// �X�s�[�h�����߂Ă���
			float speed = ENEMYBOSSBULLET_SPEED;		// ���K�������Ƃ�

			// �e�̃��C�t
			nLife = ENEMYBOSSBULLET_HP;

			// fSize �� ����Œe�̊�{�T�C�Y��ݒ肷��
			fSize = ENEMYBOSSBULLET_SCL;

			SetEneyBossBullet(pos,move, nLife, speed);

			break;
		}
	}

	return nIdxBullet;
}