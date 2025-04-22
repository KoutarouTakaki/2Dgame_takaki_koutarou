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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(1000/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(100/2)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
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

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/attack.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static GUN		g_Gun[GUN_MAX];	// �o���b�g�\����
static GUNENEMY	g_Gunenemy[GUNENEMY_MAX];

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGun(void)
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
	for (int i = 0; i < GUN_MAX; i++)
	{
		g_Gun[i].use = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
		g_Gun[i].w = TEXTURE_WIDTH;
		g_Gun[i].h = TEXTURE_HEIGHT;
		g_Gun[i].pos = XMFLOAT3(300, 300.0f, 0.0f);
		g_Gun[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Gun[i].texNo = 0;
		g_Gun[i].sponetime = 0;
		g_Gun[i].dir = GUNE_DIR_RIGHT;
		g_Gun[i].countAnim = 0;
		g_Gun[i].patternAnim = 0;

		//g_Gun[i].move = XMFLOAT3(0.0f, -BULLET_SPEED, 0.0f);	// �ړ��ʂ�������
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGun(void)
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
void UpdateGun(void)
{
	if (g_Load == FALSE) return;
	int attackCount = 0;				// ���������o���b�g�̐�

	for (int i = 0; i < GUN_MAX; i++)
	{
		if (g_Gun[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			// �A�j���[�V����  
			g_Gun[i].countAnim++;
			if ((g_Gun[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_Gun[i].patternAnim = (g_Gun[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}


			//// �o���b�g�̈ړ�����
			//XMVECTOR pos = XMLoadFloat3(&g_Gun[i].pos);
			////XMVECTOR move = XMLoadFloat3(&g_Gun[i].move);
			////pos += move;
			//XMStoreFloat3(&g_Gun[i].pos, pos);

			//// ��ʊO�܂Ői�񂾁H
			//BG* bg = GetBG();
			//if (g_Gun[i].pos.y < (-g_Gun[i].h / 2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			//{
			//	g_Gun[i].use = false;
			//}
			//if (g_Gun[i].pos.y > (bg->h + g_Gun[i].h / 2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			//{
			//	g_Gun[i].use = false;
			//}

			// �����蔻�菈��
			{
				GUNENEMY* GUNENEMY = GetGunenemy();
				PLAYER* player = GetPlayer();

				//// �G�l�~�[�̐��������蔻����s��
				//for (int j = 0; j < GUNENEMY_MAX; j++)
				//{
				//	// �����Ă�G�l�~�[�Ɠ����蔻�������
				//	if (GUNENEMY[j].use == TRUE)
				//	{
				//		BOOL ans = CollisionBB(g_Gun[i].pos, g_Gun[i].w, g_Gun[i].h,
				//			GUNENEMY[j].pos, GUNENEMY[j].w, GUNENEMY[j].h);
				//		// �������Ă���H
				//		if (ans == TRUE)
				//		{
				//			// �����������̏���
				//			GUNENEMY[j].use = FALSE;
				//			AddScore(100);
				//			// �G�t�F�N�g����
				//			SetEffect(GUNENEMY[j].pos.x, GUNENEMY[j].pos.y, 30);
				//		}
				//	}
				//}

				//for (int j = 0; j < GUNENEMY_MAX; j++)
				//{
				//	// �����Ă�G�l�~�[�Ɠ����蔻�������
				//	if (GUNENEMY[j].use == TRUE)
				//	{
				//		g_Gun[i].pos = GUNENEMY[j].pos;

				//		for (int k = 0; k < PLAYER_MAX; k++)
				//		{
				//			if (GUNENEMY[j].pos.x < player[k].pos.x)
				//			{
				//				g_Gun[i].pos.x += 100.0f;
				//			}
				//			else
				//			{
				//				g_Gun[i].pos.x -= 100.0f;
				//			}
				//		}
				//	}
				//}

			}

			attackCount++;
			g_Gun[i].sponetime++;
			if (g_Gun[i].sponetime >= 50)
			{
				g_Gun[i].use = false;
				g_Gun[i].sponetime = 0;
				SetGunhit(g_Gun[i].pos, g_Gun[i].dir);
			}
		}
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGun(void)
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

	for (int i = 0; i < GUN_MAX; i++)
	{
		if (g_Gun[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Gun[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Gun[i].pos.x - bg->pos.x;	// �o���b�g�̕\���ʒuX
			float py = g_Gun[i].pos.y - bg->pos.y;	// �o���b�g�̕\���ʒuY
			float pw = g_Gun[i].w;		// �o���b�g�̕\����
			float ph = g_Gun[i].h;		// �o���b�g�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Gun[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Gun[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 0.7f),
				g_Gun[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
GUN* GetGun(void)
{
	return &g_Gun[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetGun(XMFLOAT3 pos, int dir)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < GUN_MAX; i++)
	{
		if (g_Gun[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Gun[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_Gun[i].pos = pos;			// ���W���Z�b�g
			g_Gun[i].dir = dir;
			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}

