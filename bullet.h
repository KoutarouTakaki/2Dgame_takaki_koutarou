//=============================================================================
//
// バレット処理 [bullet.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BULLET_MAX		(10)		// バレットのMax数
//#define BULLET_SPEED	(6.0f)		// バレットの移動スピード

//enum
//{
//	CHAR_DIR_UP,
//	CHAR_DIR_RIGHT,
//	CHAR_DIR_DOWN,
//	CHAR_DIR_LEFT,
//
//	CHAR_DIR_MAX
//};


// バレット構造体
struct BULLET
{
	BOOL				use;				// true:使っている  false:未使用
	float				w, h;				// 幅と高さ
	XMFLOAT3			pos;				// バレットの座標
	XMFLOAT3			rot;				// バレットの回転量
	XMFLOAT3			move;				// バレットの移動量
	int					countAnim;			// アニメーションカウント
	int					patternAnim;		// アニメーションパターンナンバー
	int					texNo;				// 何番目のテクスチャーを使用するのか
	int					dir;			// 向き（0:上 1:右 2:下 3:左）

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET* GetBullet(void);
int BulletCount(void);
void SetBullet(XMFLOAT3 pos, int dir);
void AddBullet(void);


