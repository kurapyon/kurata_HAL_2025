//=============================================================================
//
// �`���[�g���A������ [tutuorial.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "sprite.h"
#include "renderer.h"
#include "enemy.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "bullet.h"
#include "fade.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y(960)
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y(540)

// �e�N�X�`���̐�
#define TUTORIAL_TEXTURE_MAX		(1)		

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;						// ���_���
static ID3D11ShaderResourceView	*g_Texture[TUTORIAL_TEXTURE_MAX] = { NULL };	// �e�N�X�`�����


static char *g_TexturName[TUTORIAL_TEXTURE_MAX] = {
	"data/TEXTURE/tutorial.png",

};

// ���������s�������̃t���O
static BOOL		g_Load = FALSE;		

// �G�l�~�[�̐��𐔂���
int		g_tutorialCnt;

//=============================================================================
// ����������
//=============================================================================
HRESULT  InitTutorial(void)
{
	ID3D11Device *pDevice = GetDevice();


	//�e�N�X�`������
	for (int i = 0; i < TUTORIAL_TEXTURE_MAX; i++)
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
			
	// �`���[�g���A���\���̂̏�����
	g_tutorialCnt = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTutorial(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TUTORIAL_TEXTURE_MAX; i++)
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
void UpdatTutorial(void)
{
	//TUTORIAL_ENEMY_UPDATE();

	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
	{
		SetFadeMode(FADE_OUT, MODE_GAME);
	}

	
#ifdef _DEBUG	// �f�o�b�O����\������

	if (GetKeyboardTrigger(DIK_5))
	{
		SetFadeMode(FADE_OUT, MODE_GAME);
	}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorial(void)
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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

}