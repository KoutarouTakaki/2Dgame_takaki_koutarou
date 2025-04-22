//=============================================================================
//
// �o���b�g���� [bullet.h]
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
#define BULLET_MAX		(10)		// �o���b�g��Max��
//#define BULLET_SPEED	(6.0f)		// �o���b�g�̈ړ��X�s�[�h

//enum
//{
//	CHAR_DIR_UP,
//	CHAR_DIR_RIGHT,
//	CHAR_DIR_DOWN,
//	CHAR_DIR_LEFT,
//
//	CHAR_DIR_MAX
//};


// �o���b�g�\����
struct BULLET
{
	BOOL				use;				// true:�g���Ă���  false:���g�p
	float				w, h;				// ���ƍ���
	XMFLOAT3			pos;				// �o���b�g�̍��W
	XMFLOAT3			rot;				// �o���b�g�̉�]��
	XMFLOAT3			move;				// �o���b�g�̈ړ���
	int					countAnim;			// �A�j���[�V�����J�E���g
	int					patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
	int					dir;			// �����i0:�� 1:�E 2:�� 3:���j

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET* GetBullet(void);
int BulletCount(void);
void SetBullet(XMFLOAT3 pos, int dir);
void AddBullet(void);


