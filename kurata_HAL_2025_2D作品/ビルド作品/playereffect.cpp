//=============================================================================
//
// �G�t�F�N�g���� [effect.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "playereffect.h"
#include "player.h"
#include "bg.h"
#include "map.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define EFFECT_TEXTURE_SIZE_X	(100)		// �e�N�X�`���T�C�Y
#define EFFECT_TEXTURE_SIZE_Y	(100)		// ����

#define EFFECT_TIME_ANIMATION			(1)	// �A�j���[�V�����̐؂�ւ��J�E���g

#define EMISSION_FULL 0		//�p�[�e�B�N���S�����t���O
#define EMISSION_RATE 5		//�p�[�e�B�N���̐����Ԋu(duration/EMISSION_RATE�̐����G�t�F�N�g���o��)

#define EMISSION_WIDTH  50	//�p�[�e�B�N�������͈́i�����j
#define EMISSION_HEIGHT 50	//�p�[�e�B�N�������͈́i�����j


#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void ResetPlayerParticle(int i, int n);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/levelup01.png",
	"data/TEXTURE/effect01.png",

};

//// �A�j���p�^�[����(X,Y)
//int animepatternX;
//int animepatternY;
//// �A�j���p�^�[�����̍��v
int animepatternNUM;

static BOOL		g_Load = FALSE;								// ���������s�������̃t���O

// �G�t�F�N�g�\����
static PLAYER_EFFECT	effectWk[TEXTURE_MAX][PLAYER_NUM_EFFECTS];

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayerEffect(void)
{
	ID3D11Device* pDevice = GetDevice();

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


	// ����������
	for (int j = 0; j < TEXTURE_MAX; j++)
	{
		for (int i = 0; i < PLAYER_NUM_EFFECTS; i++)
		{
			effectWk[j][i].use = false;
			effectWk[j][i].elapsed = 0;

			for (int n = 0; n < PLAYER_NUM_PARTS; n++)
			{
				ResetPlayerParticle(i, n);
			}
		}

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �p�[�e�B�N���̃��Z�b�g
//=============================================================================
void ResetPlayerParticle(int i, int n) 
{
	for (int j = 0; j < TEXTURE_MAX; j++)
	{

		effectWk[j][i].pParticle[n].pos = XMFLOAT3(effectWk[j][i].pos.x + rand() % EMISSION_WIDTH - (EMISSION_WIDTH / 2), effectWk[j][i].pos.y + rand() % EMISSION_HEIGHT - (EMISSION_HEIGHT / 2), 0.0f);	// ���W�f�[�^��������
		effectWk[j][i].pParticle[n].move = XMFLOAT3(0.0f, 0.0f, 0.0f);					// �ړ���

		effectWk[j][i].pParticle[n].PatternAnim = 0;									// �A�j���p�^�[���ԍ��������_���ŏ�����
		effectWk[j][i].pParticle[n].CountAnim = 0;										// �A�j���J�E���g��������
		effectWk[j][i].pParticle[n].liveTime = 2000;
		effectWk[j][i].pParticle[n].isFinish = 0;
	}
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayerEffect(void)
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
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayerEffect(void)
{
	for (int j = 0; j < TEXTURE_MAX; j++)
	{

		for (int i = 0; i < PLAYER_NUM_EFFECTS; i++)
		{
			if (effectWk[j][i].use)
			{
				if (effectWk[j][i].isEnding) {
					effectWk[j][i].use = FALSE;
					continue;
				}

				//�G�~�b�g���L���ł���΃G�t�F�N�g�쐬�������s��
				if (effectWk[j][i].isRemoveOnFinish == FALSE)
				{

					//�G�t�F�N�g�쐬���[�g�̑�������
					if (effectWk[j][i].effectCount < PLAYER_NUM_PARTS)
						effectWk[j][i].emitCounter++;

					//�o�b�t�@�ɋ󂫂�����A�ǉ��^�C�~���O�����Ă���ΐV���ȃG�t�F�N�g��ǉ�����
					while ((effectWk[j][i].effectCount < PLAYER_NUM_PARTS) && (effectWk[j][i].emitCounter > EMISSION_RATE))
					{
						//�S�̒ǉ��t���O��ON�ł���΋󂫗̈�S�ĂɐV���ȃG�t�F�N�g��ǉ�����
						if (EMISSION_FULL)
							effectWk[j][i].effectCount = PLAYER_NUM_PARTS;
						else
							effectWk[j][i].effectCount++;

						//�G�t�F�N�g�쐬���[�g�̏�����
						effectWk[j][i].emitCounter = 0;
					}

					effectWk[j][i].elapsed++;

					//���Ԓ���
					if ((effectWk[j][i].duration != -1) && (effectWk[j][i].duration < effectWk[j][i].elapsed)) {
						effectWk[j][i].isRemoveOnFinish = TRUE;
					}
				}

				int effectIndex = 0;

				//�G�t�F�N�g�̍X�V����(��قǉ��P����)
				while (effectIndex < effectWk[j][i].effectCount)
				{
					if (effectWk[j][i].pParticle[effectIndex].liveTime > 0)
					{
						//�����t���[������
						effectWk[j][i].pParticle[effectIndex].liveTime--;

						//�A�j���p�^�[���i�s
						if (++effectWk[j][i].pParticle[effectIndex].CountAnim > EFFECT_TIME_ANIMATION) {
							//�A�j���p�^�[�����ő�l�ɒB�����ꍇ�ł��I��(30��ύX����)
							if (++effectWk[j][i].pParticle[effectIndex].PatternAnim >= (30 - 1))
							{
								effectWk[j][i].pParticle[effectIndex].PatternAnim = 30 - 1;
								effectWk[j][i].pParticle[effectIndex].liveTime = 0;
							}

							effectWk[j][i].pParticle[effectIndex].CountAnim = 0;
						}

						effectIndex++;
					}
					else {
						if (effectWk[j][i].isRemoveOnFinish) {
							if (effectWk[j][i].pParticle[effectIndex].isFinish == 0)
							{
								effectWk[j][i].pParticle[effectIndex].isFinish = 1;
								effectWk[j][i].numFinish++;
							}
							//�I������
							if (effectWk[j][i].numFinish == effectWk[j][i].effectCount)
							{
								effectWk[j][i].isEnding = TRUE;
								break;
							}
							effectIndex++;
						}
						else {
							//�o�b�t�@������������
							ResetPlayerParticle(i, effectIndex);

							//�����łȂ���΃C���f�b�N�X���l�߂�
							if (effectIndex != (effectWk[j][i].effectCount - 1))
							{
								effectWk[j][i].pParticle[effectIndex] = effectWk[j][i].pParticle[effectWk[j][i].effectCount - 1];
								ResetPlayerParticle(i, (effectWk[j][i].effectCount - 1));
							}
							effectWk[j][i].effectCount--;
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
void DrawPlayerEffect(void)
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

		BG* bg = GetBG();

		PLAYER* player = GetPlayer();
		float scrollX = GetScrollX();
		float scrollY = GetScrollY();


			for (int i = 0; i < PLAYER_NUM_EFFECTS; i++)
			{
				if (effectWk[TEXTURE_LVUP][i].use == TRUE)		// ���̃G�t�F�N�g���g���Ă���H
				{		// Yes
					// �e�N�X�`���ݒ�
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_LVUP]);

					// ���x���A�b�v�G�t�F�N�g
					for (int n = 0; n < effectWk[TEXTURE_LVUP][i].effectCount; n++)
					{
						if (effectWk[TEXTURE_LVUP][i].pParticle[n].isFinish == 0) 
						{

							//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
							float px = effectWk[TEXTURE_LVUP][i].pParticle[n].pos.x - scrollX;	// �G�t�F�N�g�̕\���ʒuX
							float py = effectWk[TEXTURE_LVUP][i].pParticle[n].pos.y - scrollY;	// �G�t�F�N�g�̕\���ʒuY

							float pw = EFFECT_TEXTURE_SIZE_X;			// �G�t�F�N�g�̕\����
							float ph = EFFECT_TEXTURE_SIZE_Y;			// �G�t�F�N�g�̕\������

							px -= EFFECT_TEXTURE_SIZE_X / 4;
							py -= EFFECT_TEXTURE_SIZE_Y / 4;

							float tw = 1.0f / 10;	// �e�N�X�`���̕�
							float th = 1.0f /  1;	// �e�N�X�`���̍���
							float tx = (float)(effectWk[TEXTURE_LVUP][i].pParticle[n].PatternAnim % 10) * tw;	// �e�N�X�`���̍���X���W
							float ty = (float)(effectWk[TEXTURE_LVUP][i].pParticle[n].PatternAnim / 10) * th;	// �e�N�X�`���̍���Y���W

							// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
							SetSpriteColorRotation(g_VertexBuffer,
								px, py, pw, ph,
								tx, ty, tw, th,
								XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0.0f);

							// �|���S���`��
							GetDeviceContext()->Draw(4, 0);
						}
					}
				}

				if (effectWk[TEXTURE_EFFECT01][i].use == TRUE)		// ���̃G�t�F�N�g���g���Ă���H
				{		// Yes
					// �e�N�X�`���ݒ�
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_EFFECT01]);

					// ���x���A�b�v�G�t�F�N�g
					for (int n = 0; n < effectWk[TEXTURE_EFFECT01][i].effectCount; n++)
					{
						if (effectWk[TEXTURE_EFFECT01][i].pParticle[n].isFinish == 0)
						{

							//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
							float px = effectWk[TEXTURE_EFFECT01][i].pParticle[n].pos.x - scrollX;	// �G�t�F�N�g�̕\���ʒuX
							float py = effectWk[TEXTURE_EFFECT01][i].pParticle[n].pos.y - scrollY;	// �G�t�F�N�g�̕\���ʒuY

							float pw = EFFECT_TEXTURE_SIZE_X;			// �G�t�F�N�g�̕\����
							float ph = EFFECT_TEXTURE_SIZE_Y;			// �G�t�F�N�g�̕\������

							px -= EFFECT_TEXTURE_SIZE_X / 4;
							py -= EFFECT_TEXTURE_SIZE_Y / 4;

							float tw = 1.0f / 10;	// �e�N�X�`���̕�
							float th = 1.0f / 1;	// �e�N�X�`���̍���
							float tx = (float)(effectWk[TEXTURE_EFFECT01][i].pParticle[n].PatternAnim % 10) * tw;	// �e�N�X�`���̍���X���W
							float ty = (float)(effectWk[TEXTURE_EFFECT01][i].pParticle[n].PatternAnim / 10) * th;	// �e�N�X�`���̍���Y���W

							// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
							SetSpriteColorRotation(g_VertexBuffer,
								px, py, pw, ph,
								tx, ty, tw, th,
								XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);

							// �|���S���`��
							GetDeviceContext()->Draw(4, 0);
						}
					}
				}

			}

		
}


//=============================================================================
// �G�t�F�N�g�̃Z�b�g
// 
// int duration		�G�t�F�N�g�������̐�������
//=============================================================================
void SetPlayerEffect(float x, float y, int duration, int j)
{

		// �������g�p�̃G�t�F�N�g��������������s���Ȃ�( =����ȏ�\���ł��Ȃ����Ď� )
		for (int i = 0; i < PLAYER_NUM_EFFECTS; i++)
		{
			if (effectWk[j][i].use == false)		// ���g�p��Ԃ̃G�t�F�N�g��������
			{
				effectWk[j][i].use = true;
				effectWk[j][i].isEnding = false;
				effectWk[j][i].isRemoveOnFinish = FALSE;

				effectWk[j][i].duration = duration;
				effectWk[j][i].pos.x = x;			// ���W���Z�b�g
				effectWk[j][i].pos.y = y;			// ���W���Z�b�g

				effectWk[j][i].effectCount = 0;
				effectWk[j][i].elapsed = 0;
				effectWk[j][i].emitCounter = EMISSION_RATE;
				effectWk[j][i].numFinish = 0;

				//�p�[�e�B�N���̏�����
				for (int n = 0; n < PLAYER_NUM_PARTS; n++)
				{
					ResetPlayerParticle(i, n);
				}

				return;							// 1�Z�b�g�����̂ŏI������
			}
		}
}
