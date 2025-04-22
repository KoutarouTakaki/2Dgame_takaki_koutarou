//=============================================================================
//
// ���U���g��ʏ��� [result.cpp]
// Author : 
//
//=============================================================================
#include "gameover.h"
#include "input.h"
#include "score.h"
#include "fade.h"
#include "main.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(700)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(150)			// 

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
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/clear_title.jpg",
	"data/TEXTURE/GameOver_text.png",
	"data/TEXTURE/Retry_text.png",
	"data/TEXTURE/cooltext465274065798148.png",
	"data/TEXTURE/serect_arrow.png",
};

static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static BOOL						g_Load = FALSE;

//���p�̕ϐ�
int			go_arrow_count;
int 		go_countAnim;		// �A�j���[�V�����J�E���g
int			go_patternAnim;	// �A�j���[�V�����p�^�[���i���o�[

int			mode;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitGameover(void)
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
	g_Pos = { g_w / 2, g_h / 2, 0.0f };
	g_TexNo = 0;

	// BGM�Đ�
	go_arrow_count = 0;
	go_countAnim = 0;
	go_patternAnim = 0;


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGameover(void)
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
void UpdateGameover(void)
{

	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonPressed(0, BUTTON_A))
	{// Enter��������A�X�e�[�W��؂�ւ���
		switch (go_arrow_count)
		{
		case 0:
			switch (mode)
			{
			case 3:
				SetFade(FADE_OUT, MODE_STAGE1);
				break;

			case 4:
				SetFade(FADE_OUT, MODE_STAGE2);
				break;

			case 5:
				SetFade(FADE_OUT, MODE_BOSS);
				break;
			}
			break;

		case 1:
			SetFade(FADE_OUT, MODE_STAGESERECT);
			break;
		}
	}
	else if (GetKeyboardTrigger(DIK_W) || GetKeyboardTrigger(DIK_UPARROW) || IsButtonPressed(0, BUTTON_UP))
	{
		go_arrow_count--;

		if (go_arrow_count < 0)
		{
			go_arrow_count = 0;
		}
	}
	else if (GetKeyboardTrigger(DIK_S) || GetKeyboardTrigger(DIK_DOWNARROW) || IsButtonPressed(0, BUTTON_DOWN))
	{
		go_arrow_count++;

		if (go_arrow_count >= 2)
		{
			go_arrow_count = 1;
		}
	}

	// �A�j���[�V����
	go_countAnim++;
	if ((go_countAnim % ANIM_WAIT) == 0)
	{
		// �p�^�[���̐؂�ւ�
		go_patternAnim = (go_patternAnim + 1) % ANIM_PATTERN_NUM;
	}


#ifdef _DEBUG	// �f�o�b�O����\������

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGameover(void)
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

	// ���U���g�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���U���g�̃��S��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���U���g�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 600.0f, 400.0f, 200.0f, 40.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���U���g�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 610.0f, 450.0f, 250.0f, 50.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

	//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = 580.0f;	// �v���C���[�̕\���ʒuX
	float py = 420.0f + (55.0f * go_arrow_count);	// �v���C���[�̕\���ʒuY
	float pw = 50.0f;		// �v���C���[�̕\����
	float ph = 50.0f;		// �v���C���[�̕\������

	// �A�j���[�V�����p
	float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
	float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
	float ty = (float)(go_patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W
	float tx = (float)(go_patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W

	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));


	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


}

void SetOldMode(void)
{
	mode = GetMode();

}


