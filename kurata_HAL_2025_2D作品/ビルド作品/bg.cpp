//=============================================================================
//
// BG���� [bg.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "bg.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(960)					// (SCREEN_WIDTH)	8000 // �w�i�T�C�Y
#define TEXTURE_HEIGHT				(304 * 2)				// (SCREEN_HEIGHT)	2080 
#define TEXTURE_MAX					(2)						// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)					// ���S�T�C�Y(��)
#define TEXTURE_HEIGHT_LOGO			(80)					// ���S�T�C�Y(�c)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
enum 
{
	map01,
	girl,

};



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/map01.png",	// �}�b�v
	"data/TEXTURE/girl.jpg",	// pose���j���[
};


static BOOL	g_Load = FALSE;		// ���������s�������̃t���O
static BG	g_BG;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitBG(void)
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


	// �ϐ��̏�����
	{
		g_BG.w		= TEXTURE_WIDTH;						// BG�̕�
		g_BG.h		= TEXTURE_HEIGHT;						// BG�̍���
		g_BG.pos	= XMFLOAT3(0.0f, 0.0f, 0.0f);			// BG�̍��W
		g_BG.texNo  = 0;									// �e�N�X�`���[�̔ԍ�
		g_BG.scrl   = 0.0f;									// TEX�X�N���[��
		g_BG.scrl2  = 0.0f;									// TEX�X�N���[��
		g_BG.pose   = FALSE;								// pose���j���[
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{
	g_BG.old_pos = g_BG.pos;	// �P�t���O�̏���ۑ�


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBG(void)
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


	// �t�B�[���h�̔w�i��`��
	{	
			// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[map01]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//// pose���j���[�̕`��
	//if (g_BG.pose == TRUE)
	//{
	//	// �e�N�X�`���ݒ�
	//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[girl]);

	//	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//	//SetSpriteLTColor(g_VertexBuffer,
	//	//	0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
	//	//	0.0f, 0.0f, 1.0f, 1.0f,
	//	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	//	// �|���S���`��
	//	GetDeviceContext()->Draw(4, 0);
	//}

}


//=============================================================================
// BG�\���̂̐擪�A�h���X���擾
//=============================================================================
BG* GetBG(void)
{
	return &g_BG;
}





