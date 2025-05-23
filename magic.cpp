//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : 
//
//=============================================================================
//#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "bg.h"
#include "effect.h"
#include "magic.h"
#include "player.h"
#include "magichit.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(250/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(250/2)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/attack.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static MAGIC	g_Magic[MAGIC_MAX];	// バレット構造体
static ENEMY	g_Enemy[ENEMY_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMagic(void)
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


	// バレット構造体の初期化
	for (int i = 0; i < MAGIC_MAX; i++)
	{
		g_Magic[i].use = FALSE;			// 未使用（発射されていない弾）
		g_Magic[i].w = TEXTURE_WIDTH;
		g_Magic[i].h = TEXTURE_HEIGHT;
		g_Magic[i].pos = XMFLOAT3(300, 300.0f, 0.0f);
		g_Magic[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Magic[i].texNo = 0;
		g_Magic[i].sponetime = 0;

		g_Magic[i].countAnim = 0;
		g_Magic[i].patternAnim = 0;

		//g_Magic[i].move = XMFLOAT3(0.0f, -BULLET_SPEED, 0.0f);	// 移動量を初期化
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMagic(void)
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

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMagic(void)
{
	if (g_Load == FALSE) return;
	int attackCount = 0;				// 処理したバレットの数

	for (int i = 0; i < MAGIC_MAX; i++)
	{
		if (g_Magic[i].use == TRUE)	// このバレットが使われている？
		{								// Yes
			// アニメーション  
			g_Magic[i].countAnim++;
			if ((g_Magic[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				g_Magic[i].patternAnim = (g_Magic[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}


			//// バレットの移動処理
			//XMVECTOR pos = XMLoadFloat3(&g_Magic[i].pos);
			////XMVECTOR move = XMLoadFloat3(&g_Magic[i].move);
			////pos += move;
			//XMStoreFloat3(&g_Magic[i].pos, pos);

			//// 画面外まで進んだ？
			//BG* bg = GetBG();
			//if (g_Magic[i].pos.y < (-g_Magic[i].h / 2))		// 自分の大きさを考慮して画面外か判定している
			//{
			//	g_Magic[i].use = false;
			//}
			//if (g_Magic[i].pos.y > (bg->h + g_Magic[i].h / 2))	// 自分の大きさを考慮して画面外か判定している
			//{
			//	g_Magic[i].use = false;
			//}

			// 当たり判定処理
			{
				PLAYER* player = GetPlayer();

				//// エネミーの数分当たり判定を行う
				//for (int j = 0; j < ENEMY_MAX; j++)
				//{
				//	// 生きてるエネミーと当たり判定をする
				//	if (enemy[j].use == TRUE)
				//	{
				//		BOOL ans = CollisionBB(g_Magic[i].pos, g_Magic[i].w, g_Magic[i].h,
				//			enemy[j].pos, enemy[j].w, enemy[j].h);
				//		// 当たっている？
				//		if (ans == TRUE)
				//		{
				//			// 当たった時の処理
				//			enemy[j].use = FALSE;
				//			AddScore(100);
				//			// エフェクト発生
				//			SetEffect(enemy[j].pos.x, enemy[j].pos.y, 30);
				//		}
				//	}
				//}

				//for (int j = 0; j < ENEMY_MAX; j++)
				//{
				//	// 生きてるエネミーと当たり判定をする
				//	if (enemy[j].use == TRUE)
				//	{
				//		g_Magic[i].pos = enemy[j].pos;

				//		for (int k = 0; k < PLAYER_MAX; k++)
				//		{
				//			if (enemy[j].pos.x < player[k].pos.x)
				//			{
				//				g_Magic[i].pos.x += 100.0f;
				//			}
				//			else
				//			{
				//				g_Magic[i].pos.x -= 100.0f;
				//			}
				//		}
				//	}
				//}

			}

			attackCount++;
			g_Magic[i].sponetime++;
			if (g_Magic[i].sponetime >= 50)
			{
				g_Magic[i].use = false;
				g_Magic[i].sponetime = 0;
				SetMagichit(g_Magic[i].pos);
			}
		}
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMagic(void)
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

	for (int i = 0; i < MAGIC_MAX; i++)
	{
		if (g_Magic[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Magic[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Magic[i].pos.x - bg->pos.x;	// バレットの表示位置X
			float py = g_Magic[i].pos.y - bg->pos.y;	// バレットの表示位置Y
			float pw = g_Magic[i].w;		// バレットの表示幅
			float ph = g_Magic[i].h;		// バレットの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Magic[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Magic[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.7f),
				g_Magic[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
MAGIC* GetMagic(void)
{
	return &g_Magic[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetMagic(XMFLOAT3 pos)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < MAGIC_MAX; i++)
	{
		if (g_Magic[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Magic[i].use = TRUE;			// 使用状態へ変更する
			g_Magic[i].pos = pos;			// 座標をセット
			return;							// 1発セットしたので終了する
		}
	}
}

