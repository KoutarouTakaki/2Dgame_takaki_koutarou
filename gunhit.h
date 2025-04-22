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
#define GUNHIT_MAX		(100)		// �o���b�g��Max��


// �o���b�g�\����
struct GUNHIT
{
	BOOL				use;				// true:�g���Ă���  false:���g�p
	float				w, h;				// ���ƍ���
	XMFLOAT3			pos;				// �o���b�g�̍��W
	XMFLOAT3			rot;				// �o���b�g�̉�]��
	int					dir;
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
HRESULT InitGunhit(void);
void UninitGunhit(void);
void UpdateGunhit(void);
void DrawGunhit(void);

GUNHIT* GetGunhit(void);
void SetGunhit(XMFLOAT3 pos,int dir);


