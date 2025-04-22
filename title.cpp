//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : 
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(7)				// �e�N�X�`���̐�

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
	"data/TEXTURE/gametitle.jpg",
	"data/TEXTURE/brokencity.png",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/NEWGAME_text.png",
	"data/TEXTURE/cooltext465274065798148.png",
	"data/TEXTURE/EXIT_text.png",
	"data/TEXTURE/serect_arrow.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

//���p�̕ϐ�
int			arrow_count;
int 		countAnim;		// �A�j���[�V�����J�E���g
int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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

	alpha = 1.0f;
	flag_alpha = TRUE;

	effect_dx = 100.0f;
	effect_dy = 100.0f;

	g_Load = TRUE;
	return S_OK;


	arrow_count = 0;
	countAnim = 0;
	patternAnim = 0;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{

	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonPressed(0, BUTTON_A))
	{// Enter��������A�X�e�[�W��؂�ւ���
		switch (arrow_count)
		{
		case 0:
			SetFade(FADE_OUT, MODE_TUTORIAL);
			break;

		case 1:
			SetFade(FADE_OUT, MODE_STAGESERECT);
			break;

			//case 2:
			//	break;
		}
	}
	else if (GetKeyboardTrigger(DIK_W) || GetKeyboardTrigger(DIK_UPARROW) || IsButtonPressed(0, BUTTON_UP))
	{
		arrow_count--;

		if (arrow_count < 0)
		{
			arrow_count = 0;
		}
	}
	else if (GetKeyboardTrigger(DIK_S) || GetKeyboardTrigger(DIK_DOWNARROW) || IsButtonPressed(0, BUTTON_DOWN))
	{
		arrow_count++;

		if (arrow_count >= 2)
		{
			arrow_count = 1;
		}
	}

	// �A�j���[�V����
	countAnim++;
	if ((countAnim % ANIM_WAIT) == 0)
	{
		// �p�^�[���̐؂�ւ�
		patternAnim = (patternAnim + 1) % ANIM_PATTERN_NUM;
	}

	//// �Q�[���p�b�h�œ��͏���
	//else if (IsButtonTriggered(0, BUTTON_START))
	//{
	//	SetFade(FADE_OUT, MODE_GAME);
	//}
	//else if (IsButtonTriggered(0, BUTTON_B))
	//{
	//	SetFade(FADE_OUT, MODE_GAME);
	//}



	// �Z�[�u�f�[�^�����[�h����H
	//if (GetKeyboardTrigger(DIK_L))
	//{
	//	SetLoadGame(TRUE);
	//	SetFade(FADE_OUT, MODE_GAME);
	//}




#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//�e�L�X�g��`��
	//TITLE
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 100.0f, 100.0f, 400.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//NEW GAME
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 180.0f, 250.0f, 200.0f, 40.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//STAGE SERECT
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 175.0f, 300.0f, 230.0f, 45.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

	//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = 150.0f;	// �v���C���[�̕\���ʒuX
	float py = 270.0f + (50.0f * arrow_count);	// �v���C���[�̕\���ʒuY
	float pw = 50.0f;		// �v���C���[�̕\����
	float ph = 50.0f;		// �v���C���[�̕\������

	// �A�j���[�V�����p
	float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
	float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
	float ty = (float)(patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W
	float tx = (float)(patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W

	//// �A�j���[�V�����p
	//float tw = 1.0f;	// �e�N�X�`���̕�
	//float th = 1.0f;	// �e�N�X�`���̍���
	//float tx = 1.0f;	// �e�N�X�`���̍���X���W
	//float ty = 1.0f;	// �e�N�X�`���̍���Y���W


	//// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	//	g_Serect.rot.z);

	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));


	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

}





