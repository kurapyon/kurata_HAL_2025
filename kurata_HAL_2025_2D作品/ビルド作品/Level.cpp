//=============================================================================
//
// スコア処理 [score.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "Level.h"
#include "sprite.h"
#include "player.h"
#include "bg.h"
#include "playereffect.h"
#include "map.h"
#include "enemy.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(16)	// キャラサイズ
#define TEXTURE_HEIGHT				(32)	// 
#define LEVEL_TEXTURE_MAX					(4)		// テクスチャの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
enum 
{
	TEXTURE_SUUJI,
	TEXTURE_energy01,
	TEXTURE_energy,
	TEXTURE_LEVELUP,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[LEVEL_TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[LEVEL_TEXTURE_MAX] = {
	"data/TEXTURE/suuji.png",			// レベル表示（数字）
	"data/TEXTURE/energy01.png",		// スタミナゲージの枠組み
	"data/TEXTURE/energy.png",			// スタミナゲージ
	"data/TEXTURE/levelup.png",			// レベルアップエフェクト


};


static bool						g_Use;						// true:使っている  false:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

// レベルアップ
int lvupcnt = 0;

static LEVEL				g_level[LEVEL_MAX];							// レベル構造体


static int						g_Score;					// スコア

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitLevel(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < LEVEL_TEXTURE_MAX; i++)
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

	// レベル構造体の初期化
	for (int i = 0; i < LEVEL_MAX; i++)
	{
		g_level[i].guage    = 0;
		g_level[i].guagemax = 100;
		g_level[i].level    = 1;
	}

	// スコアの初期化
	g_Use   = true;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   =XMFLOAT3{ 500.0f, 20.0f, 0.0f };
	g_TexNo = 0;

	g_Score = 0;	// スコアの初期化

	g_Load = TRUE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitLevel(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < LEVEL_TEXTURE_MAX; i++)
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
void UpdateLevel(void)
{
	PLAYER* player = GetPlayer();

	// 経験値回収
	if (g_level[0].guagemax <= g_level[0].guage)
	{
		// ゲージマックス分、ゲージを０にする
		g_level[0].guage -= g_level[0].guagemax;

		// レベルが上がるごとに、ゲージマックスを上げる
		g_level[0].guagemax += (g_level[0].level * 30);

		// ゲージがマックスの時にレベルアップする
		g_level[0].level++;

		// エフェクト
   		SetPlayerEffect(player[0].pos.x, player[0].pos.y, 30, TEXTURE_LVUP);

		// レベルアップ
		SetLevelUP();
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawLevel(void)
{
	PLAYER* player = GetPlayer();
	BG* bg = GetBG();


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


	// 経験値(レベルアップ)
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[0].levelup == TRUE)
		{
			// レベルアップ表示
			lvupcnt++;
			float alpha;
			if ((lvupcnt / 8) % 2 == 0)
			{
				alpha = 1.0f;
			}
			else
			{
				alpha = 0.0f;
			}

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_LEVELUP]);

			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			//プレイヤーの位置やテクスチャー座標を反映
			float px = player[i].pos.x - scrollX;									// レベルアップ文字の表示位置X
			float py = player[i].pos.y - scrollY - 10.0f;							// レベルアップ文字の表示位置Y（-10することであたもの上に表示できる）
			float pw = 50;															// レベルアップ文字の表示幅
			float ph = 15;															// レベルアップ文字の表示高さ

			py += player[0].jumpY;													// ジャンプ中の高さを足す

			// アニメーション用
			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ


			float tx = 1.0f;	// テクスチャの左上X座標
			float ty = 1.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				player[0].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			if (lvupcnt == 100)
			{
				player[i].levelup = FALSE;
				lvupcnt = 0;
			}

		}

	}

	 //経験値バー
	{

		//ゲージの位置やテクスチャー座標を反映
		float px = 20.0f;					// ゲージの表示位置X
		float py = 90.0f;					// ゲージの表示位置Y
		float pw = 250.0;					// ゲージの表示幅
		float ph = 18.0f;					// ゲージの表示高さ

		float tw = 1.0f;					// テクスチャの幅
		float th = 1.0f;					// テクスチャの高さ
		float tx = 0.0f;					// テクスチャの左上X座標
		float ty = 0.0f;					// テクスチャの左上Y座標

		// エネミーの数に従ってゲージの長さを表示してみる
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_energy]);

		//ゲージの位置やテクスチャー座標を反映
		if (g_level[0].guage < g_level[0].guagemax)
		{
			pw = pw * (g_level[0].guage / g_level[0].guagemax);

		}

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);




	// 下敷きのゲージ（枠的な物）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_energy01]);

		//ゲージの位置やテクスチャー座標を反映
		px = 10.0f;							// ゲージの表示位置X
		py = 90.0f;							// ゲージの表示位置Y
		pw = 300.0;							// ゲージの表示幅
		ph = 18.0f;							// ゲージの表示高さ

		tw = 1.0f;							// テクスチャの幅
		th = 1.0f;							// テクスチャの高さ
		tx = 0.0f;							// テクスチャの左上X座標
		ty = 0.0f;							// テクスチャの左上Y座標

	   // １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,px, py, pw, ph,tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}


	// もし、プレイヤーがレベルアップしたら表示を変える
	// 桁数分処理する
	int number = g_level[0].level;
	for (int i = 0; i < LEVEL_DIGIT; i++)
	{

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SUUJI]);

		// 今回表示する桁の数字
		float x = (float)(number % 10);

		// スコアの位置やテクスチャー座標を反映
		float px = g_Pos.x - g_w * i;	// スコアの表示位置X
		float py = g_Pos.y;				// スコアの表示位置Y
		float pw = g_w;					// スコアの表示幅
		float ph = g_h;					// スコアの表示高さ

		float tw = 1.0f / 13;			// テクスチャの幅
		float th = 1.0f / 1;			// テクスチャの高さ
		float tx = x * tw;				// テクスチャの左上X座標
		float ty = 0.0f;				// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// 次の桁へ
		number /= 10;
	}
}


// 経験値集め
void GuagePlus(float num) 
{
	g_level[0].guage += num;
}


LEVEL* GetLevel(void)
{
	return &g_level[0];
}

