//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 
//
//=============================================================================
#include "stageserect.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(8)				// テクスチャの数

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
	"data/TEXTURE/stageserect_bg.jpg",
	"data/TEXTURE/cooltext465274065798148.png",
	"data/TEXTURE/haguruma_001.png",
	"data/TEXTURE/haguruma_002.png",
	"data/TEXTURE/haguruma_003.png",
	"data/TEXTURE/backspace.png",
	"data/TEXTURE/EXIT_text.png",
	"data/TEXTURE/serect_arrow.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号


static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

//矢印用の変数
XMFLOAT3	s_arrow_pos;
float		s_arrow_w, s_arrow_h;
int			s_arrow_count;
int 		s_countAnim;		// アニメーションカウント
int			s_patternAnim;	// アニメーションパターンナンバー


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStageSerect(void)
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


	effect_dx = 100.0f;
	effect_dy = 100.0f;

	g_Load = TRUE;
	return S_OK;


	s_arrow_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	s_arrow_w = 50.0f;
	s_arrow_h = 50.0f;
	s_arrow_count = 0;
	s_countAnim = 0;
	s_patternAnim = 0;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStageSerect(void)
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
void UpdateStageSerect(void)
{

	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
	{// Enter押したら、ステージを切り替える
		if (s_arrow_count == 0)
		{
			SetFade(FADE_OUT, MODE_STAGE1);
		}
		else if (s_arrow_count == 1)
		{
			SetFade(FADE_OUT, MODE_STAGE2);
		}
		else if (s_arrow_count == 2)
		{
			SetFade(FADE_OUT, MODE_BOSS);
		}
	}
	else if (GetKeyboardTrigger(DIK_BACKSPACE) || IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (GetKeyboardTrigger(DIK_A) || GetKeyboardTrigger(DIK_LEFTARROW) || IsButtonPressed(0, BUTTON_LEFT))
	{
		s_arrow_count--;

		if (s_arrow_count < 0)
		{
			s_arrow_count = 0;
		}
	}
	else if (GetKeyboardTrigger(DIK_D) || GetKeyboardTrigger(DIK_RIGHTARROW) || IsButtonPressed(0, BUTTON_RIGHT))
	{
		s_arrow_count++;

		if (s_arrow_count >= 3)
		{
			s_arrow_count = 2;
		}
	}

	// アニメーション
	s_countAnim++;
	if ((s_countAnim % ANIM_WAIT) == 0)
	{
		// パターンの切り替え
		s_patternAnim = (s_patternAnim + 1) % ANIM_PATTERN_NUM;
	}

	//// ゲームパッドで入力処理
	//else if (IsButtonTriggered(0, BUTTON_START))
	//{
	//	SetFade(FADE_OUT, MODE_GAME);
	//}
	//else if (IsButtonTriggered(0, BUTTON_B))
	//{
	//	SetFade(FADE_OUT, MODE_GAME);
	//}



	//// セーブデータをロードする？
	//if (GetKeyboardTrigger(DIK_L))
	//{
	//	SetLoadGame(TRUE);
	//	SetFade(FADE_OUT, MODE_GAME);
	//}


#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawStageSerect(void)
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

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//テキストを描画
	//STAGE SERECT
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 100.0f, 70.0f, 400.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//stage1
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 150.0f, 300.0f, 200.0f, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//stage2
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 400.0f, 200.0f, 200.0f, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//stage3
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 650.0f, 100.0f, 200.0f, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//EXIT
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 500.0f, 450.0f, 300.0f, 50.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 800.0f, 450.0f, 100.0f, 40.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}




	SetBlendState(BLEND_MODE_ALPHABLEND);	// 半透明処理を元に戻す





	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

	//プレイヤーの位置やテクスチャー座標を反映
	float px = 110.0f + (250.0f * s_arrow_count);	// プレイヤーの表示位置X
	float py = 400.0f - (100.0f * s_arrow_count);	// プレイヤーの表示位置Y
	float pw = 100.0f;		// プレイヤーの表示幅
	float ph = 100.0f;		// プレイヤーの表示高さ

	// アニメーション用
	float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
	float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
	float ty = (float)(s_patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標
	float tx = (float)(s_patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標

	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));


	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

}





