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
#define GUNHIT_MAX		(100)		// バレットのMax数


// バレット構造体
struct GUNHIT
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
	BOOL				hitCount;			//当たり判定重複回避用
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGunhit(void);
void UninitGunhit(void);
void UpdateGunhit(void);
void DrawGunhit(void);

GUNHIT* GetGunhit(void);
void SetGunhit(XMFLOAT3 pos,int dir);


