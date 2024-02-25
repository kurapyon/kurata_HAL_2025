//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "ui.h"
#include "sprite.h"
#include "player.h"
#include "light.h"
#include "enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(25.0f)		// �L�����T�C�Y
#define TEXTURE_HEIGHT				(25.0f)		// 
#define TEXTURE_MAX					(5)			// �e�N�X�`���̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	// �v���C���[��HP
	"data/TEXTURE/heart_main10.png",
	"data/TEXTURE/heartbase10.png",

	// �G�l�~�[��HP
	"data/TEXTURE/energy.png",
	"data/TEXTURE/energy01.png",

	// �e�̎c�e��
	"data/TEXTURE/Tamaui.png",
}; 

	


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_Score;					// �X�R�A

static BOOL						g_Load = FALSE;

enum Tex
{
	heart_main10,
	heartbase10,

	energy,
	energy01,

	tama_ui,
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitUi(void)
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

	// UI�̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 300.0f, 10.0f, 0.0f };
	g_TexNo = 0;

	g_Score = 0;	// �X�R�A�̏�����

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitUi(void)
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

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateUi(void)
{


#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawUi(void)
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

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	PLAYER* player = GetPlayer();
	ENEMY* enemy = GetEnemy();
	ENEMYBOSS* boss = GetEnemyBoss();

	// �v���C���[��HP
	{
		// ���~���̃Q�[�W�i�g�I�ȕ��j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[heartbase10]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 10.0f;	// �Q�[�W�̕\���ʒuX
		float py = 10.0f;	// �Q�[�W�̕\���ʒuY
		float pw = 300.0f;	// �Q�[�W�̕\����
		float ph = 30.0f;	// �Q�[�W�̕\������

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

	   // �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// ���C�t
		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		px = 10.0f;	// �Q�[�W�̕\���ʒuX
		py = 10.0f;		// �Q�[�W�̕\���ʒuY
		pw = 300.0f;	// �Q�[�W�̕\����
		ph = 30.0f;		// �Q�[�W�̕\������

		tw = 1.0f;		// �e�N�X�`���̕�
		th = 1.0f;		// �e�N�X�`���̍���
		tx = 0.0f;		// �e�N�X�`���̍���X���W
		ty = 0.0f;		// �e�N�X�`���̍���Y���W

		pw = pw * (float)player[0].hp / PLAYER_HP;
		tw = tw * (float)player[0].hp / PLAYER_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[heart_main10]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �e�̎c�萔
	for(int i = 0; i < player->reload; i++)
	{
		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px =  50.0f;						// �Q�[�W�̕\���ʒuX
		float py =  300.0f - TEXTURE_HEIGHT * i;	// �Q�[�W�̕\���ʒuY
		float pw =  50.0f;						// �Q�[�W�̕\����
		float ph = TEXTURE_HEIGHT;						// �Q�[�W�̕\������

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[tama_ui]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �G�l�~�[��HP(��)
	// �}�e���A���ݒ�
	material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	SetMaterial(material);


	if(enemy[1].use == TRUE)
	{
		// ���C�t
		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 650.0f;	// �Q�[�W�̕\���ʒuX
		float py = 17.0f;	// �Q�[�W�̕\���ʒuY
		float pw = 250.0f;	// �Q�[�W�̕\����
		float ph = 10.0f;	// �Q�[�W�̕\������
			  
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		pw = pw * (float)enemy[1].hp / TEMPLE_HP;
		tw = tw * (float)enemy[1].hp / TEMPLE_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// ���~���̃Q�[�W�i�g�I�ȕ��j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy01]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		px = 630.0f;	// �Q�[�W�̕\���ʒuX
		py = 15.0f;		// �Q�[�W�̕\���ʒuY
		pw = 300.0f;	// �Q�[�W�̕\����
		ph = 18.0f;		// �Q�[�W�̕\������

		tw = 1.0f;		// �e�N�X�`���̕�
		th = 1.0f;		// �e�N�X�`���̍���
		tx = 0.0f;		// �e�N�X�`���̍���X���W
		ty = 0.0f;		// �e�N�X�`���̍���Y���W

	   // �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


	}

	// �G�l�~�[��HP(��)

	// �}�e���A���ݒ�
	material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	SetMaterial(material);

	if(enemy[1].use == FALSE)
	{
		// ���C�t
		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 650.0f;	// �Q�[�W�̕\���ʒuX
		float py = 17.0f;	// �Q�[�W�̕\���ʒuY
		float pw = 250.0f;	// �Q�[�W�̕\����
		float ph = 10.0f;	// �Q�[�W�̕\������
			  
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		pw = pw * (float)enemy[0].hp / ENEMY_HP;
		tw = tw * (float)enemy[0].hp / ENEMY_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		
		// ���~���̃Q�[�W�i�g�I�ȕ��j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy01]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		 px = 630.0f;	// �Q�[�W�̕\���ʒuX
		 py = 15.0f;	// �Q�[�W�̕\���ʒuY
		 pw = 300.0f;	// �Q�[�W�̕\����
		 ph = 18.0f;	// �Q�[�W�̕\������

		 tw = 1.0f;	// �e�N�X�`���̕�
		 th = 1.0f;	// �e�N�X�`���̍���
		 tx = 0.0f;	// �e�N�X�`���̍���X���W
		 ty = 0.0f;	// �e�N�X�`���̍���Y���W

	   // �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


	}


	// �{�X��HP(��)

	// �}�e���A���ݒ�
	material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	SetMaterial(material);

	if (boss->use == TRUE)
	{
		// ���C�t
		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 650.0f;	// �Q�[�W�̕\���ʒuX
		float py = 17.0f;	// �Q�[�W�̕\���ʒuY
		float pw = 250.0f;	// �Q�[�W�̕\����
		float ph = 10.0f;	// �Q�[�W�̕\������

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		pw = pw * (float)boss->hp / ENEMYBOSS_HP;
		tw = tw * (float)boss->hp / ENEMYBOSS_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// ���~���̃Q�[�W�i�g�I�ȕ��j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy01]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		px = 630.0f;	// �Q�[�W�̕\���ʒuX
		py = 15.0f;	// �Q�[�W�̕\���ʒuY
		pw = 300.0f;	// �Q�[�W�̕\����
		ph = 18.0f;	// �Q�[�W�̕\������

		tw = 1.0f;	// �e�N�X�`���̕�
		th = 1.0f;	// �e�N�X�`���̍���
		tx = 0.0f;	// �e�N�X�`���̍���X���W
		ty = 0.0f;	// �e�N�X�`���̍���Y���W

	  // �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);
}


//=============================================================================
// �X�R�A�����Z����
// ����:add :�ǉ�����_���B�}�C�i�X���\
//=============================================================================
void AddScore(int add)
{
	g_Score += add;
	if (g_Score > SCORE_MAX)
	{
		g_Score = SCORE_MAX;
	}

}


int GetScore(void)
{
	return g_Score;
}

