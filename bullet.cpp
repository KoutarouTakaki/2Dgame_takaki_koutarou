//=============================================================================
//
// �o���b�g���� [bullet.cpp]
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(100/2)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[] = {
	"data/TEXTURE/bullet.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BULLET	g_Bullet[BULLET_MAX];	// �o���b�g�\����


int bullet_n;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
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

	bullet_n = 10;

	// �o���b�g�\���̂̏�����
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
		g_Bullet[i].w = TEXTURE_WIDTH;
		g_Bullet[i].h = TEXTURE_HEIGHT;
		g_Bullet[i].pos = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo = 0;
		g_Bullet[i].dir = CHAR_DIR_RIGHT;

		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move = XMFLOAT3(15.0f, 0.0f, 0.0f);	// �ړ��ʂ�������
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateBullet(void)
{
	if (g_Load == FALSE) return;
	int bulletCount = 0;				// ���������o���b�g�̐�

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes

			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Bullet[i].pos;

			// �A�j���[�V����  
			g_Bullet[i].countAnim++;
			if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// �o���b�g�̈ړ�����
			XMVECTOR pos = XMLoadFloat3(&g_Bullet[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_Bullet[i].move);
			pos += move;
			XMStoreFloat3(&g_Bullet[i].pos, pos);

			// ��ʊO�܂Ői�񂾁H
			BG* bg = GetBG();
			if (g_Bullet[i].pos.y < (-g_Bullet[i].h / 2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Bullet[i].use = false;
			}
			if (g_Bullet[i].pos.y > (bg->h + g_Bullet[i].h / 2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Bullet[i].use = false;
			}

			// �����蔻�菈��
			{
				ENEMY* enemy = GetEnemy();
				GUNENEMY* gunenemy = GetGunenemy();
				MAGICENEMY* magicenemy = GetMagicenemy();
				TUTORIALENEMY* tutorialenemy = GetTutorialEnemy();
				BOSS* boss = GetBoss();

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (enemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							enemy[j].pos, enemy[j].w, enemy[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							enemy[j].flushing = TRUE;
							g_Bullet[i].use = FALSE;
							AddScore(100);

							// �G�t�F�N�g����
							SetEffect(enemy[j].pos.x, enemy[j].pos.y, 30);
						}
					}
				}

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < GUNENEMY_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (gunenemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							gunenemy[j].pos, gunenemy[j].w, gunenemy[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							gunenemy[j].flushing = TRUE;
							g_Bullet[i].use = FALSE;
							AddScore(100);

							// �G�t�F�N�g����
							SetEffect(gunenemy[j].pos.x, gunenemy[j].pos.y, 30);
							PlaySound(SOUND_LABEL_SE_EXP);
						}
					}
				}

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < MAGICENEMY_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (magicenemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							magicenemy[j].pos, magicenemy[j].w, magicenemy[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							magicenemy[j].flushing = TRUE;
							g_Bullet[i].use = FALSE;
							AddScore(100);

							// �G�t�F�N�g����
							SetEffect(magicenemy[j].pos.x, magicenemy[j].pos.y, 30);
							PlaySound(SOUND_LABEL_SE_EXP);
						}
					}
				}

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < TUTORIALENEMY_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (tutorialenemy[j].use == TRUE && j != 2)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							tutorialenemy[j].pos, tutorialenemy[j].w, tutorialenemy[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							tutorialenemy[j].flushing = TRUE;
							g_Bullet[i].use = FALSE;
							AddScore(100);

							// �G�t�F�N�g����
							SetEffect(tutorialenemy[j].pos.x, tutorialenemy[j].pos.y, 30);
							PlaySound(SOUND_LABEL_SE_EXP);
						}
					}
				}

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < BOSS_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (boss[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							boss[j].pos, boss[j].w, boss[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							g_Bullet[i].use = FALSE;
							AddScore(500);
							SubBosshp();

							// �G�t�F�N�g����
							SetEffect(enemy[j].pos.x, enemy[j].pos.y, 30);
							PlaySound(SOUND_LABEL_SE_EXP);
						}
					}
				}

				MAPCHIP* mc = GetMapChip();
				STAGE1* stage1 = GetStage1();
				STAGE2* stage2 = GetStage2();
				BOSSSTAGE* bossstage = GetBossStage();

				//�`���[�g���A���X�e�[�W�p�̓����蔻��
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

				//�X�e�[�W�P�p�̓����蔻��
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

				//�X�e�[�W�Q�p�̓����蔻��
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

				//�`���[�g���A���X�e�[�W�p�̓����蔻��
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
// �`�揈��
//=============================================================================
void DrawBullet(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Bullet[i].pos.x - bg->pos.x;	// �o���b�g�̕\���ʒuX
			float py = g_Bullet[i].pos.y - bg->pos.y;	// �o���b�g�̕\���ʒuY
			float pw = g_Bullet[i].w;		// �o���b�g�̕\����
			float ph = g_Bullet[i].h;		// �o���b�g�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
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
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetBullet(XMFLOAT3 pos, int dir)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Bullet[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_Bullet[i].pos = pos;			// ���W���Z�b�g
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

			return;							// 1���Z�b�g�����̂ŏI������
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