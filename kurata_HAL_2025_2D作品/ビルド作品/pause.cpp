//=============================================================================
//
// BG���� [bg.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PAUSE_TEXTURE_WIDTH				(960)
#define PAUSE_TEXTURE_HEIGHT			(304 * 2)
#define PAUSE_TEXTURE_MAX				(3)

// �J�[�\���̈ʒu
#define	PAUSE_CUR_1_X					(300.0f)
#define	PAUSE_CUR_1_Y					(300.0f)

#define	PAUSE_CUR_2_X					(300.0f)
#define	PAUSE_CUR_2_Y					(360.0f)

#define	PAUSE_CUR_3_X					(300.0f)
#define	PAUSE_CUR_3_Y					(400.0f)


// �J�[�\���̕��ƍ���
#define PAUSE_CURSOR_WIDTH				(40)
#define PAUSE_CURSOR_HEIGHT				(40)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
enum 
{
	pause,
	pause_cursor,
	black,
};



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[PAUSE_TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[PAUSE_TEXTURE_MAX] = {
	"data/TEXTURE/pause.png",			// pose���j���[
	"data/TEXTURE/cursor_pause.png",	// �J�[�\��
	"data/TEXTURE/black_f.png",			// ���w�i


};


static BOOL	g_Load = FALSE;		// ���������s�������̃t���O

// �|���S���̍��W
XMFLOAT3	g_PosePos;

// �|�[�Y���j���[���J�������ǂ���
static BOOL g_pause;

// �J�[�\���̕ϐ�
int pose_cursor;

//// �X�N���[���ϐ�
//float pose_scroll;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPause(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < PAUSE_TEXTURE_MAX; i++)
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
	g_PosePos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_pause		= FALSE;
	g_Load		= TRUE;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPause(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < PAUSE_TEXTURE_MAX; i++)
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
void UpdatePause(void)
{
	if (g_pause == FALSE)
	{	// pause���j���[�ɍs��
		if (GetKeyboardTrigger(DIK_P) || IsButtonTriggered(0, BUTTON_START))
		{
			SetPause();
			g_pause = TRUE;
		}
	}



	// �J�[�\���̈ړ�����(�㉺�̈ړ��̈ړ�)
	if (GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_UP))
	{
		pose_cursor = (--pose_cursor + TITLE_MAX) % TITLE_MAX;
}
	if (GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_DOWN))
	{
		pose_cursor = (++pose_cursor + TITLE_MAX) % TITLE_MAX;
	}

	// �����|�[�Y���j���[���J���ꂽ��
	if (g_pause == TRUE)
	{
		switch (pose_cursor)
		{
		case TITLE_GAMESTART:		// �Q�[����ʂɖ߂�
			g_PosePos = XMFLOAT3(PAUSE_CUR_1_X, PAUSE_CUR_1_Y, 0.0f);
			if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
			{
				SetPause();
				g_pause = FALSE;

			}
			break;

		case TITLE_EXPLANATION:		// �`���[�g���A��
			g_PosePos = XMFLOAT3(PAUSE_CUR_2_X, PAUSE_CUR_2_Y, 0.0f);
			if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
			{
				PlaySound(SOUND_LABEL_game_start);

				SetFadeMode(FADE_OUT, MODE_EXPLANATION);
				SetPause();

			}

			break;

		case TITLE_EXIT:		// �Q�[���I��
			g_PosePos = XMFLOAT3(PAUSE_CUR_3_X, PAUSE_CUR_3_Y, 0.0f);
			if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
			{
				GameLoop();
			}

			break;
		}


	}




#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPause(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	// pause�{�^���������ꂽ��
	if (g_pause == TRUE)
	{
		// ���w�i�̕\��
		{
			// �}�e���A���ݒ�
			MATERIAL material;
			ZeroMemory(&material, sizeof(material));
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.7f);
			SetMaterial(material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[black]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, PAUSE_TEXTURE_WIDTH, PAUSE_TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}

			// pause���ڂ�`��
		{
			// �}�e���A���ݒ�
			MATERIAL material;
			ZeroMemory(&material, sizeof(material));
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			SetMaterial(material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[pause]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, PAUSE_TEXTURE_WIDTH, PAUSE_TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		

			// �J�[�\���̕\��
		
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[pause_cursor]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, g_PosePos.x, g_PosePos.y, PAUSE_CURSOR_WIDTH, PAUSE_CURSOR_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}

	}
}