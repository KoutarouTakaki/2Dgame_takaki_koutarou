//=============================================================================
//
// �G�l�~�[���� [enemy.cpp]
// Author : 
//
//=============================================================================
#include "enemy.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "attack.h"
#include "life.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(150/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(150/2)	// 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(16)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(10)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enemy_textrue.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ENEMY	g_Enemy[ENEMY_MAX];		// �G�l�~�[�\����

static int		g_EnemyCount = ENEMY_MAX;

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

int kazu;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(int min, int max)
{
	kazu = max - min;

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
	g_EnemyCount = 0;
	for (int i = 0; i <= kazu; i++)
	{
		g_EnemyCount++;
		g_Enemy[i].use = TRUE;
		if (i % 3 == 0)
		{
			g_Enemy[i].pos = XMFLOAT3(200.0f + i * 1000.0f, 100.0f, 0.0f);	// ���S�_����\��
		}
		else if (i % 3 == 1)
		{
			g_Enemy[i].pos = XMFLOAT3(200.0f + i * 1000.0f, 500.0f, 0.0f);	// ���S�_����\��
		}
		else
		{
			g_Enemy[i].pos = XMFLOAT3(200.0f + i * 1000.0f, 900.0f, 0.0f);	// ���S�_����\��
		}
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].w = TEXTURE_WIDTH;
		g_Enemy[i].h = TEXTURE_HEIGHT;
		g_Enemy[i].texNo = 0;
		g_Enemy[i].attacktime = 0 + (46 * i);
		g_Enemy[i].dir = ENEMY_DIR_LEFT;

		g_Enemy[i].flushing = FALSE;
		g_Enemy[i].flushingCount = 0;
		g_Enemy[i].visible = TRUE;

		g_Enemy[i].countAnim = 0;
		g_Enemy[i].patternAnim = 0;

		g_Enemy[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���

		g_Enemy[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Enemy[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
	}

	//// 0�Ԃ������`��Ԃœ������Ă݂�
	//g_Enemy[0].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	//g_Enemy[0].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Enemy[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	//// 1�Ԃ������`��Ԃœ������Ă݂�
	//g_Enemy[1].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	//g_Enemy[1].tblNo = 1;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Enemy[1].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	//// 2�Ԃ������`��Ԃœ������Ă݂�
	//g_Enemy[2].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	//g_Enemy[2].tblNo = 2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Enemy[2].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{
	if (g_Load == FALSE) return;
	g_EnemyCount = 0;			// �����Ă�G�l�~�[�̐�

	for (int i = 0; i <= kazu; i++)
	{
		// �����Ă�G�l�~�[��������������
		if (g_Enemy[i].use == TRUE)
		{
			g_EnemyCount++;		// �����Ă��G�̐�

			if (g_Enemy[i].flushing == TRUE)
			{
				g_Enemy[i].flushingCount++;

				if (g_Enemy[i].flushingCount % 9 < 5)
				{
					g_Enemy[i].visible = TRUE;
				}
				else
				{
					g_Enemy[i].visible = FALSE;
				}

				if (g_Enemy[i].flushingCount >= 50)
				{
					g_Enemy[i].use = FALSE;
				}
			}




			if (g_Enemy[i].flushing != TRUE)
			{
				// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
				XMFLOAT3 pos_old = g_Enemy[i].pos;

				// �A�j���[�V����
				g_Enemy[i].countAnim += 1.0f;
				if (g_Enemy[i].countAnim > ANIM_WAIT)
				{
					g_Enemy[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					g_Enemy[i].patternAnim = (g_Enemy[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Enemy[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}

				// �ړ�����
				PLAYER* player = GetPlayer();

				for (int j = 0; j < PLAYER_MAX; j++)
				{
					if (g_Enemy[i].pos.x <= player[j].pos.x)
					{
						g_Enemy[i].dir = ENEMY_DIR_RIGHT;
					}
					else
					{
						g_Enemy[i].dir = ENEMY_DIR_LEFT;
					}
				}

				float radian = atan2f(player[0].pos.y - g_Enemy[i].pos.y, player[0].pos.x - g_Enemy[i].pos.x);
				//float radian = atan2f(g_Enemy[i].pos.y - player[0].pos.y, g_Enemy[i].pos.x - player[0].pos.x);

				g_Enemy[i].pos.x += 1 * cosf(radian);
				g_Enemy[i].pos.y += 1 * sinf(radian);



				//if (g_Enemy[i].tblMax > 0)	// ���`��Ԃ����s����H
				//{	// ���`��Ԃ̏���
				//	int nowNo = (int)g_Enemy[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				//	int maxNo = g_Enemy[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				//	int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				//	INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// �s���e�[�u���̃A�h���X���擾
				//	
				//	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				//	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				//	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�
				//	
				//	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				//	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				//	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���
				//	
				//	float nowTime = g_Enemy[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���
				//	
				//	Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				//	Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				//	Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				//	// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				//	XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				//	// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				//	XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				//	// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				//	XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);
				//	g_Enemy[i].w = TEXTURE_WIDTH * g_Enemy[i].scl.x;
				//	g_Enemy[i].h = TEXTURE_HEIGHT * g_Enemy[i].scl.y;

				//	// frame���g�Ď��Ԍo�ߏ���������
				//	g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				//	if ((int)g_Enemy[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				//	{
				//		g_Enemy[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				//	}

				//}


				g_Enemy[i].attacktime++;
				if (g_Enemy[i].attacktime >= 100)
				{
					radian = atan2f(player[0].pos.y - g_Enemy[i].pos.y, player[0].pos.x - g_Enemy[i].pos.x);


					XMFLOAT3 pos = g_Enemy[i].pos;

					pos.x += 100 * cosf(radian);
					pos.y += 100 * sinf(radian);

					for (int j = 0; j < PLAYER_MAX; j++)
					{
						SetAttack(pos);
					}
					g_Enemy[i].attacktime = 0;
				}

				// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
				{
					PLAYER* player = GetPlayer();

					// �G�l�~�[�̐��������蔻����s��
					for (int j = 0; j < PLAYER_MAX; j++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if (player[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Enemy[i].pos, g_Enemy[i].w * 0.5f, g_Enemy[i].h * 0.5f,
								player[j].pos, player[j].w * 0.5f, player[j].h * 0.5f);
							// �������Ă���H
							if (ans == TRUE && player[j].muteki == FALSE && player[j].flushing == FALSE)
							{
								// �����������̏���
								g_Enemy[i].use = FALSE;
								SubLife(1);
								player[j].flushing = TRUE;
							}
						}
					}
				}
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
void DrawEnemy(void)
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

	for (int i = 0; i <= kazu; i++)
	{
		if (g_Enemy[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes

			if (g_Enemy[i].visible == FALSE) continue;

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Enemy[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_Enemy[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_Enemy[i].w;		// �G�l�~�[�̕\����
			float ph = g_Enemy[i].h;		// �G�l�~�[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			//float tw = 1.0f;	// �e�N�X�`���̕�
			//float th = 1.0f;	// �e�N�X�`���̍���
			//float tx = 0.0f;	// �e�N�X�`���̍���X���W
			//float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


	//// �Q�[�W�̃e�X�g
	//{
	//	// ���~���̃Q�[�W�i�g�I�ȕ��j
	//	// �e�N�X�`���ݒ�
	//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

	//	//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
	//	float px = 600.0f;		// �Q�[�W�̕\���ʒuX
	//	float py =  10.0f;		// �Q�[�W�̕\���ʒuY
	//	float pw = 300.0f;		// �Q�[�W�̕\����
	//	float ph =  30.0f;		// �Q�[�W�̕\������

	//	float tw = 1.0f;	// �e�N�X�`���̕�
	//	float th = 1.0f;	// �e�N�X�`���̍���
	//	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	//	float ty = 0.0f;	// �e�N�X�`���̍���Y���W

	//	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//	SetSpriteLTColor(g_VertexBuffer,
	//		px, py, pw, ph,
	//		tx, ty, tw, th,
	//		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	//	// �|���S���`��
	//	GetDeviceContext()->Draw(4, 0);


	//	// �G�l�~�[�̐��ɏ]���ăQ�[�W�̒�����\�����Ă݂�
	//	// �e�N�X�`���ݒ�
	//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

	//	//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
	//	pw = pw * ((float)g_EnemyCount / ENEMY_MAX);

	//	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//	SetSpriteLTColor(g_VertexBuffer,
	//		px, py, pw, ph,
	//		tx, ty, tw, th,
	//		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

	//	// �|���S���`��
	//	GetDeviceContext()->Draw(4, 0);


	//}




}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}


// �����Ă�G�l�~�[�̐�
int GetEnemyCount(void)
{
	return g_EnemyCount;
}


