//=============================================================================
//
// �G�l�~�[���� [GUNENEMY.cpp]
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(200/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(200/2)	// 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
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
	"data/TEXTURE/gunenemy_textrue.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static GUNENEMY	g_Gunenemy[GUNENEMY_MAX];		// �G�l�~�[�\����

static int		g_GunenemyCount = GUNENEMY_MAX;

//static INTERPOLATION_DATA g_MoveTbl0[] = {
//	//���W									��]��							�g�嗦					����
//	{ XMFLOAT3(850.0f, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	100 },
//	{ XMFLOAT3(1000.0f, 800.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	100 },
//};
//
//
//static INTERPOLATION_DATA g_MoveTbl1[] = {
//	//���W									��]��							�g�嗦							����
//	{ XMFLOAT3(1700.0f,   0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
//	{ XMFLOAT3(1700.0f,  SCREEN_HEIGHT, 0.0f),XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(2.0f, 2.0f, 1.0f),	60 },
//};
//
//
//static INTERPOLATION_DATA g_MoveTbl2[] = {
//	//���W									��]��							�g�嗦							����
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
// ����������
//=============================================================================
HRESULT InitGunenemy(int min, int max)
{
	g_kazu = max - min;

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
	g_GunenemyCount = 0;
	for (int i = 0; i <= g_kazu; i++)
	{
		g_GunenemyCount++;
		g_Gunenemy[i].use = TRUE;
		if (i % 3 == 0)
		{
			g_Gunenemy[i].pos = XMFLOAT3(300.0f + i * 1000.0f, 100.0f, 0.0f);	// ���S�_����\��
		}
		else if (i % 3 == 1)
		{
			g_Gunenemy[i].pos = XMFLOAT3(300.0f + i * 1000.0f, 500.0f, 0.0f);	// ���S�_����\��
		}
		else
		{
			g_Gunenemy[i].pos = XMFLOAT3(300.0f + i * 1000.0f, 900.0f, 0.0f);	// ���S�_����\��
		}
		g_Gunenemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Gunenemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Gunenemy[i].w = TEXTURE_WIDTH;
		g_Gunenemy[i].h = TEXTURE_HEIGHT;
		g_Gunenemy[i].texNo = 0;
		g_Gunenemy[i].attacktime = 0 + (15 * i);
		g_Gunenemy[i].countAnim = 0;
		g_Gunenemy[i].patternAnim = 0;
		g_Gunenemy[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���
		g_Gunenemy[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Gunenemy[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Gunenemy[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		g_Gunenemy[i].flushing = FALSE;
		g_Gunenemy[i].flushingCount = 0;
		g_Gunenemy[i].visible = TRUE;
	}

	//// 0�Ԃ������`��Ԃœ������Ă݂�
	//g_Gunenemy[0].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	//g_Gunenemy[0].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Gunenemy[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	//// 1�Ԃ������`��Ԃœ������Ă݂�
	//g_Gunenemy[1].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	//g_Gunenemy[1].tblNo = 1;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Gunenemy[1].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	//// 2�Ԃ������`��Ԃœ������Ă݂�
	//g_Gunenemy[2].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	//g_Gunenemy[2].tblNo = 2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Gunenemy[2].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateGunenemy(void)
{
	if (g_Load == FALSE) return;
	g_GunenemyCount = 0;			// �����Ă�G�l�~�[�̐�

	for (int i = 0; i <= g_kazu; i++)
	{
		// �����Ă�G�l�~�[��������������
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
				g_GunenemyCount++;		// �����Ă��G�̐�

				// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
				XMFLOAT3 pos_old = g_Gunenemy[i].pos;

				// �A�j���[�V����
				g_Gunenemy[i].countAnim += 1.0f;
				if (g_Gunenemy[i].countAnim > ANIM_WAIT)
				{
					g_Gunenemy[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					g_Gunenemy[i].patternAnim = (g_Gunenemy[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Gunenemy[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}

				// �ړ�����
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

				//// �ړ�����
				//if (g_Gunenemy[i].tblMax > 0)	// ���`��Ԃ����s����H
				//{	// ���`��Ԃ̏���
				//	int nowNo = (int)g_Gunenemy[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				//	int maxNo = g_Gunenemy[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				//	int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				//	INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Gunenemy[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

				//	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				//	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				//	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				//	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				//	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				//	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				//	float nowTime = g_Gunenemy[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				//	Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				//	Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				//	Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				//	// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				//	XMStoreFloat3(&g_Gunenemy[i].pos, nowPos + Pos);

				//	// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				//	XMStoreFloat3(&g_Gunenemy[i].rot, nowRot + Rot);

				//	// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				//	XMStoreFloat3(&g_Gunenemy[i].scl, nowScl + Scl);
				//	g_Gunenemy[i].w = TEXTURE_WIDTH * g_Gunenemy[i].scl.x;
				//	g_Gunenemy[i].h = TEXTURE_HEIGHT * g_Gunenemy[i].scl.y;

				//	// frame���g�Ď��Ԍo�ߏ���������
				//	g_Gunenemy[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				//	if ((int)g_Gunenemy[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				//	{
				//		g_Gunenemy[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
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

				// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
				{

					// �G�l�~�[�̐��������蔻����s��
					for (int j = 0; j < GUNENEMY_MAX; j++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if (player[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Gunenemy[i].pos, g_Gunenemy[i].w * 0.5f, g_Gunenemy[i].h * 0.5f,
								player[j].pos, player[j].w * 0.5f, player[j].h * 0.5f);
							// �������Ă���H
							if (ans == TRUE && player[j].muteki == FALSE && player[j].flushing == FALSE)
							{
								// �����������̏���
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


	//// �G�l�~�[�S�Ń`�F�b�N
	//if (g_GunenemyCount <= 0)
	//{
 //		SetFade(FADE_OUT, MODE_RESULT);
	//}

#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGunenemy(void)
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

	for (int i = 0; i <= g_kazu; i++)
	{
		if (g_Gunenemy[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes

			if (g_Gunenemy[i].visible == FALSE) continue;

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Gunenemy[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Gunenemy[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_Gunenemy[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_Gunenemy[i].w;		// �G�l�~�[�̕\����
			float ph = g_Gunenemy[i].h;		// �G�l�~�[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Gunenemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Gunenemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			//float tw = 1.0f;	// �e�N�X�`���̕�
			//float th = 1.0f;	// �e�N�X�`���̍���
			//float tx = 0.0f;	// �e�N�X�`���̍���X���W
			//float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Gunenemy[i].rot.z);

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
	//	float py = 10.0f;		// �Q�[�W�̕\���ʒuY
	//	float pw = 300.0f;		// �Q�[�W�̕\����
	//	float ph = 30.0f;		// �Q�[�W�̕\������

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
	//	pw = pw * ((float)g_GunenemyCount / GUNENEMY_MAX);

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
// GUNENEMY�\���̂̐擪�A�h���X���擾
//=============================================================================
GUNENEMY* GetGunenemy(void)
{
	return &g_Gunenemy[0];
}


// �����Ă�G�l�~�[�̐�
int GetGunenemyCount(void)
{
	return g_GunenemyCount;
}