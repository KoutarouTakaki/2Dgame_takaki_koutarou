//=============================================================================
//
// �v���C���[���� [player.cpp]
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(200/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(200/2)	// 
#define TEXTURE_MAX					(3)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(6)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(8)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l

// �v���C���[�̉�ʓ��z�u���W
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

// �W�����v����
#define	PLAYER_JUMP_CNT_MAX			(30)		// 30�t���[���Œ��n����
#define	PLAYER_JUMP_Y_MAX			(300.0f)	// �W�����v�̍���


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/char01.png",
	"data/TEXTURE/shadow000.jpg",
	"data/TEXTURE/main_player.png"
};


static BOOL		g_Load = FALSE;				// ���������s�������̃t���O
static PLAYER	g_Player[PLAYER_MAX];		// �v���C���[�\����
static int		g_PlayerCount = PLAYER_MAX;	// �����Ă�v���C���[�̐�

static int      g_jumpCnt = 0;
static int		g_jump[PLAYER_JUMP_CNT_MAX] =
{
	-15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5,-4,-3,-2,-1,
	  1,   2,   3,   4,   5,   6,  7,  8,  9, 10, 11,12,13,14,15
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	g_PlayerCount = 0;						// �����Ă�v���C���[�̐�

	MAPCHIP* mc = GetMapChip();
	// �v���C���[�\���̂̏�����
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_PlayerCount++;
		g_Player[i].use = TRUE;
		g_Player[i].pos = XMFLOAT3(200.0f, 1000.0f, 0.0f);	// ���S�_����\��
		g_Player[i].old_pos = XMFLOAT3(200.0f, 1000.0f, 0.0f);	// ���S�_����\��
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 2;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;

		g_Player[i].move = XMFLOAT3(7.0f, 0.0f, 0.0f);		// �ړ���

		g_Player[i].dir = CHAR_DIR_DOWN_R;// �������ɂ��Ƃ���
		g_Player[i].dir_old = CHAR_DIR_DOWN_R;
		g_Player[i].dirBullet = 0;
		g_Player[i].dirBullet_old = 0;
		g_Player[i].moving = FALSE;							// �ړ����t���O
		g_Player[i].patternAnim = g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X;
		g_Player[i].muteki = FALSE;
		g_Player[i].mutekiCount = 0;

		g_Player[i].flushing = FALSE;
		g_Player[i].flushingCount = 0;
		g_Player[i].visible = TRUE;

		// �W�����v�̏�����
		g_Player[i].jump = FALSE;
		g_Player[i].jumpCnt = 1;
		g_Player[i].jumpY = 0.0f;
		g_Player[i].jumpYMax = PLAYER_JUMP_Y_MAX;

		g_Player[i].atk_use = FALSE;
		g_Player[i].atkCnt = 0;

		// ���g�p
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
// �I������
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
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	if (g_Load == FALSE) return;
	g_PlayerCount = 0;				// �����Ă�v���C���[�̐�

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// �����Ă�v���C���[��������������
		if (g_Player[i].use == TRUE)
		{
			g_PlayerCount++;		// �����Ă�v���C���[�̐�

			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Player[i].pos;
			g_Player[i].old_pos = g_Player[i].pos;
			// ���g�p
			for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j - 1];
			}
			g_Player[i].offset[0] = pos_old;

			g_Player[i].moving = FALSE;

			// �A�j���[�V����
			g_Player[i].countAnim += 1.0f;
			if (g_Player[i].countAnim > ANIM_WAIT)
			{
				g_Player[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
			}


			// �L�[���͂ňړ� 
			{
				float speed = g_Player[i].move.x;

				if (GetKeyboardTrigger(DIK_K) || IsButtonPressed(0, BUTTON_L))
				{
					ATTACK* attack = GetAttack();
					GUN* gun = GetGun();
					MAGIC* magic = GetMagic();

					//�ߐڍU���p�̃_�b�V������
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

					//�������U���p�̃_�b�V������
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

					//���@�U���p�̃_�b�V������
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


				//�E�ړ�
				if (GetKeyboardPress(DIK_D) || GetKeyboardPress(DIK_RIGHTARROW) || IsButtonPressed(0, BUTTON_RIGHT))
				{
					g_Player[i].pos.x += speed;
					g_Player[i].dir = CHAR_DIR_RIGHT;
					g_Player[i].dir_old = g_Player[i].dir;
					g_Player[i].moving = TRUE;

				}
				//���ړ�
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








				// MAP�O�`�F�b�N
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
					//�`���[�g���A���X�e�[�W�p�̓����蔻��
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
					//�X�e�[�W�P�p�̓����蔻��
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
					//�X�e�[�W�Q�p�̓����蔻��
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
					//�{�X�X�e�[�W�p�̓����蔻��
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

				// �v���C���[�̗����ʒu����MAP�̃X�N���[�����W���v�Z����
				bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;


				// �o���b�g����
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


				//// �o���b�g����
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


	// ������Z�[�u����
	if (GetKeyboardTrigger(DIK_S))
	{
		SaveData();
	}


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// ���̃v���C���[���g���Ă���H
		{									// Yes

			if (g_Player[i].visible == FALSE) continue;
			//{	// �e�\��
			//	SetBlendState(BLEND_MODE_SUBTRACT);	// ���Z����

			//	// �e�N�X�`���ݒ�
			//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			//	float px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
			//	float py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
			//	float pw = g_Player[i].w;		// �v���C���[�̕\����
			//	float ph = g_Player[i].h/4;		// �v���C���[�̕\������
			//	py += 50.0f;		// �����ɕ\��

			//	float tw = 1.0f;	// �e�N�X�`���̕�
			//	float th = 1.0f;	// �e�N�X�`���̍���
			//	float tx = 0.0f;	// �e�N�X�`���̍���X���W
			//	float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			//	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			//	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			//		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			//	// �|���S���`��
			//	GetDeviceContext()->Draw(4, 0);

			//	SetBlendState(BLEND_MODE_ALPHABLEND);	// ���������������ɖ߂�

			//}

			// �v���C���[�̕��g��`��
			if (g_Player[i].dash)
			{	// �_�b�V�����������g����
				DrawPlayerOffset(i);
			}

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
			float py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
			float pw = g_Player[i].w;		// �v���C���[�̕\����
			float ph = g_Player[i].h;		// �v���C���[�̕\������

			py += g_Player[i].jumpY;		// �W�����v���̍����𑫂�

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			//float tw = 1.0f;	// �e�N�X�`���̕�
			//float th = 1.0f;	// �e�N�X�`���̍���
			//float tx = 0.0f;	// �e�N�X�`���̍���X���W
			//float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Player[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}
	}


}


//=============================================================================
// Player�\���̂̐擪�A�h���X���擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


// �����Ă�G�l�~�[�̐�
int GetPlayerCount(void)
{
	return g_PlayerCount;
}


//=============================================================================
// �v���C���[�̕��g��`��
//=============================================================================
void DrawPlayerOffset(int no)
{
	BG* bg = GetBG();
	float alpha = 0.0f;

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].texNo]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Player[no].offset[j].x - bg->pos.x;	// �v���C���[�̕\���ʒuX
		float py = g_Player[no].offset[j].y - bg->pos.y;	// �v���C���[�̕\���ʒuY
		float pw = g_Player[no].w;		// �v���C���[�̕\����
		float ph = g_Player[no].h;		// �v���C���[�̕\������

		// �A�j���[�V�����p
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
		float tx = (float)(g_Player[no].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
		float ty = (float)(g_Player[no].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}



