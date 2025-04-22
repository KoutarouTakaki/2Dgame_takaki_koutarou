//=============================================================================
//
// BG���� [bg.cpp]
// Author : 
//
//=============================================================================
#include "bg.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(6000)			// (SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(1500)			// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/1.png",
	"data/TEXTURE/2.png",
	"data/TEXTURE/3.png",
	"data/TEXTURE/4.png",
	"data/TEXTURE/5.png",
};


static BOOL	g_Load = FALSE;		// ���������s�������̃t���O
static BG	g_BG;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBG(void)
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
	g_BG.w = TEXTURE_WIDTH;
	g_BG.h = TEXTURE_HEIGHT;
	g_BG.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_BG.texNo = 0;

	g_BG.scrl = 0.0f;		// TEX�X�N���[��
	g_BG.scrl2 = 0.0f;		// TEX�X�N���[��
	g_BG.scrl3 = 0.0f;		// TEX�X�N���[��
	g_BG.scrl4 = 0.0f;		// TEX�X�N���[��


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{
	g_BG.old_pos = g_BG.pos;	// �P�t���O�̏���ۑ�

	PLAYER* player = GetPlayer();

	if (player[0].dir == CHAR_DIR_RIGHT || player[0].dir == CHAR_DIR_UP_R || player[0].dir == CHAR_DIR_ATTACK_R && player[0].moving == TRUE)
	{
		if (player[0].pos.x != player[0].old_pos.x)
		{
			g_BG.scrl += 0.0005f;
			g_BG.scrl2 += 0.001f;
			g_BG.scrl3 += 0.0015f;
			g_BG.scrl4 += 0.002f;
		}
	}
	else if (player[0].dir == CHAR_DIR_LEFT || player[0].dir == CHAR_DIR_UP_L || player[0].dir == CHAR_DIR_ATTACK_L && player[0].moving == TRUE)
	{
		if (player[0].pos.x != player[0].old_pos.x)
		{
			g_BG.scrl -= 0.0005f;
			g_BG.scrl2 -= 0.001f;
			g_BG.scrl3 -= 0.0015f;
			g_BG.scrl4 -= 0.002f;
		}
	}

	//g_BG.scrl -= 0.0f;		// 0.005f;		// �X�N���[��


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBG(void)
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

	// �n�ʂ�`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_BG.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// ���`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//float	tx = (g_BG.pos.x - g_BG.old_pos.x) * ((float)SCREEN_WIDTH / TEXTURE_WIDTH);
		//g_BG.scrl += tx * 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0 - 0.0f, SCREEN_WIDTH, SCREEN_WIDTH,
			g_BG.scrl, 0.3f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//float	tx = (g_BG.pos.x - g_BG.old_pos.x) * ((float)SCREEN_WIDTH / TEXTURE_WIDTH);
		//g_BG.scrl += tx * 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0 - 0.0f, SCREEN_WIDTH, SCREEN_WIDTH,
			g_BG.scrl2, 0.3f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//float	tx = (g_BG.pos.x - g_BG.old_pos.x) * ((float)SCREEN_WIDTH / TEXTURE_WIDTH);
		//g_BG.scrl += tx * 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0 - 0.0f, SCREEN_WIDTH, SCREEN_WIDTH,
			g_BG.scrl3, 0.3f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//float	tx = (g_BG.pos.x - g_BG.old_pos.x) * ((float)SCREEN_WIDTH / TEXTURE_WIDTH);
		//g_BG.scrl += tx * 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0 - 0.0f, SCREEN_WIDTH, SCREEN_WIDTH,
			g_BG.scrl4, 0.3f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �Ȃ񂿂���đ��d�X�N���[��
	//{
	//	// �e�N�X�`���ݒ�
	//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	//	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//	float	tx = (g_BG.pos.x - g_BG.old_pos.x) * ((float)SCREEN_WIDTH / TEXTURE_WIDTH);
	//	g_BG.scrl2 += tx * 0.01f;
	//	//g_BG.scrl2 += 0.003f;

	//	SetSpriteLTColor(g_VertexBuffer,
	//		0.0f, SKY_H/2, SCREEN_WIDTH, SKY_H,
	//		g_BG.scrl2, 0.0f, 1.0f, 1.0f,
	//		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	//	// �|���S���`��
	//	GetDeviceContext()->Draw(4, 0);
	//}


}


//=============================================================================
// BG�\���̂̐擪�A�h���X���擾
//=============================================================================
BG* GetBG(void)
{
	return &g_BG;
}





