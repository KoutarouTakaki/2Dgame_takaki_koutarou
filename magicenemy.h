//=============================================================================
//
// エネミー処理 [enemy.h]
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
#define MAGICENEMY_MAX		(100)		// エネミーのMax人数

enum
{
	MAGIC_DIR_RIGHT,
	MAGIC_DIR_LEFT,

	MAGIC_DIR_MAX
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct MAGICENEMY
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ

	int			dir;			// 向き（0:上 1:右 2:下 3:左）

	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号
	XMFLOAT3	move;			// 移動速度
	int			attacktime;		//攻撃タイミング用

	BOOL		flushing;
	int			flushingCount;
	BOOL		visible;

	float		time;			// 線形補間用
	int			tblNo;			// 行動データのテーブル番号
	int			tblMax;			// そのテーブルのデータ数

	//INTERPOLATION_DATA* tbl_adr;			// アニメデータのテーブル先頭アドレス
	//int					tbl_size;			// 登録したテーブルのレコード総数
	//float				move_time;			// 実行時間
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMagicenemy(int min, int max);
void UninitMagicenemy(void);
void UpdateMagicenemy(void);
void DrawMagicenemy(void);

MAGICENEMY* GetMagicenemy(void);

int GetMagicenemyCount(void);



