//=============================================================================
//
// BG処理 [bg.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PAUSE_TEXTURE_WIDTH				(960)
#define PAUSE_TEXTURE_HEIGHT			(304 * 2)
#define PAUSE_TEXTURE_MAX				(3)

// カーソルの位置
#define	PAUSE_CUR_1_X					(300.0f)
#define	PAUSE_CUR_1_Y					(300.0f)

#define	PAUSE_CUR_2_X					(300.0f)
#define	PAUSE_CUR_2_Y					(360.0f)

#define	PAUSE_CUR_3_X					(300.0f)
#define	PAUSE_CUR_3_Y					(400.0f)


// カーソルの幅と高さ
#define PAUSE_CURSOR_WIDTH				(40)
#define PAUSE_CURSOR_HEIGHT				(40)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
enum 
{
	pause,
	pause_cursor,
	black,
};



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[PAUSE_TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[PAUSE_TEXTURE_MAX] = {
	"data/TEXTURE/pause.png",			// poseメニュー
	"data/TEXTURE/cursor_pause.png",	// カーソル
	"data/TEXTURE/black_f.png",			// 黒背景


};


static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ

// ポリゴンの座標
XMFLOAT3	g_PosePos;

// ポーズメニューを開いたかどうか
static BOOL g_pause;

// カーソルの変数
int pose_cursor;

//// スクロール変数
//float pose_scroll;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPause(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < PAUSE_TEXTURE_MAX; i++)
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


	// 変数の初期化
	g_PosePos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_pause		= FALSE;
	g_Load		= TRUE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPause(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < PAUSE_TEXTURE_MAX; i++)
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
void UpdatePause(void)
{
	if (g_pause == FALSE)
	{	// pauseメニューに行く
		if (GetKeyboardTrigger(DIK_P) || IsButtonTriggered(0, BUTTON_START))
		{
			SetPause();
			g_pause = TRUE;
		}
	}



	// カーソルの移動処理(上下の移動の移動)
	if (GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_UP))
	{
		pose_cursor = (--pose_cursor + TITLE_MAX) % TITLE_MAX;
}
	if (GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_DOWN))
	{
		pose_cursor = (++pose_cursor + TITLE_MAX) % TITLE_MAX;
	}

	// もしポーズメニューが開かれたら
	if (g_pause == TRUE)
	{
		switch (pose_cursor)
		{
		case TITLE_GAMESTART:		// ゲーム画面に戻る
			g_PosePos = XMFLOAT3(PAUSE_CUR_1_X, PAUSE_CUR_1_Y, 0.0f);
			if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
			{
				SetPause();
				g_pause = FALSE;

			}
			break;

		case TITLE_EXPLANATION:		// チュートリアル
			g_PosePos = XMFLOAT3(PAUSE_CUR_2_X, PAUSE_CUR_2_Y, 0.0f);
			if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
			{
				PlaySound(SOUND_LABEL_game_start);

				SetFadeMode(FADE_OUT, MODE_EXPLANATION);
				SetPause();

			}

			break;

		case TITLE_EXIT:		// ゲーム終了
			g_PosePos = XMFLOAT3(PAUSE_CUR_3_X, PAUSE_CUR_3_Y, 0.0f);
			if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
			{
				GameLoop();
			}

			break;
		}


	}




#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPause(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	// pauseボタンを押されたら
	if (g_pause == TRUE)
	{
		// 黒背景の表示
		{
			// マテリアル設定
			MATERIAL material;
			ZeroMemory(&material, sizeof(material));
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.7f);
			SetMaterial(material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[black]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, PAUSE_TEXTURE_WIDTH, PAUSE_TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}

			// pause項目を描画
		{
			// マテリアル設定
			MATERIAL material;
			ZeroMemory(&material, sizeof(material));
			material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			SetMaterial(material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[pause]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, PAUSE_TEXTURE_WIDTH, PAUSE_TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		

			// カーソルの表示
		
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[pause_cursor]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, g_PosePos.x, g_PosePos.y, PAUSE_CURSOR_WIDTH, PAUSE_CURSOR_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}

	}
}