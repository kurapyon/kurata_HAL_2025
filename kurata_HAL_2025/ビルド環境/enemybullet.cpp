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
#include "enemybullet.h"
#include "enemy.h"
#include "fade.h"
#include "debugproc.h"
#include "camera.h"
#include "light.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	ENEMYBULLET_WIDTH		(10.0f)			// ���_�T�C�Y
#define	ENEMYBULLET_HEIGHT		(10.0f)			// ���_�T�C�Y

#define	ENEMYBULLET_SPEED		(10.0f)			// �e�̈ړ��X�s�[�h


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexEnemyBullet(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static ENEMYBULLET					g_EnemyBullet[MAX_ENEMYBULLET];	// �e���[�N
static int							g_TexNo;				// �e�N�X�`���ԍ�
static BOOL							g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static char *g_TextureName[] =
{
	"data/TEXTURE/f00.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyBullet(void)
{
	MakeVertexEnemyBullet();

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
	for (int nCntBullet = 0; nCntBullet < MAX_ENEMYBULLET; nCntBullet++)
	{
		ZeroMemory(&g_EnemyBullet[nCntBullet].material, sizeof(g_EnemyBullet[nCntBullet].material));
		g_EnemyBullet[nCntBullet].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_EnemyBullet[nCntBullet].pos  = { 0.0f, 0.0f, 0.0f };
		g_EnemyBullet[nCntBullet].rot  = { 0.0f, 0.0f, 0.0f };
		g_EnemyBullet[nCntBullet].move = { 0.0f, 0.0f, 0.0f };
		g_EnemyBullet[nCntBullet].scl  = { 1.0f, 1.0f, 1.0f };
		g_EnemyBullet[nCntBullet].spd  = { ENEMYBULLET_SPEED, ENEMYBULLET_SPEED, ENEMYBULLET_SPEED };
		g_EnemyBullet[nCntBullet].fWidth = ENEMYBULLET_WIDTH;
		g_EnemyBullet[nCntBullet].fHeight = ENEMYBULLET_HEIGHT;
		g_EnemyBullet[nCntBullet].use = FALSE;

	}


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyBullet(void)
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
void UpdateEnemyBullet(void)
{

	for (int i = 0; i < MAX_ENEMYBULLET; i++)
	{
		if (g_EnemyBullet[i].use)
		{
			// �e�̈ړ�����
			g_EnemyBullet[i].pos.x -= sinf(g_EnemyBullet[i].rot.y) * g_EnemyBullet[i].spd.x;
			//g_EnemyAtc[i].pos.y -= sinf(g_EnemyAtc[i].rot.x) * g_EnemyAtc[i].spd.y;
			g_EnemyBullet[i].pos.z -= cosf(g_EnemyBullet[i].rot.y) * g_EnemyBullet[i].spd.z;

			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_EnemyBullet[i].shadowIdx, XMFLOAT3(g_EnemyBullet[i].pos.x, 0.1f, g_EnemyBullet[i].pos.z));


			// �t�B�[���h�̊O�ɏo����e����������
			if (g_EnemyBullet[i].pos.x < MAP_LEFT
				|| g_EnemyBullet[i].pos.x > MAP_RIGHT
				|| g_EnemyBullet[i].pos.z < MAP_DOWN
				|| g_EnemyBullet[i].pos.z > MAP_TOP)
			{
				g_EnemyBullet[i].use = FALSE;
				ReleaseShadow(g_EnemyBullet[i].shadowIdx);
			}

		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemyBullet(void)
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

	for (int i = 0; i < MAX_ENEMYBULLET; i++)
	{
		if (g_EnemyBullet[i].use)
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
			mtxScl = XMMatrixScaling(g_EnemyBullet[i].scl.x * 2.0f, g_EnemyBullet[i].scl.y * 2.0f, g_EnemyBullet[i].scl.z * 2.0f);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_EnemyBullet[i].pos.x, g_EnemyBullet[i].pos.y, g_EnemyBullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_EnemyBullet[i].material);

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
HRESULT MakeVertexEnemyBullet(void)
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

	float fWidth = ENEMYBULLET_WIDTH;
	float fHeight = ENEMYBULLET_HEIGHT;

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
int SetEnemyBullet(XMFLOAT3 pos, XMFLOAT3 rot)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_ENEMYBULLET; nCntBullet++)
	{
		if (!g_EnemyBullet[nCntBullet].use)
		{
			g_EnemyBullet[nCntBullet].pos = pos;
			g_EnemyBullet[nCntBullet].rot = rot;
			g_EnemyBullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_EnemyBullet[nCntBullet].use = TRUE;

			// �e�̐ݒ�
			g_EnemyBullet[nCntBullet].shadowIdx = CreateShadow(g_EnemyBullet[nCntBullet].pos, 0.5f, 0.5f);

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
ENEMYBULLET *GetEnemyBullet(void)
{
	return &(g_EnemyBullet[0]);
}

