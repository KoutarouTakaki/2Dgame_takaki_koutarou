//=============================================================================
//
// エネミー処理 [MAGICENEMY.cpp]
// Author : 
//
//=============================================================================
#include "magicenemy.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "magic.h"
#include "life.h"
#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(200/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(200/2)	// 
#define TEXTURE_MAX					(2)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(4)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// アニメパターンのテクスチャ内分割数（Y)
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
	"data/TEXTURE/magicenemy_textrue.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static MAGICENEMY	g_Magicenemy[MAGICENEMY_MAX];		// エネミー構造体

static int		g_MagicenemyCount = MAGICENEMY_MAX;

//static INTERPOLATION_DATA g_MoveTbl0[] = {
//	//座標									回転率							拡大率					時間
//	{ XMFLOAT3(1200.0f, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	100 },
//	{ XMFLOAT3(1400.0f, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	100 },
//};
//
//
//static INTERPOLATION_DATA g_MoveTbl1[] = {
//	//座標									回転率							拡大率							時間
//	{ XMFLOAT3(3500.0f, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
//	{ XMFLOAT3(3500.0f, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
//};
//
//
//static INTERPOLATION_DATA g_MoveTbl2[] = {
//	//座標									回転率							拡大率							時間
//	{ XMFLOAT3(5000.0f, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
//	{ XMFLOAT3(5000.0f, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
//};
//
//
//static INTERPOLATION_DATA* g_MoveTblAdr[] =
//{
//	g_MoveTbl0,
//	g_MoveTbl1,
//	g_MoveTbl2,
//
//};




static int MIN;
static int MAX;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMagicenemy(int min, int max)
{
	MIN = min;
	MAX = max;


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
	g_MagicenemyCount = 0;
	for (int i = MIN; i <= MAX; i++)
	{
		g_MagicenemyCount++;
		g_Magicenemy[i].use = TRUE;
		g_Magicenemy[i].pos = XMFLOAT3(200.0f + i * 1800.0f, 800.0f - (300 - (i % 2)), 0.0f);	// 中心点から表示
		g_Magicenemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Magicenemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Magicenemy[i].w = TEXTURE_WIDTH;
		g_Magicenemy[i].h = TEXTURE_HEIGHT;
		g_Magicenemy[i].texNo = 0;
		g_Magicenemy[i].attacktime = 0 + (100 + i);

		g_Magicenemy[i].dir = MAGIC_DIR_RIGHT;

		g_Magicenemy[i].countAnim = 0;
		g_Magicenemy[i].patternAnim = 0;

		g_Magicenemy[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量

		g_Magicenemy[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Magicenemy[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Magicenemy[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		g_Magicenemy[i].flushing = FALSE;
		g_Magicenemy[i].flushingCount = 0;
		g_Magicenemy[i].visible = TRUE;


	}

	//// 0番だけ線形補間で動かしてみる
	//g_Magicenemy[0].time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Magicenemy[0].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
	//g_Magicenemy[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	//// 1番だけ線形補間で動かしてみる
	//g_Magicenemy[1].time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Magicenemy[1].tblNo = 1;		// 再生するアニメデータの先頭アドレスをセット
	//g_Magicenemy[1].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	//// 2番だけ線形補間で動かしてみる
	//g_Magicenemy[2].time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Magicenemy[2].tblNo = 2;		// 再生するアニメデータの先頭アドレスをセット
	//g_Magicenemy[2].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMagicenemy(void)
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
void UpdateMagicenemy(void)
{
	if (g_Load == FALSE) return;
	g_MagicenemyCount = 0;			// 生きてるエネミーの数

	for (int i = MIN; i <= MAX; i++)
	{
		// 生きてるエネミーだけ処理をする
		if (g_Magicenemy[i].use == TRUE)
		{

			if (g_Magicenemy[i].flushing == TRUE)
			{
				g_Magicenemy[i].flushingCount++;

				if (g_Magicenemy[i].flushingCount % 9 < 5)
				{
					g_Magicenemy[i].visible = TRUE;
				}
				else
				{
					g_Magicenemy[i].visible = FALSE;
				}

				if (g_Magicenemy[i].flushingCount >= 50)
				{
					g_Magicenemy[i].use = FALSE;
				}
			}

			if (g_Magicenemy[i].flushing != TRUE)
			{
				g_MagicenemyCount++;		// 生きてた敵の数

				// 地形との当たり判定用に座標のバックアップを取っておく
				XMFLOAT3 pos_old = g_Magicenemy[i].pos;

				// アニメーション
				g_Magicenemy[i].countAnim += 1.0f;
				if (g_Magicenemy[i].countAnim > ANIM_WAIT)
				{
					g_Magicenemy[i].countAnim = 0.0f;
					// パターンの切り替え
					g_Magicenemy[i].patternAnim = (g_Magicenemy[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Magicenemy[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}

				// 移動処理
				PLAYER* player = GetPlayer();


				for (int j = 0; j < PLAYER_MAX; j++)
				{
					if (g_Magicenemy[i].pos.x <= player[j].pos.x)
					{
						g_Magicenemy[i].dir = MAGIC_DIR_RIGHT;
					}
					else
					{
						g_Magicenemy[i].dir = MAGIC_DIR_LEFT;
					}
				}
				float radian = atan2f(player[0].pos.y - g_Magicenemy[i].pos.y, player[0].pos.x - g_Magicenemy[i].pos.x);

				g_Magicenemy[i].pos.x += 0.2f * cosf(radian);
				g_Magicenemy[i].pos.y += 0.2f * sinf(radian);

				//// 移動処理
				//if (g_Magicenemy[i].tblMax > 0)	// 線形補間を実行する？
				//{	// 線形補間の処理
				//	int nowNo = (int)g_Magicenemy[i].time;			// 整数分であるテーブル番号を取り出している
				//	int maxNo = g_Magicenemy[i].tblMax;				// 登録テーブル数を数えている
				//	int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				//	INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Magicenemy[i].tblNo];	// 行動テーブルのアドレスを取得

				//	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				//	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				//	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				//	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				//	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				//	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				//	float nowTime = g_Magicenemy[i].time - nowNo;	// 時間部分である少数を取り出している

				//	Pos *= nowTime;								// 現在の移動量を計算している
				//	Rot *= nowTime;								// 現在の回転量を計算している
				//	Scl *= nowTime;								// 現在の拡大率を計算している

				//	// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				//	XMStoreFloat3(&g_Magicenemy[i].pos, nowPos + Pos);

				//	// 計算して求めた回転量を現在の移動テーブルに足している
				//	XMStoreFloat3(&g_Magicenemy[i].rot, nowRot + Rot);

				//	// 計算して求めた拡大率を現在の移動テーブルに足している
				//	XMStoreFloat3(&g_Magicenemy[i].scl, nowScl + Scl);
				//	g_Magicenemy[i].w = TEXTURE_WIDTH * g_Magicenemy[i].scl.x;
				//	g_Magicenemy[i].h = TEXTURE_HEIGHT * g_Magicenemy[i].scl.y;

				//	// frameを使て時間経過処理をする
				//	g_Magicenemy[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				//	if ((int)g_Magicenemy[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				//	{
				//		g_Magicenemy[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				//	}

				//}



				g_Magicenemy[i].attacktime++;
				if (g_Magicenemy[i].attacktime >= 560)
				{

					for (int j = 0; j < PLAYER_MAX; j++)
					{
						XMFLOAT3 pos = player[j].pos;

						SetMagic(pos);
					}
					g_Magicenemy[i].attacktime = 0;
				}

				// 移動が終わったらエネミーとの当たり判定
				{
					PLAYER* player = GetPlayer();

					// エネミーの数分当たり判定を行う
					for (int j = 0; j < PLAYER_MAX; j++)
					{
						// 生きてるエネミーと当たり判定をする
						if (player[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Magicenemy[i].pos, g_Magicenemy[i].w * 0.5f, g_Magicenemy[i].h * 0.5f,
								player[j].pos, player[j].w * 0.5f, player[j].h * 0.5f);
							// 当たっている？
							if (ans == TRUE && player[j].muteki == FALSE && player[j].flushing == FALSE)
							{
								// 当たった時の処理
								g_Magicenemy[i].use = FALSE;
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
	//if (g_MagicenemyCount <= 0)
	//{
 //		SetFade(FADE_OUT, MODE_RESULT);
	//}

#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMagicenemy(void)
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

	for (int i = MIN; i <= MAX; i++)
	{
		if (g_Magicenemy[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes

			if (g_Magicenemy[i].visible == FALSE) continue;

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Magicenemy[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_Magicenemy[i].pos.x - bg->pos.x;	// エネミーの表示位置X
			float py = g_Magicenemy[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
			float pw = g_Magicenemy[i].w;		// エネミーの表示幅
			float ph = g_Magicenemy[i].h;		// エネミーの表示高さ

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Magicenemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Magicenemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			//float tw = 1.0f;	// テクスチャの幅
			//float th = 1.0f;	// テクスチャの高さ
			//float tx = 0.0f;	// テクスチャの左上X座標
			//float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Magicenemy[i].rot.z);

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
	//	pw = pw * ((float)g_MagicenemyCount / MAGICENEMY_MAX);

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
// MAGICENEMY構造体の先頭アドレスを取得
//=============================================================================
MAGICENEMY* GetMagicenemy(void)
{
	return &g_Magicenemy[0];
}


// 生きてるエネミーの数
int GetMagicenemyCount(void)
{
	return g_MagicenemyCount;
}


