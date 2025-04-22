//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 
//
//=============================================================================
#include "door.h"
#include "input.h"
#include "fade.h"
#include "bg.h"
#include "main.h"
#include "player.h"
#include "collision.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define TEXTURE_PATTERN_DIVIDE_X	(3)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(10)		// アニメーションの切り替わるWait値
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/potal.png",
	"data/TEXTURE/saiber_2.png",
	"data/TEXTURE/saiber_3.png",
	"data/TEXTURE/saiber_4.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

//float	alpha;
//BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

static DOOR door[DOOR_MAX];

static int door_number;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitDoor(int n)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	g_TexNo = 0;

	//alpha = 1.0f;
	//flag_alpha = TRUE;

	effect_dx = 100.0f;
	effect_dy = 100.0f;

	for (int i = 0; i < DOOR_MAX; i++)
	{
		if (i == 0)
		{
			door[i].pos = XMFLOAT3(5850.0f, 1070.0f, 0.0f);
		}
		else if (i == 1)
		{
			door[i].pos = XMFLOAT3(5850.0f, 1070.0f, 0.0f);
		}
		else if (i == 2)
		{
			door[i].pos = XMFLOAT3(5650.0f, 900.0f, 0.0f);
		}
		else if (i == 3)
		{
			door[i].pos = XMFLOAT3(5850.0f, 1070.0f, 0.0f);
		}

		door[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		door[i].w = 200.0f;
		door[i].h = 200.0f;

	}

	if (n == 0)
	{
		door_number = 0;
	}
	else if (n == 1)
	{
		door_number = 1;
	}
	else if (n == 2)
	{
		door_number = 2;
	}



	g_Load = TRUE;
	return S_OK;


	//arrow_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//arrow_w = 50.0f;
	//arrow_h = 50.0f;
	//arrow_count = 0;
	//arrow_max = 3;
	//countAnim = 0;
	//patternAnim = 0;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitDoor(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateDoor(void)
{

	for (int i = 0; i < DOOR_MAX; i++)
	{
		if (i != door_number)continue;

		PLAYER* player = GetPlayer();

		// エネミーの数分当たり判定を行う
		for (int j = 0; j < PLAYER_MAX; j++)
		{
			// 生きてるエネミーと当たり判定をする
			if (player[j].use == TRUE)
			{
				BOOL ans = CollisionBB(door[i].pos, (door[i].w / 2), (door[i].h / 2),
					player[j].pos, player[j].w, player[j].h);
				// 当たっている？
				if (ans == TRUE)
				{
					int mode = GetMode();
					if (mode == MODE_TUTORIAL)
					{
						SetFade(FADE_OUT, MODE_STAGE1);
					}
					else
					{
						SetFade(FADE_OUT, MODE_RESULT);
					}
				}
			}
		}

		door[i].rot.z += 0.01f;
	}



#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawDoor(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	for (int i = 0; i < DOOR_MAX; i++)
	{
		if (i != door_number)continue;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// スコアの位置やテクスチャー座標を反映
		float px = door[i].pos.x - bg->pos.x;			// スコアの表示位置X
		float py = door[i].pos.y - bg->pos.y;			// スコアの表示位置Y
		float pw = door[i].w;				// スコアの表示幅
		float ph = door[i].h;				// スコアの表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = 0.0f;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			door[i].rot.z);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}


	SetBlendState(BLEND_MODE_ALPHABLEND);	// 半透明処理を元に戻す

}





