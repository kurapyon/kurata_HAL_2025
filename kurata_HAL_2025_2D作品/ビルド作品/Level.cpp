//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "Level.h"
#include "sprite.h"
#include "player.h"
#include "bg.h"
#include "playereffect.h"
#include "map.h"
#include "enemy.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(16)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(32)	// 
#define LEVEL_TEXTURE_MAX					(4)		// �e�N�X�`���̐�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
enum 
{
	TEXTURE_SUUJI,
	TEXTURE_energy01,
	TEXTURE_energy,
	TEXTURE_LEVELUP,
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[LEVEL_TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[LEVEL_TEXTURE_MAX] = {
	"data/TEXTURE/suuji.png",			// ���x���\���i�����j
	"data/TEXTURE/energy01.png",		// �X�^�~�i�Q�[�W�̘g�g��
	"data/TEXTURE/energy.png",			// �X�^�~�i�Q�[�W
	"data/TEXTURE/levelup.png",			// ���x���A�b�v�G�t�F�N�g


};


static bool						g_Use;						// true:�g���Ă���  false:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

// ���x���A�b�v
int lvupcnt = 0;

static LEVEL				g_level[LEVEL_MAX];							// ���x���\����


static int						g_Score;					// �X�R�A

static BOOL						g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitLevel(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < LEVEL_TEXTURE_MAX; i++)
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

	// ���x���\���̂̏�����
	for (int i = 0; i < LEVEL_MAX; i++)
	{
		g_level[i].guage    = 0;
		g_level[i].guagemax = 100;
		g_level[i].level    = 1;
	}

	// �X�R�A�̏�����
	g_Use   = true;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   =XMFLOAT3{ 500.0f, 20.0f, 0.0f };
	g_TexNo = 0;

	g_Score = 0;	// �X�R�A�̏�����

	g_Load = TRUE;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitLevel(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < LEVEL_TEXTURE_MAX; i++)
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
void UpdateLevel(void)
{
	PLAYER* player = GetPlayer();

	// �o���l���
	if (g_level[0].guagemax <= g_level[0].guage)
	{
		// �Q�[�W�}�b�N�X���A�Q�[�W���O�ɂ���
		g_level[0].guage -= g_level[0].guagemax;

		// ���x�����オ�邲�ƂɁA�Q�[�W�}�b�N�X���グ��
		g_level[0].guagemax += (g_level[0].level * 30);

		// �Q�[�W���}�b�N�X�̎��Ƀ��x���A�b�v����
		g_level[0].level++;

		// �G�t�F�N�g
   		SetPlayerEffect(player[0].pos.x, player[0].pos.y, 30, TEXTURE_LVUP);

		// ���x���A�b�v
		SetLevelUP();
	}


#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawLevel(void)
{
	PLAYER* player = GetPlayer();
	BG* bg = GetBG();


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


	// �o���l(���x���A�b�v)
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[0].levelup == TRUE)
		{
			// ���x���A�b�v�\��
			lvupcnt++;
			float alpha;
			if ((lvupcnt / 8) % 2 == 0)
			{
				alpha = 1.0f;
			}
			else
			{
				alpha = 0.0f;
			}

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_LEVELUP]);

			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = player[i].pos.x - scrollX;									// ���x���A�b�v�����̕\���ʒuX
			float py = player[i].pos.y - scrollY - 10.0f;							// ���x���A�b�v�����̕\���ʒuY�i-10���邱�Ƃł������̏�ɕ\���ł���j
			float pw = 50;															// ���x���A�b�v�����̕\����
			float ph = 15;															// ���x���A�b�v�����̕\������

			py += player[0].jumpY;													// �W�����v���̍����𑫂�

			// �A�j���[�V�����p
			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���


			float tx = 1.0f;	// �e�N�X�`���̍���X���W
			float ty = 1.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				player[0].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			if (lvupcnt == 100)
			{
				player[i].levelup = FALSE;
				lvupcnt = 0;
			}

		}

	}

	 //�o���l�o�[
	{

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 20.0f;					// �Q�[�W�̕\���ʒuX
		float py = 90.0f;					// �Q�[�W�̕\���ʒuY
		float pw = 250.0;					// �Q�[�W�̕\����
		float ph = 18.0f;					// �Q�[�W�̕\������

		float tw = 1.0f;					// �e�N�X�`���̕�
		float th = 1.0f;					// �e�N�X�`���̍���
		float tx = 0.0f;					// �e�N�X�`���̍���X���W
		float ty = 0.0f;					// �e�N�X�`���̍���Y���W

		// �G�l�~�[�̐��ɏ]���ăQ�[�W�̒�����\�����Ă݂�
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_energy]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		if (g_level[0].guage < g_level[0].guagemax)
		{
			pw = pw * (g_level[0].guage / g_level[0].guagemax);

		}

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);




	// ���~���̃Q�[�W�i�g�I�ȕ��j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_energy01]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		px = 10.0f;							// �Q�[�W�̕\���ʒuX
		py = 90.0f;							// �Q�[�W�̕\���ʒuY
		pw = 300.0;							// �Q�[�W�̕\����
		ph = 18.0f;							// �Q�[�W�̕\������

		tw = 1.0f;							// �e�N�X�`���̕�
		th = 1.0f;							// �e�N�X�`���̍���
		tx = 0.0f;							// �e�N�X�`���̍���X���W
		ty = 0.0f;							// �e�N�X�`���̍���Y���W

	   // �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,px, py, pw, ph,tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}


	// �����A�v���C���[�����x���A�b�v������\����ς���
	// ��������������
	int number = g_level[0].level;
	for (int i = 0; i < LEVEL_DIGIT; i++)
	{

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SUUJI]);

		// ����\�����錅�̐���
		float x = (float)(number % 10);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Pos.x - g_w * i;	// �X�R�A�̕\���ʒuX
		float py = g_Pos.y;				// �X�R�A�̕\���ʒuY
		float pw = g_w;					// �X�R�A�̕\����
		float ph = g_h;					// �X�R�A�̕\������

		float tw = 1.0f / 13;			// �e�N�X�`���̕�
		float th = 1.0f / 1;			// �e�N�X�`���̍���
		float tx = x * tw;				// �e�N�X�`���̍���X���W
		float ty = 0.0f;				// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// ���̌���
		number /= 10;
	}
}


// �o���l�W��
void GuagePlus(float num) 
{
	g_level[0].guage += num;
}


LEVEL* GetLevel(void)
{
	return &g_level[0];
}

