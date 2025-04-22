//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : 
//
//=============================================================================
#include "bullet.h"
#include "enemy.h"
#include "gunenemy.h"
#include "magicenemy.h"
#include "collision.h"
#include "score.h"
#include "bg.h"
#include "effect.h"
#include "player.h"
#include "mapchip.h"
#include "tutorialenemy.h"
#include "stage1.h"
#include "stage2.h"
#include "bossstage.h"
#include "boss.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(100/2)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(4)		// アニメパターンのテクスチャ内分割数（X)
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

static char* g_TexturName[] = {
	"data/TEXTURE/bullet.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static BULLET	g_Bullet[BULLET_MAX];	// バレット構造体


int bullet_n;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
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

	bullet_n = 10;

	// バレット構造体の初期化
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use = FALSE;			// 未使用（発射されていない弾）
		g_Bullet[i].w = TEXTURE_WIDTH;
		g_Bullet[i].h = TEXTURE_HEIGHT;
		g_Bullet[i].pos = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo = 0;
		g_Bullet[i].dir = CHAR_DIR_RIGHT;

		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move = XMFLOAT3(15.0f, 0.0f, 0.0f);	// 移動量を初期化
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
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
void UpdateBullet(void)
{
	if (g_Load == FALSE) return;
	int bulletCount = 0;				// 処理したバレットの数

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)	// このバレットが使われている？
		{								// Yes

			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Bullet[i].pos;

			// アニメーション  
			g_Bullet[i].countAnim++;
			if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// バレットの移動処理
			XMVECTOR pos = XMLoadFloat3(&g_Bullet[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_Bullet[i].move);
			pos += move;
			XMStoreFloat3(&g_Bullet[i].pos, pos);

			// 画面外まで進んだ？
			BG* bg = GetBG();
			if (g_Bullet[i].pos.y < (-g_Bullet[i].h / 2))		// 自分の大きさを考慮して画面外か判定している
			{
				g_Bullet[i].use = false;
			}
			if (g_Bullet[i].pos.y > (bg->h + g_Bullet[i].h / 2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_Bullet[i].use = false;
			}

			// 当たり判定処理
			{
				ENEMY* enemy = GetEnemy();
				GUNENEMY* gunenemy = GetGunenemy();
				MAGICENEMY* magicenemy = GetMagicenemy();
				TUTORIALENEMY* tutorialenemy = GetTutorialEnemy();
				BOSS* boss = GetBoss();

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (enemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							enemy[j].pos, enemy[j].w, enemy[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							enemy[j].flushing = TRUE;
							g_Bullet[i].use = FALSE;
							AddScore(100);

							// エフェクト発生
							SetEffect(enemy[j].pos.x, enemy[j].pos.y, 30);
						}
					}
				}

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < GUNENEMY_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (gunenemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							gunenemy[j].pos, gunenemy[j].w, gunenemy[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							gunenemy[j].flushing = TRUE;
							g_Bullet[i].use = FALSE;
							AddScore(100);

							// エフェクト発生
							SetEffect(gunenemy[j].pos.x, gunenemy[j].pos.y, 30);
							PlaySound(SOUND_LABEL_SE_EXP);
						}
					}
				}

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < MAGICENEMY_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (magicenemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							magicenemy[j].pos, magicenemy[j].w, magicenemy[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							magicenemy[j].flushing = TRUE;
							g_Bullet[i].use = FALSE;
							AddScore(100);

							// エフェクト発生
							SetEffect(magicenemy[j].pos.x, magicenemy[j].pos.y, 30);
							PlaySound(SOUND_LABEL_SE_EXP);
						}
					}
				}

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < TUTORIALENEMY_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (tutorialenemy[j].use == TRUE && j != 2)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							tutorialenemy[j].pos, tutorialenemy[j].w, tutorialenemy[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							tutorialenemy[j].flushing = TRUE;
							g_Bullet[i].use = FALSE;
							AddScore(100);

							// エフェクト発生
							SetEffect(tutorialenemy[j].pos.x, tutorialenemy[j].pos.y, 30);
							PlaySound(SOUND_LABEL_SE_EXP);
						}
					}
				}

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < BOSS_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (boss[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							boss[j].pos, boss[j].w, boss[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							g_Bullet[i].use = FALSE;
							AddScore(500);
							SubBosshp();

							// エフェクト発生
							SetEffect(enemy[j].pos.x, enemy[j].pos.y, 30);
							PlaySound(SOUND_LABEL_SE_EXP);
						}
					}
				}

				MAPCHIP* mc = GetMapChip();
				STAGE1* stage1 = GetStage1();
				STAGE2* stage2 = GetStage2();
				BOSSSTAGE* bossstage = GetBossStage();

				//チュートリアルステージ用の当たり判定
				for (int j = 0; j < MAPCHIP_MAX; j++)
				{
					if (mc[j].use = TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							mc[j].pos, mc[j].w + 2, mc[j].h - 2);
						if (ans == TRUE)
						{
							g_Bullet[i].use = FALSE;
						}

						ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							mc[j].pos, mc[j].w, mc[j].h);
						if (ans == TRUE)
						{
							switch (mc[j].ChipType)
							{
							case 0:
								break;

							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
							case 11:
							case 12:
								g_Bullet[i].use = FALSE;

								if (g_Bullet[i].pos.y < mc[j].pos.y)
									break;

							default:
								break;
							}
						}
					}
				}

				//ステージ１用の当たり判定
				for (int j = 0; j < STAGE1_MAX; j++)
				{
					int chiptype = stage1[j].ChipType;

					if (stage1[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							stage1[j].pos, stage1[j].w + 2, stage1[j].h - 2);
						if (ans == TRUE)
						{
							g_Bullet[i].use = FALSE;
						}

						ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							stage1[j].pos, stage1[j].w, stage1[j].h);
						if (ans == TRUE)
						{
							switch (stage1[j].ChipType)
							{
							case 0:
								break;

							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
							case 11:
							case 12:
							case 13:
								g_Bullet[i].use = FALSE;

								if (g_Bullet[i].pos.y < mc[j].pos.y)
									break;

							default:
								break;
							}
						}
					}
				}

				//ステージ２用の当たり判定
				for (int j = 0; j < STAGE2_MAX; j++)
				{
					int chiptype = stage2[j].ChipType;

					if (stage2[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							stage2[j].pos, stage2[j].w + 2, stage2[j].h - 2);
						if (ans == TRUE)
						{
							g_Bullet[i].use = FALSE;
						}

						ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							stage2[j].pos, stage2[j].w, stage2[j].h);
						if (ans == TRUE)
						{
							switch (stage2[j].ChipType)
							{
							case 0:
								break;

							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
							case 11:
							case 12:
							case 13:
								g_Bullet[i].use = FALSE;

								if (g_Bullet[i].pos.y < mc[j].pos.y)
									break;

							default:
								break;
							}
						}
					}
				}

				//チュートリアルステージ用の当たり判定
				for (int j = 0; j < BOSSSTAGE_MAX; j++)
				{
					if (bossstage[j].use = TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							bossstage[j].pos, bossstage[j].w + 2, bossstage[j].h - 2);
						if (ans == TRUE)
						{
							g_Bullet[i].use = FALSE;
						}

						ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							bossstage[j].pos, bossstage[j].w, bossstage[j].h);
						if (ans == TRUE)
						{
							switch (bossstage[j].ChipType)
							{
							case 0:
								break;

							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
							case 11:
							case 12:
								g_Bullet[i].use = FALSE;

								if (g_Bullet[i].pos.y < bossstage[j].pos.y)
									break;

							default:
								break;
							}
						}
					}
				}

			}


			bulletCount++;
		}
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Bullet[i].pos.x - bg->pos.x;	// バレットの表示位置X
			float py = g_Bullet[i].pos.y - bg->pos.y;	// バレットの表示位置Y
			float pw = g_Bullet[i].w;		// バレットの表示幅
			float ph = g_Bullet[i].h;		// バレットの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLET* GetBullet(void)
{
	return &g_Bullet[0];
}

int BulletCount(void)
{
	return bullet_n;
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetBullet(XMFLOAT3 pos, int dir)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Bullet[i].use = TRUE;			// 使用状態へ変更する
			g_Bullet[i].pos = pos;			// 座標をセット
			g_Bullet[i].dir = dir;
			bullet_n--;

			if (g_Bullet[i].dir == CHAR_DIR_RIGHT || g_Bullet[i].dir == CHAR_DIR_UP_R || g_Bullet[i].dir == CHAR_DIR_DOWN_R || g_Bullet[i].dir == CHAR_DIR_ATTACK_R)
			{
				g_Bullet[i].rot.z = 1.5708f;
				if (g_Bullet[i].move.x < 0)
				{
					g_Bullet[i].move.x *= -1;
				}
			}
			else if (g_Bullet[i].dir == CHAR_DIR_LEFT || g_Bullet[i].dir == CHAR_DIR_UP_L || g_Bullet[i].dir == CHAR_DIR_DOWN_L || g_Bullet[i].dir == CHAR_DIR_ATTACK_L)
			{
				g_Bullet[i].rot.z = -1.5708f;
				if (g_Bullet[i].move.x > 0)
				{
					g_Bullet[i].move.x *= -1;
				}
			}

			return;							// 1発セットしたので終了する
		}
	}
}

//void AddBullet(void)
//{
//	bullet_n++;
//
//	if (bullet_n > BULLET_MAX)
//	{
//		bullet_n = BULLET_MAX;
//	}
//}