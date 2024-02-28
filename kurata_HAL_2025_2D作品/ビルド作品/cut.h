//=============================================================================
//
// �J�b�g���� [cutting.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CUTTING_MAX		(10)		// �J�b�g��Max��
#define CUTTING_SPEED	(12.0f)		// �J�b�g�̈ړ��X�s�[�h

// �o���b�g�\����
struct CUTTING
{
	BOOL				use;				// true:�g���Ă���  false:���g�p
	float				w, h;				// ���ƍ���
	XMFLOAT3			pos;				// �J�b�g�̍��W
	XMFLOAT3			rot;				// �J�b�g�̉�]��
	XMFLOAT3			move;				// �J�b�g�̈ړ���
	XMFLOAT3			xmove;
	int					countAnim;			// �A�j���[�V�����J�E���g
	int					patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
	int					dir;			            // �����i0:�� 1:�E 2:�� 3:���j
	int					bulletlife;
	int					bulletmode;
};

//�a���̕`��p�񋓌^
enum
{

	CUTTING_START,
	CUTTING_CHU,
	CUTTING_FIN,

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitCutting(void);
void UninitCutting(void);
void UpdateCutting(void);
void DrawCutting(void);

CUTTING *GetCutting(void);
void SetCutting(XMFLOAT3 pos);


