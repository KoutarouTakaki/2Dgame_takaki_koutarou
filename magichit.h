//=============================================================================
//
// �o���b�g���� [attack.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAGICHIT_MAX		(100)		// �o���b�g��Max��


// �o���b�g�\����
struct MAGICHIT
{
	BOOL				use;				// true:�g���Ă���  false:���g�p
	float				w, h;				// ���ƍ���
	XMFLOAT3			pos;				// �o���b�g�̍��W
	XMFLOAT3			rot;				// �o���b�g�̉�]��
	//XMFLOAT3			move;				// �o���b�g�̈ړ���
	int					countAnim;			// �A�j���[�V�����J�E���g
	int					patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
	int					sponetime;			//�o������
	BOOL				hitCount;			//�����蔻��d�����p
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMagichit(void);
void UninitMagichit(void);
void UpdateMagichit(void);
void DrawMagichit(void);

MAGICHIT* GetMagichit(void);
void SetMagichit(XMFLOAT3 pos);


