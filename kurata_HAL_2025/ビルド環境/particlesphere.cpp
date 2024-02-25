//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "light.h"
#include "particlesphere.h"
#include "player.h"
#include "enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�

#define	PARTICLE_SIZE_X		(40.0f)		// ���_�T�C�Y
#define	PARTICLE_SIZE_Y		(40.0f)		// ���_�T�C�Y
#define	VALUE_MOVE_PARTICLE	(5.0f)		// �ړ����x

#define	MAX_PARTICLESPHERE	(512)		// �p�[�e�B�N���ő吔

#define	DISP_SHADOW						// �e�̕\��
//#undef DISP_SHADOW

#define PARTICLE_START		(100.0f)	// �p�[�e�B�N���̃X�^�[�g�ʒu
#define PARTICLE_SPEED		(10.0f)		// �p�[�e�B�N���̃X�s�[�h
#define PARTICLE_SAIZE		(1.0f)		// �T�C�Y


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		localpos;		// ���[�J�����W
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scale;			// �X�P�[��
	XMFLOAT3		move;			// �ړ���
	MATERIAL		material;		// �}�e���A��
	float			fSizeX;			// ��
	float			fSizeY;			// ����
	int				nIdxShadow;		// �eID
	int				nLife;			// ����
	BOOL			use;			// �g�p���Ă��邩�ǂ���

} PARTICLESPHERE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticleSphere(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�

static PARTICLESPHERE			g_ParticleSphere[MAX_PARTICLESPHERE];		// �p�[�e�B�N�����[�N
static XMFLOAT3					g_posBase;						// �r���{�[�h�����ʒu
static float					g_fWidthBase = 5.0f;			// ��̕�
static float					g_fHeightBase = 10.0f;			// ��̍���
static float					g_roty = 0.0f;					// �ړ�����
static float					g_spd = 0.0f;					// �ړ��X�s�[�h

XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);					// �p�[�e�B�N���̍��W(������)
XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);					// �p�[�e�B�N���̉�](������)

static char *g_TextureName[] =
{
	"data/TEXTURE/effect.jpg",
};

static BOOL							g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticleSphere(void)
{
	// ���_���̍쐬
	MakeVertexParticleSphere();

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

	// �p�[�e�B�N�����[�N�̏�����
	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLESPHERE; nCntParticle++)
	{
		g_ParticleSphere[nCntParticle].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleSphere[nCntParticle].localpos = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_ParticleSphere[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleSphere[nCntParticle].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_ParticleSphere[nCntParticle].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_ParticleSphere[nCntParticle].material, sizeof(g_ParticleSphere[nCntParticle].material));
		g_ParticleSphere[nCntParticle].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_ParticleSphere[nCntParticle].fSizeX = PARTICLE_SIZE_X;
		g_ParticleSphere[nCntParticle].fSizeY = PARTICLE_SIZE_Y;
		g_ParticleSphere[nCntParticle].nIdxShadow = -1;
		g_ParticleSphere[nCntParticle].nLife = 0;
		g_ParticleSphere[nCntParticle].use = FALSE;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitParticleSphere(void)
{
	if (g_Load == FALSE) return;

	//�e�N�X�`���̉��
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateParticleSphere(void)
{
	ENEMY* enemy = GetEnemy();
	XMFLOAT3 g_enemypos = enemy[0].pos;

	{
		for(int nCntParticle = 0; nCntParticle < MAX_PARTICLESPHERE; nCntParticle++)
		{
			if(g_ParticleSphere[nCntParticle].use)
			{// ���[�J�����W��move�𑫂�

				g_ParticleSphere[nCntParticle].localpos.x += g_ParticleSphere[nCntParticle].move.x;
				g_ParticleSphere[nCntParticle].localpos.y += g_ParticleSphere[nCntParticle].move.y;
				g_ParticleSphere[nCntParticle].localpos.z += g_ParticleSphere[nCntParticle].move.z;

				g_ParticleSphere[nCntParticle].pos.x = g_ParticleSphere[nCntParticle].localpos.x + g_enemypos.x;
				g_ParticleSphere[nCntParticle].pos.y = g_ParticleSphere[nCntParticle].localpos.y + g_enemypos.y;
				g_ParticleSphere[nCntParticle].pos.z = g_ParticleSphere[nCntParticle].localpos.z + g_enemypos.z;



				g_ParticleSphere[nCntParticle].nLife--;
				if(g_ParticleSphere[nCntParticle].nLife <= 0)
				{
					g_ParticleSphere[nCntParticle].use = FALSE;
					ReleaseShadow(g_ParticleSphere[nCntParticle].nIdxShadow);
					g_ParticleSphere[nCntParticle].nIdxShadow = -1;
				}
				else
				{
					if(g_ParticleSphere[nCntParticle].nLife <= 80)
					{
						g_ParticleSphere[nCntParticle].material.Diffuse.x = 0.8f - (float)(80 - g_ParticleSphere[nCntParticle].nLife) / 80 * 0.8f;
						g_ParticleSphere[nCntParticle].material.Diffuse.y = 0.7f - (float)(80 - g_ParticleSphere[nCntParticle].nLife) / 80 * 0.7f;
						g_ParticleSphere[nCntParticle].material.Diffuse.z = 0.2f - (float)(80 - g_ParticleSphere[nCntParticle].nLife) / 80 * 0.2f;
					}

					if(g_ParticleSphere[nCntParticle].nLife <= 20)
					{
						// ���l�ݒ�
						g_ParticleSphere[nCntParticle].material.Diffuse.w -= 0.05f;
						if(g_ParticleSphere[nCntParticle].material.Diffuse.w < 0.0f)
						{
							g_ParticleSphere[nCntParticle].material.Diffuse.w = 0.0f;
						}
					}
				}
			}
		}

	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticleSphere(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���C�e�B���O�𖳌���
	SetLightEnable(FALSE);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_SUBTRACT);

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
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLESPHERE; nCntParticle++)
	{
		if(g_ParticleSphere[nCntParticle].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

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
			mtxScl = XMMatrixScaling(g_ParticleSphere[nCntParticle].scale.x, g_ParticleSphere[nCntParticle].scale.y, g_ParticleSphere[nCntParticle].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_ParticleSphere[nCntParticle].pos.x, g_ParticleSphere[nCntParticle].pos.y, g_ParticleSphere[nCntParticle].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_ParticleSphere[nCntParticle].material);

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
	SetFogEnable( GetFogEnable() );

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexParticleSphere(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// �@���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// ���ˌ��̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// �}�e���A���J���[�̐ݒ�
//=============================================================================
void SetColorParticleSphere(int nIdxParticle, XMFLOAT4 col)
{
	g_ParticleSphere[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int Particle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLESPHERE; nCntParticle++)
	{
		if (!g_ParticleSphere[nCntParticle].use)
		{
			//g_ParticleSphere[nCntParticle].pos = pos;
			g_ParticleSphere[nCntParticle].localpos = pos;
			g_ParticleSphere[nCntParticle].rot = rot;
			g_ParticleSphere[nCntParticle].scale = { PARTICLESPHERE_SCALE, PARTICLESPHERE_SCALE, PARTICLESPHERE_SCALE };
			g_ParticleSphere[nCntParticle].move = move;
			g_ParticleSphere[nCntParticle].material.Diffuse = col;
			g_ParticleSphere[nCntParticle].fSizeX = fSizeX;
			g_ParticleSphere[nCntParticle].fSizeY = fSizeY;
			g_ParticleSphere[nCntParticle].nLife = nLife;
			g_ParticleSphere[nCntParticle].use = TRUE;

			nIdxParticle = nCntParticle;

#ifdef DISP_SHADOW
			// �e�̐ݒ�
			g_ParticleSphere[nCntParticle].nIdxShadow = CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// �e�̐ݒ�
			if (g_ParticleSphere[nCntParticle].nIdxShadow != -1)
			{
				SetColorShadow(g_ParticleSphere[nCntParticle].nIdxShadow, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxParticle;
}




//=============================================================================
// �p�[�e�B�N�� ����
//=============================================================================
void SetParticleSphere(void)
{

	ENEMY* enemy = GetEnemy();

	// �p�[�e�B�N���p�ϐ�
	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);		// �p�[�e�B�N���̉�](������)
	XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);		// �p�[�e�B�N���̉�](������)
	XMFLOAT3		move;
	float			fAngleX, fAngleY, fAngleZ, fLength;
	int				nLife;
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
	move.x = vector.x * 1.0f * -1.0f;
	move.y = vector.y * 1.0f * -1.0f;
	move.z = vector.z * 1.0f * -1.0f;

	// ���W
	pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pos.x += vector.x * PARTICLE_START;
	pos.y += vector.y * PARTICLE_START;
	pos.z += vector.z * PARTICLE_START;


	// �p�[�e�B�N������
	static float	g_fWidthBase  = PARTICLESPHERE_WIDTH_BASE;			// ��̕�
	static float	g_fHeightBase = PARTICLESPHERE_HEIGHTBASE;			// ��̍���

	fLength = rand() % (int)(g_fWidthBase * 200) / g_fHeightBase - g_fWidthBase;



	// �v���C���[�ƃp�[�e�B�N���̍��������߂Ă���
	XMVECTOR vpos = XMLoadFloat3(&pos);
	XMVECTOR vec = XMLoadFloat3(&enemy[0].pos) - vpos;

	// ���K���i�傫�����O�`�P�ɂ���j
	vec = XMVector3Normalize(vec);

	//// �X�s�[�h�����߂Ă���
	float speed = PARTICLE_SPEED;		// ���K�������Ƃ�

	// �p�[�e�B�N���̃��C�t
	nLife = PARTICLESPHERE_LIFE;

	// fSize �� ����Ńp�[�e�B�N���̊�{�T�C�Y��ݒ肷��
	fSize = PARTICLE_SAIZE;

	// �p�[�e�B�N���̔��� �G�l�~�[�Ɍ����ďW�߂Ă���
	Particle(pos, move,  rot, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), fSize, fSize, nLife);


}

//// �v���C���[�̈ʒu�ɍ��킹�Ĕ��������Ă���
//pos.x = pPlayer->pos.x + sinf(fAngleX) * 100;		// �v���C���[�̍��E�ɔ�������
////pos.x = pPlayer->pos.x + rand() % 100;			// �v���C���[�̉E����ɔ�������
////pos.x = pPlayer->pos.x + rand() % 100 * -1;		// �v���C���[�̍�����ɔ�������

//pos.y = pPlayer->pos.y + sinf(fAngleY) * 100;		// �v���C���[�̏㉺�ɔ�������
////pos.y = pPlayer->pos.y + rand() % 100;			// �v���C���[�̏ォ��ɔ�������
////pos.y = pPlayer->pos.y + rand() % 100 * -1;		// �v���C���[�̉�����ɔ�������

//pos.z = pPlayer->pos.z + cosf(fAngleZ) * 100;		// �v���C���[�̑O��ɔ�������
////pos.z = pPlayer->pos.z + rand() % 100;			// �v���C���[�̌�납��ɔ�������
////pos.z = pPlayer->pos.z + rand() % 100 * -1;		// �v���C���[�̑O����ɔ�������
	// �x�N�g���ɃX�s�[�h�������ē������Ă���
	//move.x = vec.m128_f32[0] * speed * 1;
	//move.y = vec.m128_f32[1] * speed * 1;
	//move.z = vec.m128_f32[2] * speed * 1;

	//float speed = 0.01f;				// ���K�����Ȃ��Ƃ�
