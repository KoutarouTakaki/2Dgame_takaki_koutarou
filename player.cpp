//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "player.h"
#include "input.h"
#include "bg.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "file.h"
#include "life.h"
#include "mapchip.h"
#include "attack.h"
#include "gun.h"
#include "magic.h"
#include "fade.h"
#include "main.h"
#include "stage1.h"
#include "stage2.h"
#include "gameover.h"
#include "bossstage.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(200/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(200/2)	// 
#define TEXTURE_MAX					(3)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(6)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(8)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

// プレイヤーの画面内配置座標
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

// ジャンプ処理
#define	PLAYER_JUMP_CNT_MAX			(30)		// 30フレームで着地する
#define	PLAYER_JUMP_Y_MAX			(300.0f)	// ジャンプの高さ


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/char01.png",
	"data/TEXTURE/shadow000.jpg",
	"data/TEXTURE/main_player.png"
};


static BOOL		g_Load = FALSE;				// 初期化を行ったかのフラグ
static PLAYER	g_Player[PLAYER_MAX];		// プレイヤー構造体
static int		g_PlayerCount = PLAYER_MAX;	// 生きてるプレイヤーの数

static int      g_jumpCnt = 0;
static int		g_jump[PLAYER_JUMP_CNT_MAX] =
{
	-15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5,-4,-3,-2,-1,
	  1,   2,   3,   4,   5,   6,  7,  8,  9, 10, 11,12,13,14,15
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
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

	g_PlayerCount = 0;						// 生きてるプレイヤーの数

	MAPCHIP* mc = GetMapChip();
	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_PlayerCount++;
		g_Player[i].use = TRUE;
		g_Player[i].pos = XMFLOAT3(200.0f, 1000.0f, 0.0f);	// 中心点から表示
		g_Player[i].old_pos = XMFLOAT3(200.0f, 1000.0f, 0.0f);	// 中心点から表示
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 2;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;

		g_Player[i].move = XMFLOAT3(7.0f, 0.0f, 0.0f);		// 移動量

		g_Player[i].dir = CHAR_DIR_DOWN_R;// 下向きにしとくか
		g_Player[i].dir_old = CHAR_DIR_DOWN_R;
		g_Player[i].dirBullet = 0;
		g_Player[i].dirBullet_old = 0;
		g_Player[i].moving = FALSE;							// 移動中フラグ
		g_Player[i].patternAnim = g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X;
		g_Player[i].muteki = FALSE;
		g_Player[i].mutekiCount = 0;

		g_Player[i].flushing = FALSE;
		g_Player[i].flushingCount = 0;
		g_Player[i].visible = TRUE;

		// ジャンプの初期化
		g_Player[i].jump = FALSE;
		g_Player[i].jumpCnt = 1;
		g_Player[i].jumpY = 0.0f;
		g_Player[i].jumpYMax = PLAYER_JUMP_Y_MAX;

		g_Player[i].atk_use = FALSE;
		g_Player[i].atkCnt = 0;

		// 分身用
		g_Player[i].dash = FALSE;
		g_Player[i].dashCount = 0;
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{
	if (g_Load == FALSE) return;
	g_PlayerCount = 0;				// 生きてるプレイヤーの数

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (g_Player[i].use == TRUE)
		{
			g_PlayerCount++;		// 生きてるプレイヤーの数

			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Player[i].pos;
			g_Player[i].old_pos = g_Player[i].pos;
			// 分身用
			for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j - 1];
			}
			g_Player[i].offset[0] = pos_old;

			g_Player[i].moving = FALSE;

			// アニメーション
			g_Player[i].countAnim += 1.0f;
			if (g_Player[i].countAnim > ANIM_WAIT)
			{
				g_Player[i].countAnim = 0.0f;
				// パターンの切り替え
				g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
			}


			// キー入力で移動 
			{
				float speed = g_Player[i].move.x;

				if (GetKeyboardTrigger(DIK_K) || IsButtonPressed(0, BUTTON_L))
				{
					ATTACK* attack = GetAttack();
					GUN* gun = GetGun();
					MAGIC* magic = GetMagic();

					//近接攻撃用のダッシュ判定
					for (int j = 0; j < ATTACK_MAX; j++)
					{
						if (attack[j].use == TRUE) {
							BOOL ans = CollisionBB(g_Player[i].pos, g_Player[i].w, g_Player[i].h,
								attack[j].pos, attack[j].w, attack[j].h);

							if (ans == TRUE)
							{
								g_Player[i].dash = TRUE;
								g_Player[i].muteki = TRUE;
								AddScore(100);
								PlaySound(SOUND_LABEL_SE_DASH);
								if (GetRand(0, 100) < 50)
								{
									AddLife(1);
								}
							}
						}
					}

					//遠距離攻撃用のダッシュ判定
					for (int j = 0; j < GUN_MAX; j++)
					{
						if (gun[j].use == TRUE) {
							BOOL ans = CollisionBB(g_Player[i].pos, g_Player[i].w, g_Player[i].h,
								gun[j].pos, gun[j].w, gun[j].h);

							if (ans == TRUE)
							{
								g_Player[i].dash = TRUE;
								g_Player[i].muteki = TRUE;
								AddScore(100);
								PlaySound(SOUND_LABEL_SE_DASH);
								if (GetRand(0, 100) < 50)
								{
									AddLife(1);
								}
							}
						}
					}

					//魔法攻撃用のダッシュ判定
					for (int j = 0; j < MAGIC_MAX; j++)
					{
						if (magic[j].use == TRUE) {
							BOOL ans = CollisionBB(g_Player[i].pos, g_Player[i].w, g_Player[i].h,
								magic[j].pos, magic[j].w, magic[j].h);

							if (ans == TRUE)
							{
								g_Player[i].dash = TRUE;
								g_Player[i].muteki = TRUE;
								AddScore(100);
								PlaySound(SOUND_LABEL_SE_DASH);
								if (GetRand(0, 100) < 50)
								{
									AddLife(1);
								}
							}
						}
					}
				}


				//if ((GetKeyboardPress(DIK_S)) && (g_Player[i].moving == FALSE))
				//{

				//	if (g_jumpCnt == 0)
				//	{
				//		//g_Player[i].h = TEXTURE_HEIGHT/2;
				//	}
				//	else
				//	{
				//		g_Player[i].dir = CHAR_DIR_DOWN;
				//		g_Player[i].dirBullet = 3.1415f;
				//	}
				//}
				//else if ((GetKeyboardPress(DIK_W)) && (g_Player[i].moving == FALSE))
				//{
				//	g_Player[i].dir = CHAR_DIR_UP;
				//	g_Player[i].dirBullet = 0.0f;
				//	g_Player[i].h = TEXTURE_HEIGHT;
				//}


				if (g_Player[i].moving == FALSE || g_Player[i].atk_use == FALSE)
				{
					if (g_Player[i].dir == CHAR_DIR_RIGHT || g_Player[i].dir == CHAR_DIR_UP_R || g_Player[i].dir == CHAR_DIR_ATTACK_R)
					{
						g_Player[i].dir = CHAR_DIR_DOWN_R;
					}
					else if (g_Player[i].dir == CHAR_DIR_LEFT || g_Player[i].dir == CHAR_DIR_UP_L || g_Player[i].dir == CHAR_DIR_ATTACK_L)
					{
						g_Player[i].dir = CHAR_DIR_DOWN_L;
					}
				}


				//右移動
				if (GetKeyboardPress(DIK_D) || GetKeyboardPress(DIK_RIGHTARROW) || IsButtonPressed(0, BUTTON_RIGHT))
				{
					g_Player[i].pos.x += speed;
					g_Player[i].dir = CHAR_DIR_RIGHT;
					g_Player[i].dir_old = g_Player[i].dir;
					g_Player[i].moving = TRUE;

				}
				//左移動
				else if (GetKeyboardPress(DIK_A) || GetKeyboardPress(DIK_LEFTARROW) || IsButtonPressed(0, BUTTON_LEFT))
				{
					g_Player[i].pos.x -= speed;
					g_Player[i].dir = CHAR_DIR_LEFT;
					g_Player[i].dir_old = g_Player[i].dir;
					g_Player[i].moving = TRUE;

				}

				if (g_Player[i].move.y < 20.0f)
					g_Player[i].move.y += 0.6f;

				g_Player[i].pos.y += g_Player[i].move.y;

				if ((g_jumpCnt <= 6) && (GetKeyboardPress(DIK_W)) || (g_jumpCnt <= 6) && (GetKeyboardPress(DIK_UPARROW)) || (g_jumpCnt <= 6) && IsButtonPressed(0, BUTTON_A) || (g_jumpCnt <= 6) && (GetKeyboardPress(DIK_SPACE)))
				{
					if (g_jumpCnt % 2 == 0)
						g_Player[i].move.y -= 6.0f - (g_jumpCnt / 2.3f);

					g_Player[i].pos.y += g_Player[i].move.y;
					g_jumpCnt++;

				}

				if (g_jumpCnt != 0)
				{
					g_Player[i].moving = TRUE;

					if (g_Player[i].dir == CHAR_DIR_RIGHT || g_Player[i].dir == CHAR_DIR_DOWN_R)
					{
						g_Player[i].dir = CHAR_DIR_UP_R;
					}
					else if (g_Player[i].dir == CHAR_DIR_LEFT || g_Player[i].dir == CHAR_DIR_DOWN_L)
					{
						g_Player[i].dir = CHAR_DIR_UP_L;
					}
				}



				if (g_Player[i].dash == TRUE)
				{
					if (g_Player[i].dir == CHAR_DIR_RIGHT || g_Player[i].dir == CHAR_DIR_UP_R || g_Player[i].dir == CHAR_DIR_DOWN_R || g_Player[i].dir == CHAR_DIR_ATTACK_R)
					{
						g_Player[i].pos.x += 20;
					}
					else if (g_Player[i].dir == CHAR_DIR_LEFT || g_Player[i].dir == CHAR_DIR_UP_L || g_Player[i].dir == CHAR_DIR_DOWN_L || g_Player[i].dir == CHAR_DIR_ATTACK_L)
					{
						g_Player[i].pos.x -= 20;
					}
					g_Player[i].dashCount++;

					if (g_Player[i].dashCount >= 10)
					{
						g_Player[i].dash = FALSE;
						g_Player[i].dashCount = 0;
					}
				}

				if (g_Player[i].dash == TRUE || g_Player[i].muteki== TRUE)
				{
					g_Player[i].mutekiCount++;

					if (g_Player[i].mutekiCount >= 50)
					{
						g_Player[i].muteki = FALSE;
						g_Player[i].mutekiCount = 0;
					}
				}

				if (g_Player[i].flushing == TRUE)
				{
					g_Player[i].flushingCount++;

					if (g_Player[i].flushingCount % 9 < 5)
					{
						g_Player[i].visible = TRUE;
					}
					else
					{
						g_Player[i].visible = FALSE;
					}


					if (g_Player[i].flushingCount >= 50)
					{
						g_Player[i].visible = TRUE;
						g_Player[i].flushingCount = 0;
						g_Player[i].flushing = FALSE;
					}
				}








				// MAP外チェック
				BG* bg = GetBG();

				if (g_Player[i].pos.x < 0.0f)
				{
					g_Player[i].pos.x = 0.0f;
				}

				if (g_Player[i].pos.x > bg->w)
				{
					g_Player[i].pos.x = bg->w;
				}

				if (g_Player[i].pos.y < 0.0f)
				{
					g_Player[i].pos.y = 0.0f;
				}

				if (g_Player[i].pos.y > bg->h)
				{
					//g_Player[i].pos.y = bg->h;

					SetOldMode();
					SetFade(FADE_OUT, MODE_GAMEOVER);
				}


				int mode = GetMode();
				MAPCHIP* mc = GetMapChip();
				STAGE1* stage1 = GetStage1();
				STAGE2* stage2 = GetStage2();
				BOSSSTAGE* bossstage = GetBossStage();


				switch (mode)
				{
				case MODE_TUTORIAL:
					//チュートリアルステージ用の当たり判定
					for (int j = 0; j < MAPCHIP_MAX; j++)
					{
						int chiptype = mc[j].ChipType;

						if (mc[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Player[i].pos, g_Player[i].w - 60, g_Player[i].h - 15,
								mc[j].pos, mc[j].w + 2, mc[j].h - 2);
							if (ans == TRUE)
							{
								g_Player[i].pos.x = pos_old.x;
							}

							ans = CollisionBB(g_Player[i].pos, g_Player[i].w - 60, g_Player[i].h - 15,
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
								case 13:
									g_Player[i].pos.y = pos_old.y;
									g_Player[i].move.y = 0.0f;

									if (g_Player[i].pos.y < mc[j].pos.y)
										g_jumpCnt = 0;
									break;

								default:
									break;
								}
							}
						}
					}
					break;

				case MODE_STAGE1:
					//ステージ１用の当たり判定
					for (int j = 0; j < STAGE1_MAX; j++)
					{
						int chiptype = stage1[j].ChipType;

						if (stage1[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Player[i].pos, g_Player[i].w - 60, g_Player[i].h - 15,
								stage1[j].pos, stage1[j].w + 2, stage1[j].h - 2);
							if (ans == TRUE)
							{
								g_Player[i].pos.x = pos_old.x;
							}

							ans = CollisionBB(g_Player[i].pos, g_Player[i].w - 60, g_Player[i].h - 15,
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
									g_Player[i].pos.y = pos_old.y;
									g_Player[i].move.y = 0.0f;

									if (g_Player[i].pos.y < stage1[j].pos.y)
										g_jumpCnt = 0;
									break;

								default:
									break;
								}
							}
						}
					}
					break;

				case MODE_STAGE2:
					//ステージ２用の当たり判定
					for (int j = 0; j < STAGE2_MAX; j++)
					{
						int chiptype = stage2[j].ChipType;

						if (stage2[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Player[i].pos, g_Player[i].w - 60, g_Player[i].h - 15,
								stage2[j].pos, stage2[j].w + 2, stage2[j].h - 2);
							if (ans == TRUE)
							{
								g_Player[i].pos.x = pos_old.x;
							}

							ans = CollisionBB(g_Player[i].pos, g_Player[i].w - 60, g_Player[i].h - 15,
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
									g_Player[i].pos.y = pos_old.y;
									g_Player[i].move.y = 0.0f;

									if (g_Player[i].pos.y < stage2[j].pos.y)
										g_jumpCnt = 0;
									break;

								default:
									break;
								}
							}
						}
					}
					break;

				case MODE_BOSS:
					//ボスステージ用の当たり判定
					for (int j = 0; j < BOSSSTAGE_MAX; j++)
					{
						int chiptype = bossstage[j].ChipType;

						if (bossstage[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Player[i].pos, g_Player[i].w - 60, g_Player[i].h - 15,
								bossstage[j].pos, bossstage[j].w + 2, bossstage[j].h - 2);
							if (ans == TRUE)
							{
								g_Player[i].pos.x = pos_old.x;
							}

							ans = CollisionBB(g_Player[i].pos, g_Player[i].w - 60, g_Player[i].h - 15,
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
								case 13:
									g_Player[i].pos.y = pos_old.y;
									g_Player[i].move.y = 0.0f;

									if (g_Player[i].pos.y < bossstage[j].pos.y)
										g_jumpCnt = 0;
									break;

								default:
									break;
								}
							}
						}
					}
					break;

				default:
					break;
				}

				// プレイヤーの立ち位置からMAPのスクロール座標を計算する
				bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;


				// バレット処理
				if (GetKeyboardTrigger(DIK_J) || IsButtonTriggered(0, BUTTON_R))
				{
					XMFLOAT3 pos = g_Player[i].pos;
					pos.y += g_Player[i].jumpY;
					SetBullet(pos, g_Player[i].dir);
					PlaySound(SOUND_LABEL_SE_SHOT);

					g_Player[i].atk_use = TRUE;
				}

				if (g_Player[i].atk_use == TRUE)
				{
					if (g_Player[i].atkCnt < 20)
					{
						if (g_Player[i].dir == CHAR_DIR_UP_R || g_Player[i].dir == CHAR_DIR_DOWN_R || g_Player[i].dir == CHAR_DIR_RIGHT)
						{
							g_Player[i].dir = CHAR_DIR_ATTACK_R;
						}
						if (g_Player[i].dir == CHAR_DIR_UP_L || g_Player[i].dir == CHAR_DIR_DOWN_L || g_Player[i].dir == CHAR_DIR_LEFT)
						{
							g_Player[i].dir = CHAR_DIR_ATTACK_L;
						}
					}
					else
					{
						g_Player[i].atkCnt = 0;
						g_Player[i].atk_use = FALSE;
					}
					g_Player[i].atkCnt++;
				}


				//// バレット処理
				//if (GetKeyboardTrigger(DIK_J))
				//{
				//	XMFLOAT3 pos = g_Player[i].pos;
				//	pos.x += 100;//g_Player[i].jumpY;
				//	SetAttack(pos);
				//}

				//if (IsButtonTriggered(0, BUTTON_B))
				//{
				//	XMFLOAT3 pos = g_Player[i].pos;
				//	pos.y += g_Player[i].jumpY;
				//	SetBullet(pos);
				//}

			}
		}
	}


	// 現状をセーブする
	if (GetKeyboardTrigger(DIK_S))
	{
		SaveData();
	}


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// このプレイヤーが使われている？
		{									// Yes

			if (g_Player[i].visible == FALSE) continue;
			//{	// 影表示
			//	SetBlendState(BLEND_MODE_SUBTRACT);	// 減算合成

			//	// テクスチャ設定
			//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			//	float px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
			//	float py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
			//	float pw = g_Player[i].w;		// プレイヤーの表示幅
			//	float ph = g_Player[i].h/4;		// プレイヤーの表示高さ
			//	py += 50.0f;		// 足元に表示

			//	float tw = 1.0f;	// テクスチャの幅
			//	float th = 1.0f;	// テクスチャの高さ
			//	float tx = 0.0f;	// テクスチャの左上X座標
			//	float ty = 0.0f;	// テクスチャの左上Y座標

			//	// １枚のポリゴンの頂点とテクスチャ座標を設定
			//	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			//		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			//	// ポリゴン描画
			//	GetDeviceContext()->Draw(4, 0);

			//	SetBlendState(BLEND_MODE_ALPHABLEND);	// 半透明処理を元に戻す

			//}

			// プレイヤーの分身を描画
			if (g_Player[i].dash)
			{	// ダッシュ中だけ分身処理
				DrawPlayerOffset(i);
			}

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
			float py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
			float pw = g_Player[i].w;		// プレイヤーの表示幅
			float ph = g_Player[i].h;		// プレイヤーの表示高さ

			py += g_Player[i].jumpY;		// ジャンプ中の高さを足す

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			//float tw = 1.0f;	// テクスチャの幅
			//float th = 1.0f;	// テクスチャの高さ
			//float tx = 0.0f;	// テクスチャの左上X座標
			//float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Player[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}
	}


}


//=============================================================================
// Player構造体の先頭アドレスを取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


// 生きてるエネミーの数
int GetPlayerCount(void)
{
	return g_PlayerCount;
}


//=============================================================================
// プレイヤーの分身を描画
//=============================================================================
void DrawPlayerOffset(int no)
{
	BG* bg = GetBG();
	float alpha = 0.0f;

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].texNo]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Player[no].offset[j].x - bg->pos.x;	// プレイヤーの表示位置X
		float py = g_Player[no].offset[j].y - bg->pos.y;	// プレイヤーの表示位置Y
		float pw = g_Player[no].w;		// プレイヤーの表示幅
		float ph = g_Player[no].h;		// プレイヤーの表示高さ

		// アニメーション用
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
		float tx = (float)(g_Player[no].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
		float ty = (float)(g_Player[no].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}



