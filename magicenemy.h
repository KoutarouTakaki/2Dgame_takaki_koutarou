//=============================================================================
//
// �G�l�~�[���� [enemy.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAGICENEMY_MAX		(100)		// �G�l�~�[��Max�l��

enum
{
	MAGIC_DIR_RIGHT,
	MAGIC_DIR_LEFT,

	MAGIC_DIR_MAX
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct MAGICENEMY
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���

	int			dir;			// �����i0:�� 1:�E 2:�� 3:���j

	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// �e�N�X�`���ԍ�
	XMFLOAT3	move;			// �ړ����x
	int			attacktime;		//�U���^�C�~���O�p

	BOOL		flushing;
	int			flushingCount;
	BOOL		visible;

	float		time;			// ���`��ԗp
	int			tblNo;			// �s���f�[�^�̃e�[�u���ԍ�
	int			tblMax;			// ���̃e�[�u���̃f�[�^��

	//INTERPOLATION_DATA* tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	//int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
	//float				move_time;			// ���s����
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMagicenemy(int min, int max);
void UninitMagicenemy(void);
void UpdateMagicenemy(void);
void DrawMagicenemy(void);

MAGICENEMY* GetMagicenemy(void);

int GetMagicenemyCount(void);



