//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_MAX			(1)		// プレイヤーのMax人数

#define	PLAYER_OFFSET_CNT	(16)	// 16分身

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
// 構造体定義
//*****************************************************************************

struct PLAYER
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	old_pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号

	int			dir;			// 向き（0:上 1:右 2:下 3:左）
	int			dir_old;
	float		dirBullet;		// 玉の発射方向
	float		dirBullet_old;	// 玉の発射方向
	BOOL		moving;			// 移動中フラグ
	BOOL		onground;

	BOOL		dash;			// ダッシュ中フラグ
	int			dashCount;		//
	XMFLOAT3	move;			// 移動速度
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// 残像ポリゴンの座標
	BOOL		muteki;			//無敵用フラグ
	int			mutekiCount;

	BOOL		flushing;
	int			flushingCount;
	BOOL		visible;

	BOOL		jump;			// ジャンプフラグ
	float		jumpY;			// ジャンプの高さ
	int			jumpCnt;		// ジャンプ中のカウント
	float		jumpYMax;		// 

	BOOL		atk_use;
	int			atkCnt;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);

int GetPlayerCount(void);


