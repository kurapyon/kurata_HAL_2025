//=============================================================================
//
// �e���ˏ��� [bullet.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "model.h"
#include "renderer.h"
#include "shadow.h"
#include "collision.h"
#include "bossbullet.h"
#include "enemy.h"
#include "fade.h"
#include "debugproc.h"
#include "camera.h"
#include "light.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	BULLET_WIDTH		(10.0f)			// ���_�T�C�Y
#define	BULLET_HEIGHT		(10.0f)			// ���_�T�C�Y

#define	BULLET_SPEED		(5.0f)			// �e�̈ړ��X�s�[�h
#define BOSSBULLET_SCL		(5.0f)			// �e�̃X�P�[��
#define BOSSBULLET_HP		(60.0f)		// �e�̏o������

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexBossBullet(void);
// �x�W�F�Ȑ�
XMVECTOR LinearInterpolation(XMVECTOR P0, XMVECTOR P1, float s);
XMVECTOR QuadraticBezierCurve(XMVECTOR p0, XMVECTOR p1, XMVECTOR p2, float s);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static BOSSBULLET						g_BossBullet[MAX_BOSSBULLET];	// �e���[�N
static int							g_TexNo;				// �e�N�X�`���ԍ�
static BOOL							g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static char *g_TextureName[] =
{
	"data/TEXTURE/blt.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBossBullet(void)
{
	MakeVertexBossBullet();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// �e���[�N�̏�����
	for (int nCntBullet = 0; nCntBullet < MAX_BOSSBULLET; nCntBullet++)
	{
		ZeroMemory(&g_BossBullet[nCntBullet].material, sizeof(g_BossBullet[nCntBullet].material));
		g_BossBullet[nCntBullet].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_BossBullet[nCntBullet].pos  = { 0.0f, 0.0f, 0.0f };
		g_BossBullet[nCntBullet].rot  = { 0.0f, 0.0f, 0.0f };
		g_BossBullet[nCntBullet].move = { 0.0f, 0.0f, 0.0f };
		g_BossBullet[nCntBullet].scl  = { 1.0f, 1.0f, 1.0f };
		g_BossBullet[nCntBullet].spd  = BULLET_SPEED;
		g_BossBullet[nCntBullet].hp   = BOSSBULLET_HP;
		g_BossBullet[nCntBullet].fWidth = BULLET_WIDTH;
		g_BossBullet[nCntBullet].fHeight = BULLET_HEIGHT;
		g_BossBullet[nCntBullet].use = FALSE;

	}


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBossBullet(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBossBullet(void)
{
	ENEMY* enemy = GetEnemy();
	//PLAYER* player = GetPlayer();


	for (int i = 0; i < MAX_BOSSBULLET; i++)
	{
		if (g_BossBullet[i].use)
		{
			// �e�̈ړ����� �x�W�F�Ȑ�
			g_BossBullet[i].s += 0.005f;
			g_BossBullet[i].hp++;

			if (g_BossBullet[i].s > 1.0f)
			{
				g_BossBullet[i].use = FALSE;
				g_BossBullet[i].s = 0.0f;
				ReleaseShadow(g_BossBullet[i].shadowIdx);
			}


			XMVECTOR vP0 = XMLoadFloat3(&g_BossBullet[i].p0);
			XMVECTOR vP1 = XMLoadFloat3(&g_BossBullet[i].p1);
			XMVECTOR vP2 = XMLoadFloat3(&g_BossBullet[i].p2t);


			float s = g_BossBullet[i].s * g_BossBullet[i].s;
			XMVECTOR vAns = QuadraticBezierCurve(vP0, vP1, vP2, s);

			XMStoreFloat3(&g_BossBullet[i].pos, vAns);
			XMVECTOR vold_pos = XMLoadFloat3(&g_BossBullet[i].pos);

			XMVECTOR vec = vAns - vold_pos;        //�ړ��O�Ƃ��Ƃ̍��W�̍����i�p�x�j
			float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);        //���̍������g���Ċp�x�����߂Ă���

			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_BossBullet[i].shadowIdx, XMFLOAT3(g_BossBullet[i].pos.x, 0.1f, g_BossBullet[i].pos.z));

			// ���Ԍo�߂ŏ���
			if (g_BossBullet[i].hp >= BOSSBULLET_HP)
			{
				g_BossBullet[i].use = FALSE;
				g_BossBullet[i].hp = 0.0f;
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBossBullet(void)
{
	// ���e�X�g�ݒ�
	if (g_bAlpaTest == TRUE)
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

	for (int i = 0; i < MAX_BOSSBULLET; i++)
	{
		if (g_BossBullet[i].use)
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
			mtxScl = XMMatrixScaling(g_BossBullet[i].scl.x * BOSSBULLET_SCL, g_BossBullet[i].scl.y * BOSSBULLET_SCL, g_BossBullet[i].scl.z * BOSSBULLET_SCL);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_BossBullet[i].pos.x, g_BossBullet[i].pos.y, g_BossBullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_BossBullet[i].material);

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
HRESULT MakeVertexBossBullet(void)
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

	float fWidth = BULLET_WIDTH;
	float fHeight = BULLET_HEIGHT;

	// ���_���W�̐ݒ�
	vertex[0].Position = { -fWidth / 2.0f,  fHeight / 2.0f , 0.0f};
	vertex[1].Position = {  fWidth / 2.0f,  fHeight / 2.0f , 0.0f};
	vertex[2].Position = { -fWidth / 2.0f, -fHeight / 2.0f , 0.0f};
	vertex[3].Position = {  fWidth / 2.0f, -fHeight / 2.0f , 0.0f};

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
// �e�̃p�����[�^���Z�b�g
//=============================================================================
int SetBossBullet(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 tpos)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BOSSBULLET; nCntBullet++)
	{
		if (!g_BossBullet[nCntBullet].use)
		{
			g_BossBullet[nCntBullet].pos = pos;
			g_BossBullet[nCntBullet].p0 = pos;
			g_BossBullet[nCntBullet].p1 = pos;

			g_BossBullet[nCntBullet].p1.x = 200.0f;
			g_BossBullet[nCntBullet].p1.y = 200.0f;
			g_BossBullet[nCntBullet].p1.z = 200.0f;

			g_BossBullet[nCntBullet].p2t = tpos;

			g_BossBullet[nCntBullet].rot = rot;
			g_BossBullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_BossBullet[nCntBullet].use = TRUE;

			// �e�̐ݒ�
			g_BossBullet[nCntBullet].shadowIdx = CreateShadow(g_BossBullet[nCntBullet].pos, 0.5f, 0.5f);

			nIdxBullet = nCntBullet;

			//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

//=============================================================================
// �e�̎擾
//=============================================================================
BOSSBULLET * GetBossBullet(void)
{
	return &(g_BossBullet[0]);
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
