//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : 
//
//=============================================================================
//#include "bullet.h"
#include "gunenemy.h"
#include "collision.h"
#include "score.h"
#include "bg.h"
#include "effect.h"
#include "gun.h"
#include "player.h"
#include "gunhit.h"
#include "life.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(200/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(1000/2)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(8)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(8)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/beam.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static GUNHIT	g_Gunhit[GUNHIT_MAX];	// バレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGunhit(void)
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
	for (int i = 0; i < GUNHIT_MAX; i++)
	{
		g_Gunhit[i].use = FALSE;			// 未使用（発射されていない弾）
		g_Gunhit[i].w = TEXTURE_WIDTH;
		g_Gunhit[i].h = TEXTURE_HEIGHT;
		g_Gunhit[i].pos = XMFLOAT3(300, 300.0f, 0.0f);
		g_Gunhit[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Gunhit[i].texNo = 0;
		g_Gunhit[i].sponetime = 0;
		g_Gunhit[i].hitCount = FALSE;

		g_Gunhit[i].countAnim = 0;
		g_Gunhit[i].patternAnim = 0;

		//g_Gunhit[i].move = XMFLOAT3(0.0f, -BULLET_SPEED, 0.0f);	// 移動量を初期化
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGunhit(void)
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
void UpdateGunhit(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < GUNHIT_MAX; i++)
	{
		if (g_Gunhit[i].use == TRUE)	// このバレットが使われている？
		{								// Yes
			// アニメーション
			g_Gunhit[i].countAnim++;
			if ((g_Gunhit[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				//g_Gunhit[i].patternAnim = (g_Gunhit[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				g_Gunhit[i].patternAnim = (g_Gunhit[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			//if (g_Gunhit[i].patternAnim >= 2)
			//{
			//	g_Gunhit[i].use = FALSE;
			//	g_Gunhit[i].patternAnim = 0;


			//	//if (g_Gunhit[i].hitCount == TRUE)
			//	//{
			//	//	g_Gunhit[i].hitCount = FALSE;
			//	//}
			//}


			//// バレットの移動処理
			//XMVECTOR pos = XMLoadFloat3(&g_Gunhit[i].pos);
			////XMVECTOR move = XMLoadFloat3(&g_Gunhit[i].move);
			////pos += move;
			//XMStoreFloat3(&g_Gunhit[i].pos, pos);

			//// 画面外まで進んだ？
			//BG* bg = GetBG();
			//if (g_Gunhit[i].pos.y < (-g_Gunhit[i].h / 2))		// 自分の大きさを考慮して画面外か判定している
			//{
			//	g_Gunhit[i].use = false;
			//}
			//if (g_Gunhit[i].pos.y > (bg->h + g_Gunhit[i].h / 2))	// 自分の大きさを考慮して画面外か判定している
			//{
			//	g_Gunhit[i].use = false;
			//}

			// 当たり判定処理
			{
				PLAYER* player = GetPlayer();

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (player[j].use == TRUE)
					{
						if (player[j].muteki == FALSE)
						{
							BOOL ans = CollisionBB(g_Gunhit[i].pos, g_Gunhit[i].w * 0.7f, g_Gunhit[i].h,
								player[j].pos, player[j].w * 0.5f, player[j].h * 0.5f);
							// 当たっている？
							if (ans == TRUE && g_Gunhit[i].hitCount == FALSE && player[j].muteki == FALSE && player[j].flushing == FALSE)
							{
								// 当たった時の処理
								g_Gunhit[i].hitCount = TRUE;
								SubLife(1);
								player[j].flushing = TRUE;
								// エフェクト発生
								//SetEffect(player[j].pos.x, player[j].pos.y, 30);
							}
						}
					}
				}

				//for (int j = 0; j < ENEMY_MAX; j++)
				//{
				//	// 生きてるエネミーと当たり判定をする
				//	if (enemy[j].use == TRUE)
				//	{
				//		g_Gunhit[i].pos = enemy[j].pos;

				//		for (int k = 0; k < PLAYER_MAX; k++)
				//		{
				//			if (enemy[j].pos.x < player[k].pos.x)
				//			{
				//				g_Gunhit[i].pos.x += 100.0f;
				//			}
				//			else
				//			{
				//				g_Gunhit[i].pos.x -= 100.0f;
				//			}
				//		}
				//	}
				//}

				if (g_Gunhit[i].patternAnim >= ANIM_PATTERN_NUM - 1)
				{
					g_Gunhit[i].use = FALSE;
					g_Gunhit[i].patternAnim = 0;
					g_Gunhit[i].countAnim = 0;


					if (g_Gunhit[i].hitCount == TRUE)
					{
						g_Gunhit[i].hitCount = FALSE;
					}
				}


				//g_Gunhit[i].sponetime++;
				//if (g_Gunhit[i].sponetime >= 30)
				//{
				//	g_Gunhit[i].use = FALSE;
				//	g_Gunhit[i].sponetime = 0;
				//	
				//	if (g_Gunhit[i].hitCount == TRUE)
				//	{
				//		g_Gunhit[i].hitCount = FALSE;
				//	}
				//}
			}
		}
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGunhit(void)
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

	for (int i = 0; i < GUNHIT_MAX; i++)
	{
		if (g_Gunhit[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Gunhit[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Gunhit[i].pos.x - bg->pos.x;	// バレットの表示位置X
			float py = g_Gunhit[i].pos.y - bg->pos.y;	// バレットの表示位置Y
			float pw = g_Gunhit[i].w;		// バレットの表示幅
			float ph = g_Gunhit[i].h;		// バレットの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Gunhit[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Gunhit[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Gunhit[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
GUNHIT* GetGunhit(void)
{
	return &g_Gunhit[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetGunhit(XMFLOAT3 pos,int dir)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < GUNHIT_MAX; i++)
	{
		if (g_Gunhit[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Gunhit[i].use = TRUE;			// 使用状態へ変更する
			g_Gunhit[i].pos = pos;			// 座標をセット
			g_Gunhit[i].dir = dir;

			if (g_Gunhit[i].dir == GUNE_DIR_RIGHT)
			{
				g_Gunhit[i].rot.z = 1.5708f;
			}
			else if (g_Gunhit[i].dir == GUNE_DIR_LEFT)
			{
				g_Gunhit[i].rot.z = -1.5708f;
			}

			return;							// 1発セットしたので終了する
		}
	}
}

