//=============================================================================
//
// エネミー処理 [GUNENEMY.cpp]
// Author : 
//
//=============================================================================
#include "gunenemy.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "gun.h"
#include "life.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(200/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(200/2)	// 
#define TEXTURE_MAX					(2)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(4)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// アニメパターンのテクスチャ内分割数（Y)
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
	"data/TEXTURE/gunenemy_textrue.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static GUNENEMY	g_Gunenemy[GUNENEMY_MAX];		// エネミー構造体

static int		g_GunenemyCount = GUNENEMY_MAX;

//static INTERPOLATION_DATA g_MoveTbl0[] = {
//	//座標									回転率							拡大率					時間
//	{ XMFLOAT3(850.0f, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	100 },
//	{ XMFLOAT3(1000.0f, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	100 },
//};
//
//
//static INTERPOLATION_DATA g_MoveTbl1[] = {
//	//座標									回転率							拡大率							時間
//	{ XMFLOAT3(1700.0f,   0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
//	{ XMFLOAT3(1700.0f,  SCREEN_HEIGHT, 0.0f),XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(2.0f, 2.0f, 1.0f),	60 },
//};
//
//
//static INTERPOLATION_DATA g_MoveTbl2[] = {
//	//座標									回転率							拡大率							時間
//	{ XMFLOAT3(3000.0f, 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
//	{ XMFLOAT3(3000 + SCREEN_WIDTH, 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
//};


//static INTERPOLATION_DATA* g_MoveTblAdr[] =
//{
//	g_MoveTbl0,
//	g_MoveTbl1,
//	g_MoveTbl2,
//
//};

int g_kazu;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGunenemy(int min, int max)
{
	g_kazu = max - min;

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


	// エネミー構造体の初期化
	g_GunenemyCount = 0;
	for (int i = 0; i <= g_kazu; i++)
	{
		g_GunenemyCount++;
		g_Gunenemy[i].use = TRUE;
		if (i % 3 == 0)
		{
			g_Gunenemy[i].pos = XMFLOAT3(300.0f + i * 1000.0f, 100.0f, 0.0f);	// 中心点から表示
		}
		else if (i % 3 == 1)
		{
			g_Gunenemy[i].pos = XMFLOAT3(300.0f + i * 1000.0f, 500.0f, 0.0f);	// 中心点から表示
		}
		else
		{
			g_Gunenemy[i].pos = XMFLOAT3(300.0f + i * 1000.0f, 900.0f, 0.0f);	// 中心点から表示
		}
		g_Gunenemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Gunenemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Gunenemy[i].w = TEXTURE_WIDTH;
		g_Gunenemy[i].h = TEXTURE_HEIGHT;
		g_Gunenemy[i].texNo = 0;
		g_Gunenemy[i].attacktime = 0 + (15 * i);
		g_Gunenemy[i].countAnim = 0;
		g_Gunenemy[i].patternAnim = 0;
		g_Gunenemy[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量
		g_Gunenemy[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Gunenemy[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Gunenemy[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		g_Gunenemy[i].flushing = FALSE;
		g_Gunenemy[i].flushingCount = 0;
		g_Gunenemy[i].visible = TRUE;
	}

	//// 0番だけ線形補間で動かしてみる
	//g_Gunenemy[0].time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Gunenemy[0].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
	//g_Gunenemy[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	//// 1番だけ線形補間で動かしてみる
	//g_Gunenemy[1].time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Gunenemy[1].tblNo = 1;		// 再生するアニメデータの先頭アドレスをセット
	//g_Gunenemy[1].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	//// 2番だけ線形補間で動かしてみる
	//g_Gunenemy[2].time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Gunenemy[2].tblNo = 2;		// 再生するアニメデータの先頭アドレスをセット
	//g_Gunenemy[2].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGunenemy(void)
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
void UpdateGunenemy(void)
{
	if (g_Load == FALSE) return;
	g_GunenemyCount = 0;			// 生きてるエネミーの数

	for (int i = 0; i <= g_kazu; i++)
	{
		// 生きてるエネミーだけ処理をする
		if (g_Gunenemy[i].use == TRUE)
		{

			if (g_Gunenemy[i].flushing == TRUE)
			{
				g_Gunenemy[i].flushingCount++;

				if (g_Gunenemy[i].flushingCount % 9 < 5)
				{
					g_Gunenemy[i].visible = TRUE;
				}
				else
				{
					g_Gunenemy[i].visible = FALSE;
				}

				if (g_Gunenemy[i].flushingCount >= 50)
				{
					g_Gunenemy[i].use = FALSE;
				}
			}

			if (g_Gunenemy[i].flushing != TRUE)
			{
				g_GunenemyCount++;		// 生きてた敵の数

				// 地形との当たり判定用に座標のバックアップを取っておく
				XMFLOAT3 pos_old = g_Gunenemy[i].pos;

				// アニメーション
				g_Gunenemy[i].countAnim += 1.0f;
				if (g_Gunenemy[i].countAnim > ANIM_WAIT)
				{
					g_Gunenemy[i].countAnim = 0.0f;
					// パターンの切り替え
					g_Gunenemy[i].patternAnim = (g_Gunenemy[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Gunenemy[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}

				// 移動処理
				PLAYER* player = GetPlayer();

				for (int j = 0; j < PLAYER_MAX; j++)
				{
					if (g_Gunenemy[i].pos.x <= player[j].pos.x)
					{
						g_Gunenemy[i].dir = GUNE_DIR_RIGHT;
					}
					else
					{
						g_Gunenemy[i].dir = GUNE_DIR_LEFT;
					}
				}

				float radian = atan2f(player[0].pos.y - g_Gunenemy[i].pos.y, player[0].pos.x - g_Gunenemy[i].pos.x);

				g_Gunenemy[i].pos.x += 0.2f * cosf(radian);
				g_Gunenemy[i].pos.y += 1.5f * sinf(radian);

				//// 移動処理
				//if (g_Gunenemy[i].tblMax > 0)	// 線形補間を実行する？
				//{	// 線形補間の処理
				//	int nowNo = (int)g_Gunenemy[i].time;			// 整数分であるテーブル番号を取り出している
				//	int maxNo = g_Gunenemy[i].tblMax;				// 登録テーブル数を数えている
				//	int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				//	INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Gunenemy[i].tblNo];	// 行動テーブルのアドレスを取得

				//	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				//	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				//	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				//	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				//	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				//	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				//	float nowTime = g_Gunenemy[i].time - nowNo;	// 時間部分である少数を取り出している

				//	Pos *= nowTime;								// 現在の移動量を計算している
				//	Rot *= nowTime;								// 現在の回転量を計算している
				//	Scl *= nowTime;								// 現在の拡大率を計算している

				//	// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				//	XMStoreFloat3(&g_Gunenemy[i].pos, nowPos + Pos);

				//	// 計算して求めた回転量を現在の移動テーブルに足している
				//	XMStoreFloat3(&g_Gunenemy[i].rot, nowRot + Rot);

				//	// 計算して求めた拡大率を現在の移動テーブルに足している
				//	XMStoreFloat3(&g_Gunenemy[i].scl, nowScl + Scl);
				//	g_Gunenemy[i].w = TEXTURE_WIDTH * g_Gunenemy[i].scl.x;
				//	g_Gunenemy[i].h = TEXTURE_HEIGHT * g_Gunenemy[i].scl.y;

				//	// frameを使て時間経過処理をする
				//	g_Gunenemy[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				//	if ((int)g_Gunenemy[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				//	{
				//		g_Gunenemy[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				//	}

				//}


				g_Gunenemy[i].attacktime++;
				if (g_Gunenemy[i].attacktime >= 270)
				{
					XMFLOAT3 pos = g_Gunenemy[i].pos;

					for (int j = 0; j < PLAYER_MAX; j++)
					{
						if (player[j].pos.x < g_Gunenemy[i].pos.x)
						{
							pos.x -= 300.0f;
							SetGun(pos, g_Gunenemy[i].dir);
						}
						else
						{
							pos.x += 300.0f;
							SetGun(pos, g_Gunenemy[i].dir);
						}
					}
					g_Gunenemy[i].attacktime = 0;
				}

				// 移動が終わったらエネミーとの当たり判定
				{

					// エネミーの数分当たり判定を行う
					for (int j = 0; j < GUNENEMY_MAX; j++)
					{
						// 生きてるエネミーと当たり判定をする
						if (player[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Gunenemy[i].pos, g_Gunenemy[i].w * 0.5f, g_Gunenemy[i].h * 0.5f,
								player[j].pos, player[j].w * 0.5f, player[j].h * 0.5f);
							// 当たっている？
							if (ans == TRUE && player[j].muteki == FALSE && player[j].flushing == FALSE)
							{
								// 当たった時の処理
								g_Gunenemy[i].use = FALSE;
								SubLife(1);
								player[j].muteki = TRUE;
							}
						}
					}
				}
			}
		}
	}


	//// エネミー全滅チェック
	//if (g_GunenemyCount <= 0)
	//{
 //		SetFade(FADE_OUT, MODE_RESULT);
	//}

#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGunenemy(void)
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

	for (int i = 0; i <= g_kazu; i++)
	{
		if (g_Gunenemy[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes

			if (g_Gunenemy[i].visible == FALSE) continue;

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Gunenemy[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_Gunenemy[i].pos.x - bg->pos.x;	// エネミーの表示位置X
			float py = g_Gunenemy[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
			float pw = g_Gunenemy[i].w;		// エネミーの表示幅
			float ph = g_Gunenemy[i].h;		// エネミーの表示高さ

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Gunenemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Gunenemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			//float tw = 1.0f;	// テクスチャの幅
			//float th = 1.0f;	// テクスチャの高さ
			//float tx = 0.0f;	// テクスチャの左上X座標
			//float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Gunenemy[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


	//// ゲージのテスト
	//{
	//	// 下敷きのゲージ（枠的な物）
	//	// テクスチャ設定
	//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

	//	//ゲージの位置やテクスチャー座標を反映
	//	float px = 600.0f;		// ゲージの表示位置X
	//	float py = 10.0f;		// ゲージの表示位置Y
	//	float pw = 300.0f;		// ゲージの表示幅
	//	float ph = 30.0f;		// ゲージの表示高さ

	//	float tw = 1.0f;	// テクスチャの幅
	//	float th = 1.0f;	// テクスチャの高さ
	//	float tx = 0.0f;	// テクスチャの左上X座標
	//	float ty = 0.0f;	// テクスチャの左上Y座標

	//	// １枚のポリゴンの頂点とテクスチャ座標を設定
	//	SetSpriteLTColor(g_VertexBuffer,
	//		px, py, pw, ph,
	//		tx, ty, tw, th,
	//		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	//	// ポリゴン描画
	//	GetDeviceContext()->Draw(4, 0);


	//	// エネミーの数に従ってゲージの長さを表示してみる
	//	// テクスチャ設定
	//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

	//	//ゲージの位置やテクスチャー座標を反映
	//	pw = pw * ((float)g_GunenemyCount / GUNENEMY_MAX);

	//	// １枚のポリゴンの頂点とテクスチャ座標を設定
	//	SetSpriteLTColor(g_VertexBuffer,
	//		px, py, pw, ph,
	//		tx, ty, tw, th,
	//		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

	//	// ポリゴン描画
	//	GetDeviceContext()->Draw(4, 0);


	//}




}


//=============================================================================
// GUNENEMY構造体の先頭アドレスを取得
//=============================================================================
GUNENEMY* GetGunenemy(void)
{
	return &g_Gunenemy[0];
}


// 生きてるエネミーの数
int GetGunenemyCount(void)
{
	return g_GunenemyCount;
}