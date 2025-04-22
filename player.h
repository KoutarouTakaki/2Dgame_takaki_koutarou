//=============================================================================
//
// �v���C���[���� [player.h]
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
#define PLAYER_MAX			(1)		// �v���C���[��Max�l��

#define	PLAYER_OFFSET_CNT	(16)	// 16���g

enum
{
	CHAR_DIR_UP_R,
	CHAR_DIR_UP_L,
	CHAR_DIR_DOWN_R,
	CHAR_DIR_DOWN_L,
	CHAR_DIR_RIGHT,
	CHAR_DIR_LEFT,
	CHAR_DIR_ATTACK_R,
	CHAR_DIR_ATTACK_L,

	CHAR_DIR_MAX
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct PLAYER
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	old_pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// �e�N�X�`���ԍ�

	int			dir;			// �����i0:�� 1:�E 2:�� 3:���j
	int			dir_old;
	float		dirBullet;		// �ʂ̔��˕���
	float		dirBullet_old;	// �ʂ̔��˕���
	BOOL		moving;			// �ړ����t���O
	BOOL		onground;

	BOOL		dash;			// �_�b�V�����t���O
	int			dashCount;		//
	XMFLOAT3	move;			// �ړ����x
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// �c���|���S���̍��W
	BOOL		muteki;			//���G�p�t���O
	int			mutekiCount;

	BOOL		flushing;
	int			flushingCount;
	BOOL		visible;

	BOOL		jump;			// �W�����v�t���O
	float		jumpY;			// �W�����v�̍���
	int			jumpCnt;		// �W�����v���̃J�E���g
	float		jumpYMax;		// 

	BOOL		atk_use;
	int			atkCnt;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);

int GetPlayerCount(void);


