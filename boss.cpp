//=============================================================================
//
// �G�l�~�[���� [enemy.cpp]
// Author : 
//
//=============================================================================
#include "boss.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "attack.h"
#include "life.h"
#include "magic.h"
#include "gun.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(500/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(500/2)	// 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(10)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define TEXTURE_PATTERN_DIVIDE_X_HP	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y_HP	(10)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM_HP			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT_HP				(10)		// �A�j���[�V�����̐؂�ւ��Wait�l

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/boss.png",
	"data/TEXTURE/HPber.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BOSS	g_Boss[BOSS_MAX];		// �G�l�~�[�\����

static int		g_BossCount = BOSS_MAX;

//static INTERPOLATION_DATA g_MoveTbl0[] = {
//	//���W									��]��							�g�嗦					����
//	{ XMFLOAT3(2800.0f, 1100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	100 },
//};
//
//
//static INTERPOLATION_DATA g_MoveTbl1[] = {
//	//���W									��]��							�g�嗦							����
//	{ XMFLOAT3(3500.0f,1000.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
//};
//
//
//static INTERPOLATION_DATA g_MoveTbl2[] = {
//	//���W									��]��							�g�嗦							����
//	{ XMFLOAT3(4000.0f,800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
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


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBoss(void)
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


	// �G�l�~�[�\���̂̏�����
	g_BossCount = 0;
	for (int i = 0; i < BOSS_MAX; i++)
	{
		g_BossCount++;
		g_Boss[i].use = TRUE;
		g_Boss[i].hp = 30;
		g_Boss[i].hp_max = 30;

		if (i % 3 == 0)
		{
			g_Boss[i].pos = XMFLOAT3(200.0f + i * 1000.0f, 100.0f, 0.0f);	// ���S�_����\��
		}
		else if (i % 3 == 1)
		{
			g_Boss[i].pos = XMFLOAT3(200.0f + i * 1000.0f, 500.0f, 0.0f);	// ���S�_����\��
		}
		else
		{
			g_Boss[i].pos = XMFLOAT3(200.0f + i * 1000.0f, 900.0f, 0.0f);	// ���S�_����\��
		}
		g_Boss[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Boss[i].w = TEXTURE_WIDTH;
		g_Boss[i].h = TEXTURE_HEIGHT;
		g_Boss[i].texNo = 0;
		g_Boss[i].attacktime = 0 + (46 * i);
		g_Boss[i].dir = BOSS_DIR_LEFT;

		g_Boss[i].hit = FALSE;
		g_Boss[i].hitCnt = 0;

		g_Boss[i].countAnim = 0;
		g_Boss[i].patternAnim = 0;

		g_Boss[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���

		g_Boss[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Boss[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Boss[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
	}

	//// 0�Ԃ������`��Ԃœ������Ă݂�
	//g_Boss[0].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	//g_Boss[0].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Boss[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	//// 1�Ԃ������`��Ԃœ������Ă݂�
	//g_Boss[1].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	//g_Boss[1].tblNo = 1;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Boss[1].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	//// 2�Ԃ������`��Ԃœ������Ă݂�
	//g_Boss[2].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	//g_Boss[2].tblNo = 2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Boss[2].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBoss(void)
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
void UpdateBoss(void)
{
	if (g_Load == FALSE) return;
	g_BossCount = 0;			// �����Ă�G�l�~�[�̐�

	for (int i = 0; i < BOSS_MAX; i++)
	{
		// �����Ă�G�l�~�[��������������
		if (g_Boss[i].use == TRUE)
		{
			g_BossCount++;		// �����Ă��G�̐�

			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Boss[i].pos;

			// �A�j���[�V����
			g_Boss[i].countAnim += 1.0f;
			if (g_Boss[i].countAnim > ANIM_WAIT)
			{
				g_Boss[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Boss[i].patternAnim = (g_Boss[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Boss[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
			}

			// �ړ�����
			PLAYER* player = GetPlayer();

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				if (g_Boss[i].pos.x <= player[j].pos.x)
				{
					g_Boss[i].dir = BOSS_DIR_RIGHT;
				}
				else
				{
					g_Boss[i].dir = BOSS_DIR_LEFT;
				}
			}

			float radian = atan2f(player[0].pos.y - g_Boss[i].pos.y, player[0].pos.x - g_Boss[i].pos.x);

			g_Boss[i].pos.x += 2 * cosf(radian);
			g_Boss[i].pos.y += 2 * sinf(radian);



			//if (g_Boss[i].tblMax > 0)	// ���`��Ԃ����s����H
			//{	// ���`��Ԃ̏���
			//	int nowNo = (int)g_Boss[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
			//	int maxNo = g_Boss[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
			//	int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
			//	INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Boss[i].tblNo];	// �s���e�[�u���̃A�h���X���擾
			//	
			//	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
			//	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
			//	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�
			//	
			//	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			//	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
			//	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���
			//	
			//	float nowTime = g_Boss[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���
			//	
			//	Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
			//	Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
			//	Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

			//	// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
			//	XMStoreFloat3(&g_Boss[i].pos, nowPos + Pos);

			//	// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
			//	XMStoreFloat3(&g_Boss[i].rot, nowRot + Rot);

			//	// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
			//	XMStoreFloat3(&g_Boss[i].scl, nowScl + Scl);
			//	g_Boss[i].w = TEXTURE_WIDTH * g_Boss[i].scl.x;
			//	g_Boss[i].h = TEXTURE_HEIGHT * g_Boss[i].scl.y;

			//	// frame���g�Ď��Ԍo�ߏ���������
			//	g_Boss[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
			//	if ((int)g_Boss[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			//	{
			//		g_Boss[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
			//	}

			//}


			g_Boss[i].attacktime++;
			if (g_Boss[i].attacktime % 100 == 0)
			{
				radian = atan2f(player[0].pos.y - g_Boss[i].pos.y, player[0].pos.x - g_Boss[i].pos.x);


				XMFLOAT3 pos = g_Boss[i].pos;

				pos.x += 100 * cosf(radian);
				pos.y += 100 * sinf(radian);

				for (int j = 0; j < PLAYER_MAX; j++)
				{
					SetAttack(pos);
				}
			}
			else if (g_Boss[i].attacktime % 220 == 0)
			{
				XMFLOAT3 pos = g_Boss[i].pos;

				for (int j = 0; j < PLAYER_MAX; j++)
				{
					if (player[j].pos.x < g_Boss[i].pos.x)
					{
						pos.x -= 300.0f;
						SetGun(pos, g_Boss[i].dir);
					}
					else
					{
						pos.x += 300.0f;
						SetGun(pos, g_Boss[i].dir);
					}
				}
			}
			else if (g_Boss[i].attacktime % 350 == 0)
			{

				for (int j = 0; j < PLAYER_MAX; j++)
				{
					XMFLOAT3 pos = player[j].pos;

					SetMagic(pos);
				}
			}


			// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
			{

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (player[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Boss[i].pos, g_Boss[i].w - 100, g_Boss[i].h - 100,
							player[j].pos, player[j].w, player[j].h);
						// �������Ă���H
						if (ans == TRUE && player[j].muteki == FALSE && g_Boss[i].hit == FALSE)
						{
							// �����������̏���
							SubLife(1);
							g_Boss[i].hit = TRUE;
						}
					}
				}
			}

			if (g_Boss[i].hit == TRUE)
			{
				g_Boss[i].hitCnt++;

				if (g_Boss[i].hitCnt >= 100)
				{
					g_Boss[i].hit = FALSE;
					g_Boss[i].hitCnt = 0;
				}
			}



			if (g_Boss[i].hp <= 0)
			{
				g_Boss[i].use = FALSE;
				SetFade(FADE_OUT, MODE_RESULT);
			}
		}
	}


	//// �G�l�~�[�S�Ń`�F�b�N
	//if (g_EnemyCount <= 0)
	//{
 //		SetFade(FADE_OUT, MODE_RESULT);
	//}

#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBoss(void)
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

	for (int i = 0; i < BOSS_MAX; i++)
	{
		if (g_Boss[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Boss[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Boss[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_Boss[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_Boss[i].w;		// �G�l�~�[�̕\����
			float ph = g_Boss[i].h;		// �G�l�~�[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Boss[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Boss[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			//float tw = 1.0f;	// �e�N�X�`���̕�
			//float th = 1.0f;	// �e�N�X�`���̍���
			//float tx = 0.0f;	// �e�N�X�`���̍���X���W
			//float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Boss[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// �Q�[�W�̃e�X�g
	{
		// ���~���̃Q�[�W�i�g�I�ȕ��j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 650.0f;		// �Q�[�W�̕\���ʒuX
		float py =  10.0f;		// �Q�[�W�̕\���ʒuY
		float pw = 300.0f;		// �Q�[�W�̕\����
		float ph =  30.0f;		// �Q�[�W�̕\������

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_HP;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = (float)((g_Boss[0].hp_max - g_Boss[0].hp) % TEXTURE_PATTERN_DIVIDE_Y_HP) * th;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		//// �G�l�~�[�̐��ɏ]���ăQ�[�W�̒�����\�����Ă݂�
		//// �e�N�X�`���ݒ�
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		////�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		//pw = pw * ((float)g_Boss[0].hp / g_Boss[0].hp_max);

		//// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//SetSpriteLTColor(g_VertexBuffer,
		//	px, py, pw, ph,
		//	tx, ty, tw, th,
		//	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

		//// �|���S���`��
		//GetDeviceContext()->Draw(4, 0);


	}




}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
BOSS* GetBoss(void)
{
	return &g_Boss[0];
}


// �����Ă�G�l�~�[�̐�
int GetBossCount(void)
{
	return g_BossCount;
}

void SubBosshp(void)
{
	for (int i = 0; i < BOSS_MAX; i++)
	{
		g_Boss[i].hp--;
	}
}
