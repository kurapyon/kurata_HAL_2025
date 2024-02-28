//=============================================================================
//
// �����蔻�菈�� [collision.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "collision.h"
#include "map.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************


//=============================================================================
// BB�ɂ�铖���蔻�菈��
// ��]�͍l�����Ȃ�
// �߂�l�F�������Ă���true
//=============================================================================
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh,
	XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// �O����Z�b�g���Ă���

	// ���W�����S�_�Ȃ̂Ōv�Z���₷�������ɂ��Ă���
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// �o�E���f�B���O�{�b�N�X(BB)�̏���
	if ((mpos.x + mw > ypos.x - yw) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y - yh) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// �����������̏���
		ans = TRUE;
	}

	return ans;
}

//=============================================================================
// BC�ɂ�铖���蔻�菈��
// �T�C�Y�͔��a
// �߂�l�F�������Ă���TRUE
//=============================================================================
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;						// �O����Z�b�g���Ă���

	float len = (r1 + r2) * (r1 + r2);		// ���a��2�悵����
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2�_�Ԃ̋����i2�悵�����j
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// ���a��2�悵������苗�����Z���H
	if (len > lenSq)
	{
		ans = TRUE;	// �������Ă���
	}

	return ans;
}

// �v���C���[�̓����蔻��
BOOL CollisionEdgeAndMapChip(XMFLOAT3 spos, XMFLOAT3 epos, int dir) //��:0 ��:1 ��:2 �E:3 
{
    int y = 0;
    int x = 0;

    int sy = 0;
    int sx = 0;

    int ey = 0;
    int ex = 0;

    switch (dir)
    {
    case UPPER: //��
    case LOWER: //��
        y  = (int)(spos.y)  / MAP_TEXTURE_HEIGHT;
        sx = (int)(spos.x)  / MAP_TEXTURE_WIDTH;
        ex = (int)(epos.x)  / MAP_TEXTURE_WIDTH;

        for (int x = sx; x <= ex; x++)
        {
            if (GetMapDate(y, x) == 0)
            {
                return TRUE;
            }
        }
        break;


    case LEFT:  //��
    case RIGHT: //�E

        x  = (int)(spos.x) / MAP_TEXTURE_WIDTH;
        sy = (int)(spos.y) / MAP_TEXTURE_HEIGHT;
        ey = (int)(epos.y) / MAP_TEXTURE_HEIGHT;

        for (int y = sy; y <= ey; y++)
        {
            if (GetMapDate(y, x) != -1)
            {
                return TRUE;
            }
        }
        break;
    }

    return FALSE;
}

