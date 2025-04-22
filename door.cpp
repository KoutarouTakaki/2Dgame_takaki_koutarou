//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : 
//
//=============================================================================
#include "door.h"
#include "input.h"
#include "fade.h"
#include "bg.h"
#include "main.h"
#include "player.h"
#include "collision.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define TEXTURE_PATTERN_DIVIDE_X	(3)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
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
	"data/TEXTURE/potal.png",
	"data/TEXTURE/saiber_2.png",
	"data/TEXTURE/saiber_3.png",
	"data/TEXTURE/saiber_4.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

//float	alpha;
//BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

static DOOR door[DOOR_MAX];

static int door_number;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitDoor(int n)
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


	// �ϐ��̏�����
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	g_TexNo = 0;

	//alpha = 1.0f;
	//flag_alpha = TRUE;

	effect_dx = 100.0f;
	effect_dy = 100.0f;

	for (int i = 0; i < DOOR_MAX; i++)
	{
		if (i == 0)
		{
			door[i].pos = XMFLOAT3(5850.0f, 1070.0f, 0.0f);
		}
		else if (i == 1)
		{
			door[i].pos = XMFLOAT3(5850.0f, 1070.0f, 0.0f);
		}
		else if (i == 2)
		{
			door[i].pos = XMFLOAT3(5650.0f, 900.0f, 0.0f);
		}
		else if (i == 3)
		{
			door[i].pos = XMFLOAT3(5850.0f, 1070.0f, 0.0f);
		}

		door[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		door[i].w = 200.0f;
		door[i].h = 200.0f;

	}

	if (n == 0)
	{
		door_number = 0;
	}
	else if (n == 1)
	{
		door_number = 1;
	}
	else if (n == 2)
	{
		door_number = 2;
	}



	g_Load = TRUE;
	return S_OK;


	//arrow_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//arrow_w = 50.0f;
	//arrow_h = 50.0f;
	//arrow_count = 0;
	//arrow_max = 3;
	//countAnim = 0;
	//patternAnim = 0;
}

//=============================================================================
// �I������
//=============================================================================
void UninitDoor(void)
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
void UpdateDoor(void)
{

	for (int i = 0; i < DOOR_MAX; i++)
	{
		if (i != door_number)continue;

		PLAYER* player = GetPlayer();

		// �G�l�~�[�̐��������蔻����s��
		for (int j = 0; j < PLAYER_MAX; j++)
		{
			// �����Ă�G�l�~�[�Ɠ����蔻�������
			if (player[j].use == TRUE)
			{
				BOOL ans = CollisionBB(door[i].pos, (door[i].w / 2), (door[i].h / 2),
					player[j].pos, player[j].w, player[j].h);
				// �������Ă���H
				if (ans == TRUE)
				{
					int mode = GetMode();
					if (mode == MODE_TUTORIAL)
					{
						SetFade(FADE_OUT, MODE_STAGE1);
					}
					else
					{
						SetFade(FADE_OUT, MODE_RESULT);
					}
				}
			}
		}

		door[i].rot.z += 0.01f;
	}



#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawDoor(void)
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

	for (int i = 0; i < DOOR_MAX; i++)
	{
		if (i != door_number)continue;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = door[i].pos.x - bg->pos.x;			// �X�R�A�̕\���ʒuX
		float py = door[i].pos.y - bg->pos.y;			// �X�R�A�̕\���ʒuY
		float pw = door[i].w;				// �X�R�A�̕\����
		float ph = door[i].h;				// �X�R�A�̕\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = 0.0f;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			door[i].rot.z);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}


	SetBlendState(BLEND_MODE_ALPHABLEND);	// ���������������ɖ߂�

}





