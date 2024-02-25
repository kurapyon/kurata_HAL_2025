//=============================================================================
//
// �G�l�~�[ATC���� [enemyatc.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "model.h"
#include "renderer.h"
#include "shadow.h"
#include "collision.h"
#include "enemy.h"
#include "fade.h"
#include "debugproc.h"
#include "enemyatc.h"
#include "particleatc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	ENEMYATC_WIDTH		(10.0f)			// ���_�T�C�Y
#define	ENEMYATC_HEIGHT		(10.0f)			// ���_�T�C�Y
#define	ENEMYATC_SPEED		(5.0f)			// �e�̈ړ��X�s�[�h

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexEnemyAtc(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static ENEMYATC						g_EnemyAtc[ENEMYATC_MAX_BULLET];	// �e���[�N
static int							g_TexNo;				// �e�N�X�`���ԍ�

//static char *g_TextureName[] =
//{
//};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyAtc(void)
{
	MakeVertexEnemyAtc();

	//// �e�N�X�`������
	//for (int i = 0; i < TEXTURE_MAX; i++)
	//{
	//	g_Texture[i] = NULL;
	//	D3DX11CreateShaderResourceViewFromFile(GetDevice(),
	//		g_TextureName[i],
	//		NULL,
	//		NULL,
	//		&g_Texture[i],
	//		NULL);
	//}

	g_TexNo = 0;

	// �e���[�N�̏�����
	for (int nCntBullet = 0; nCntBullet < ENEMYATC_MAX_BULLET; nCntBullet++)
	{
		ZeroMemory(&g_EnemyAtc[nCntBullet].material, sizeof(g_EnemyAtc[nCntBullet].material));
		g_EnemyAtc[nCntBullet].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_EnemyAtc[nCntBullet].pos = { 0.0f, 0.0f, 0.0f };
		g_EnemyAtc[nCntBullet].rot = { 0.0f, 0.0f, 0.0f };
		g_EnemyAtc[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
		g_EnemyAtc[nCntBullet].spd = { ENEMYATC_SPEED, ENEMYATC_SPEED, ENEMYATC_SPEED };
		g_EnemyAtc[nCntBullet].fWidth = ENEMYATC_WIDTH;
		g_EnemyAtc[nCntBullet].fHeight = ENEMYATC_HEIGHT;
		g_EnemyAtc[nCntBullet].blife = ENEMYATC_LIFE;
		g_EnemyAtc[nCntBullet].use = FALSE;

	}


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyAtc(void)
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
void UpdateEnemyAtc(void)
{
	ENEMY* enemy = GetEnemy();
	


	for (int i = 0; i < ENEMYATC_MAX_BULLET; i++)
	{
		if (g_EnemyAtc[i].use)
		{
			// �e�̈ړ�����
			g_EnemyAtc[i].pos.x -= sinf(g_EnemyAtc[i].rot.y) * g_EnemyAtc[i].spd.x;
			//g_EnemyAtc[i].pos.y -= sinf(g_EnemyAtc[i].rot.x) * g_EnemyAtc[i].spd.y;
			g_EnemyAtc[i].pos.z -= cosf(g_EnemyAtc[i].rot.y) * g_EnemyAtc[i].spd.z;

			// �p�[�e�B�N����t����
			XMFLOAT4 col  = { 1.0f, 0.0f, 1.0f, 1.0f };
			XMFLOAT3 move = { 0.0f, 0.0f, 0.0f};
			XMFLOAT3 rot  = { 0.0f, 0.0f, 0.0f };

			SetParticleAtc(g_EnemyAtc[i].pos,
				move,
				rot,
				col,
				g_EnemyAtc[i].fWidth,
				g_EnemyAtc[i].fHeight,
				PARTICLEATC_LIFE);

			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_EnemyAtc[i].shadowIdx, XMFLOAT3(g_EnemyAtc[i].pos.x, 0.1f, g_EnemyAtc[i].pos.z));

			// �t�B�[���h�̊O�ɏo��܂��́A���C�t���Ԃ�0�ɂȂ�����e����������
			g_EnemyAtc[i].blife--;
			if (g_EnemyAtc[i].pos.x < MAP_LEFT
				|| g_EnemyAtc[i].pos.x > MAP_RIGHT
				|| g_EnemyAtc[i].pos.y < MAP_DOWN
				|| g_EnemyAtc[i].pos.z < MAP_DOWN
				|| g_EnemyAtc[i].pos.z > MAP_TOP
				|| g_EnemyAtc[i].blife <= 0)
			{
				g_EnemyAtc[i].use = FALSE;
				g_EnemyAtc[i].blife = ENEMYATC_LIFE;
				ReleaseShadow(g_EnemyAtc[i].shadowIdx);
			}

#ifdef _DEBUG
			// �f�o�b�O�\��
			PrintDebugProc("Bullet rotX: %f\n", g_EnemyAtc[i].rot.x);
#endif
		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemyAtc(void)
{
	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < ENEMYATC_MAX_BULLET; i++)
	{
		if (g_EnemyAtc[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_EnemyAtc[i].scl.x, g_EnemyAtc[i].scl.y, g_EnemyAtc[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_EnemyAtc[i].rot.x, g_EnemyAtc[i].rot.y + XM_PI, g_EnemyAtc[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_EnemyAtc[i].pos.x, g_EnemyAtc[i].pos.y, g_EnemyAtc[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_EnemyAtc[i].mtxWorld, mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_EnemyAtc[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexEnemyAtc(void)
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

	float fWidth = ENEMYATC_WIDTH;
	float fHeight = ENEMYATC_HEIGHT;

	// ���_���W�̐ݒ�
	vertex[0].Position = { -fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[1].Position = {  fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[2].Position = { -fWidth / 2.0f, 0.0f, -fHeight / 2.0f };
	vertex[3].Position = {  fWidth / 2.0f, 0.0f, -fHeight / 2.0f };

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

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
int SetEnemyAtc(XMFLOAT3 pos, XMFLOAT3 rot)
{

	ENEMY* enemy = GetEnemy();

	int nIdxBullet = -1;


	for (int nCntBullet = 0; nCntBullet < ENEMYATC_MAX_BULLET; nCntBullet++)
	{
		if (!g_EnemyAtc[nCntBullet].use)
		{
			g_EnemyAtc[nCntBullet].pos = pos;
			g_EnemyAtc[nCntBullet].rot = rot;
			g_EnemyAtc[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_EnemyAtc[nCntBullet].use = TRUE;

			// �e�̐ݒ�
			g_EnemyAtc[nCntBullet].shadowIdx = CreateShadow(g_EnemyAtc[nCntBullet].pos, 0.5f, 0.5f);


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
ENEMYATC *GetEnemyAtc(void)
{
	return &(g_EnemyAtc[0]);
}

