//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "sound.h"
#include "fade.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y(960)
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y(540)
#define TEXTURE_MAX					(4)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			 

// �J�[�\���̈ʒu
#define TITLE_CUR_1_X				( 90.0f)
#define TITLE_CUR_1_Y				(280.0f)

//#define TITLE_CUR_2_X				(250.0f)
//#define TITLE_CUR_2_Y				(140.0f)

#define TITLE_CUR_3_X				(590.0f)
#define TITLE_CUR_3_Y				(280.0f)

// �J�[�\���̕��ƍ���
#define CURSOR_WIDTH				(40)
#define CURSOR_HEIGHT				(40)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
enum 
{
	title,
	item,
	cursor_title,
	button,
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/BG01.png",
	"data/TEXTURE/item.png",
	"data/TEXTURE/cursor.png",
	"data/TEXTURE/button.png",
};


XMFLOAT3					g_Pos;						// �|���S���̍��W
XMFLOAT3					g_bPos;						// �|���S���̍��W

BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
float						g_w, g_h;					// ���ƍ���
int							g_TexNo;					// �e�N�X�`���ԍ�

float	alpha;
BOOL	flag_alpha;

static BOOL					g_Load = FALSE;				// �Q�[�����[�h


static float	effect_dx;
static float	effect_dy;

// �J�[�\���̕ϐ�
int cursor;

// �X�N���[���ϐ�
float g_scroll;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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

	// ������
	g_scroll = 0.0f;

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	g_TexNo = 0;
	g_bPos = XMFLOAT3(240.0f, 400.0f, 0.0f);


	alpha = 1.0f;
	flag_alpha = TRUE;

	effect_dx = 100.0f;
	effect_dy = 100.0f;

	// �J�[�\���̕ϐ�
	cursor = 0;

	// ���ʉ�
	PlaySound(SOUND_LABEL_BGM_title_bgm);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	// �X�N���[���̂��߂̕ϐ�(+,-�Ŕ��])
	g_scroll += 0.005f;

	// �J�[�\���̈ړ�����(�㉺�̈ړ��̈ړ�)
	if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, R_STICK_RIGHT) || IsButtonTriggered(0, STICK_RIGHT))
	{
		cursor = (--cursor + TITLE_MAX) % TITLE_MAX;
	}
	if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, R_STICK_LEFT) || IsButtonTriggered(0, STICK_LEFT))
	{
		cursor = (++cursor + TITLE_MAX) % TITLE_MAX;
	}

	switch (cursor)
	{
	case TITLE_GAMESTART:		// �Q�[���X�^�[�g
		g_Pos = XMFLOAT3(TITLE_CUR_1_X, TITLE_CUR_1_Y, 0.0f);
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			//PlaySound(SOUND_LABEL_game_start);

			SetFadeMode(FADE_OUT, MODE_TUTORIAL);

		}
		break;

	case TITLE_EXIT:		// �Q�[���I��
		g_Pos = XMFLOAT3(TITLE_CUR_3_X, TITLE_CUR_3_Y, 0.0f);
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			GameLoop();
		}

		break;
	}

	//// �Z�[�u�f�[�^�����[�h����H
	//if (GetKeyboardTrigger(DIK_L))
	//{
	//	SetLoadGame(TRUE);
	//	SetFadeMode(FADE_OUT, MODE_GAME);


	//}

#ifdef _DEBUG	// �f�o�b�O����\������
	if (GetKeyboardTrigger(DIK_5))
	{
		SetFadeMode(FADE_OUT, MODE_TUTORIAL);
	}


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_scroll);
	SetMaterial(material);

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[title]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[item]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �J�[�\���̕\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[cursor_title]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, g_Pos.x, g_Pos.y, CURSOR_WIDTH, CURSOR_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

	// �^�C�g���̔w�i��`��
	{

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[button]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, g_bPos.x, g_bPos.y, TEXTURE_WIDTH / 2, TEXTURE_HEIGHT / 4, g_scroll, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}





