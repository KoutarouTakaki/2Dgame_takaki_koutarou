//=============================================================================
//
// �o���b�g���� [bullet.cpp]
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(200/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(1000/2)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(8)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(8)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/beam.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static GUNHIT	g_Gunhit[GUNHIT_MAX];	// �o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGunhit(void)
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


	// �o���b�g�\���̂̏�����
	for (int i = 0; i < GUNHIT_MAX; i++)
	{
		g_Gunhit[i].use = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
		g_Gunhit[i].w = TEXTURE_WIDTH;
		g_Gunhit[i].h = TEXTURE_HEIGHT;
		g_Gunhit[i].pos = XMFLOAT3(300, 300.0f, 0.0f);
		g_Gunhit[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Gunhit[i].texNo = 0;
		g_Gunhit[i].sponetime = 0;
		g_Gunhit[i].hitCount = FALSE;

		g_Gunhit[i].countAnim = 0;
		g_Gunhit[i].patternAnim = 0;

		//g_Gunhit[i].move = XMFLOAT3(0.0f, -BULLET_SPEED, 0.0f);	// �ړ��ʂ�������
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateGunhit(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < GUNHIT_MAX; i++)
	{
		if (g_Gunhit[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			// �A�j���[�V����
			g_Gunhit[i].countAnim++;
			if ((g_Gunhit[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
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


			//// �o���b�g�̈ړ�����
			//XMVECTOR pos = XMLoadFloat3(&g_Gunhit[i].pos);
			////XMVECTOR move = XMLoadFloat3(&g_Gunhit[i].move);
			////pos += move;
			//XMStoreFloat3(&g_Gunhit[i].pos, pos);

			//// ��ʊO�܂Ői�񂾁H
			//BG* bg = GetBG();
			//if (g_Gunhit[i].pos.y < (-g_Gunhit[i].h / 2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			//{
			//	g_Gunhit[i].use = false;
			//}
			//if (g_Gunhit[i].pos.y > (bg->h + g_Gunhit[i].h / 2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			//{
			//	g_Gunhit[i].use = false;
			//}

			// �����蔻�菈��
			{
				PLAYER* player = GetPlayer();

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (player[j].use == TRUE)
					{
						if (player[j].muteki == FALSE)
						{
							BOOL ans = CollisionBB(g_Gunhit[i].pos, g_Gunhit[i].w * 0.7f, g_Gunhit[i].h,
								player[j].pos, player[j].w * 0.5f, player[j].h * 0.5f);
							// �������Ă���H
							if (ans == TRUE && g_Gunhit[i].hitCount == FALSE && player[j].muteki == FALSE && player[j].flushing == FALSE)
							{
								// �����������̏���
								g_Gunhit[i].hitCount = TRUE;
								SubLife(1);
								player[j].flushing = TRUE;
								// �G�t�F�N�g����
								//SetEffect(player[j].pos.x, player[j].pos.y, 30);
							}
						}
					}
				}

				//for (int j = 0; j < ENEMY_MAX; j++)
				//{
				//	// �����Ă�G�l�~�[�Ɠ����蔻�������
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
// �`�揈��
//=============================================================================
void DrawGunhit(void)
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

	for (int i = 0; i < GUNHIT_MAX; i++)
	{
		if (g_Gunhit[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Gunhit[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Gunhit[i].pos.x - bg->pos.x;	// �o���b�g�̕\���ʒuX
			float py = g_Gunhit[i].pos.y - bg->pos.y;	// �o���b�g�̕\���ʒuY
			float pw = g_Gunhit[i].w;		// �o���b�g�̕\����
			float ph = g_Gunhit[i].h;		// �o���b�g�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Gunhit[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Gunhit[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Gunhit[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
GUNHIT* GetGunhit(void)
{
	return &g_Gunhit[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetGunhit(XMFLOAT3 pos,int dir)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < GUNHIT_MAX; i++)
	{
		if (g_Gunhit[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Gunhit[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_Gunhit[i].pos = pos;			// ���W���Z�b�g
			g_Gunhit[i].dir = dir;

			if (g_Gunhit[i].dir == GUNE_DIR_RIGHT)
			{
				g_Gunhit[i].rot.z = 1.5708f;
			}
			else if (g_Gunhit[i].dir == GUNE_DIR_LEFT)
			{
				g_Gunhit[i].rot.z = -1.5708f;
			}

			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}

