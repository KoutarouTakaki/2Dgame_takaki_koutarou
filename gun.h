//=============================================================================
//
// バレット処理 [attack.h]
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
#define GUN_MAX		(100)		// バレットのMax数


// バレット構造体
struct GUN
{
	BOOL				use;				// true:使っている  false:未使用
	float				w, h;				// 幅と高さ
	XMFLOAT3			pos;				// バレットの座標
	XMFLOAT3			rot;				// バレットの回転量
	int					dir;
	//XMFLOAT3			move;				// バレットの移動量
	int					countAnim;			// アニメーションカウント
	int					patternAnim;		// アニメーションパターンナンバー
	int					texNo;				// 何番目のテクスチャーを使用するのか
	int					sponetime;			//出現時間
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGun(void);
void UninitGun(void);
void UpdateGun(void);
void DrawGun(void);

GUN* GetGun(void);
void SetGun(XMFLOAT3 pos, int dir);


