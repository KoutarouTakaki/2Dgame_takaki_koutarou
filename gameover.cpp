//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : 
//
//=============================================================================
#include "gameover.h"
#include "input.h"
#include "score.h"
#include "fade.h"
#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(700)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(150)			// 

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
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/clear_title.jpg",
	"data/TEXTURE/GameOver_text.png",
	"data/TEXTURE/Retry_text.png",
	"data/TEXTURE/cooltext465274065798148.png",
	"data/TEXTURE/serect_arrow.png",
};

static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static BOOL						g_Load = FALSE;

//矢印用の変数
int			go_arrow_count;
int 		go_countAnim;		// アニメーションカウント
int			go_patternAnim;	// アニメーションパターンナンバー

int			mode;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGameover(void)
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
	g_Pos = { g_w / 2, g_h / 2, 0.0f };
	g_TexNo = 0;

	// BGM再生
	go_arrow_count = 0;
	go_countAnim = 0;
	go_patternAnim = 0;


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGameover(void)
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
void UpdateGameover(void)
{

	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonPressed(0, BUTTON_A))
	{// Enter押したら、ステージを切り替える
		switch (go_arrow_count)
		{
		case 0:
			switch (mode)
			{
			case 3:
				SetFade(FADE_OUT, MODE_STAGE1);
				break;

			case 4:
				SetFade(FADE_OUT, MODE_STAGE2);
				break;

			case 5:
				SetFade(FADE_OUT, MODE_BOSS);
				break;
			}
			break;

		case 1:
			SetFade(FADE_OUT, MODE_STAGESERECT);
			break;
		}
	}
	else if (GetKeyboardTrigger(DIK_W) || GetKeyboardTrigger(DIK_UPARROW) || IsButtonPressed(0, BUTTON_UP))
	{
		go_arrow_count--;

		if (go_arrow_count < 0)
		{
			go_arrow_count = 0;
		}
	}
	else if (GetKeyboardTrigger(DIK_S) || GetKeyboardTrigger(DIK_DOWNARROW) || IsButtonPressed(0, BUTTON_DOWN))
	{
		go_arrow_count++;

		if (go_arrow_count >= 2)
		{
			go_arrow_count = 1;
		}
	}

	// アニメーション
	go_countAnim++;
	if ((go_countAnim % ANIM_WAIT) == 0)
	{
		// パターンの切り替え
		go_patternAnim = (go_patternAnim + 1) % ANIM_PATTERN_NUM;
	}


#ifdef _DEBUG	// デバッグ情報を表示する

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGameover(void)
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

	// リザルトの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// リザルトのロゴを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// リザルトの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 600.0f, 400.0f, 200.0f, 40.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// リザルトの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 610.0f, 450.0f, 250.0f, 50.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

	//プレイヤーの位置やテクスチャー座標を反映
	float px = 580.0f;	// プレイヤーの表示位置X
	float py = 420.0f + (55.0f * go_arrow_count);	// プレイヤーの表示位置Y
	float pw = 50.0f;		// プレイヤーの表示幅
	float ph = 50.0f;		// プレイヤーの表示高さ

	// アニメーション用
	float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
	float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
	float ty = (float)(go_patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標
	float tx = (float)(go_patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標

	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));


	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


}

void SetOldMode(void)
{
	mode = GetMode();

}


