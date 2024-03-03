//=============================================================================
//
// チュートリアル処理 [tutuorial.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "sprite.h"
#include "renderer.h"
#include "enemy.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "bullet.h"
#include "fade.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ(960)
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ(540)

// テクスチャの数
#define TUTORIAL_TEXTURE_MAX		(1)		

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;						// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TUTORIAL_TEXTURE_MAX] = { NULL };	// テクスチャ情報


static char *g_TexturName[TUTORIAL_TEXTURE_MAX] = {
	"data/TEXTURE/tutorial.png",

};

// 初期化を行ったかのフラグ
static BOOL		g_Load = FALSE;		

// エネミーの数を数える
int		g_tutorialCnt;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT  InitTutorial(void)
{
	ID3D11Device *pDevice = GetDevice();


	//テクスチャ生成
	for (int i = 0; i < TUTORIAL_TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);
			
	// チュートリアル構造体の初期化
	g_tutorialCnt = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TUTORIAL_TEXTURE_MAX; i++)
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
// 更新処理
//=============================================================================
void UpdatTutorial(void)
{
	//TUTORIAL_ENEMY_UPDATE();

	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
	{
		SetFadeMode(FADE_OUT, MODE_GAME);
	}

	
#ifdef _DEBUG	// デバッグ情報を表示する

	if (GetKeyboardTrigger(DIK_5))
	{
		SetFadeMode(FADE_OUT, MODE_GAME);
	}

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

}