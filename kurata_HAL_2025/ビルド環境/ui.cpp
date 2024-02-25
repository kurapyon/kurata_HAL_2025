//=============================================================================
//
// スコア処理 [score.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "ui.h"
#include "sprite.h"
#include "player.h"
#include "light.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(25.0f)		// キャラサイズ
#define TEXTURE_HEIGHT				(25.0f)		// 
#define TEXTURE_MAX					(5)			// テクスチャの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	// プレイヤーのHP
	"data/TEXTURE/heart_main10.png",
	"data/TEXTURE/heartbase10.png",

	// エネミーのHP
	"data/TEXTURE/energy.png",
	"data/TEXTURE/energy01.png",

	// 弾の残弾数
	"data/TEXTURE/Tamaui.png",
}; 

	


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_Score;					// スコア

static BOOL						g_Load = FALSE;

enum Tex
{
	heart_main10,
	heartbase10,

	energy,
	energy01,

	tama_ui,
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUi(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// UIの初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 300.0f, 10.0f, 0.0f };
	g_TexNo = 0;

	g_Score = 0;	// スコアの初期化

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitUi(void)
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
// 更新処理
//=============================================================================
void UpdateUi(void)
{


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawUi(void)
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

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ALPHABLEND);

	PLAYER* player = GetPlayer();
	ENEMY* enemy = GetEnemy();
	ENEMYBOSS* boss = GetEnemyBoss();

	// プレイヤーのHP
	{
		// 下敷きのゲージ（枠的な物）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[heartbase10]);

		//ゲージの位置やテクスチャー座標を反映
		float px = 10.0f;	// ゲージの表示位置X
		float py = 10.0f;	// ゲージの表示位置Y
		float pw = 300.0f;	// ゲージの表示幅
		float ph = 30.0f;	// ゲージの表示高さ

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

	   // １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// ライフ
		//ゲージの位置やテクスチャー座標を反映
		px = 10.0f;	// ゲージの表示位置X
		py = 10.0f;		// ゲージの表示位置Y
		pw = 300.0f;	// ゲージの表示幅
		ph = 30.0f;		// ゲージの表示高さ

		tw = 1.0f;		// テクスチャの幅
		th = 1.0f;		// テクスチャの高さ
		tx = 0.0f;		// テクスチャの左上X座標
		ty = 0.0f;		// テクスチャの左上Y座標

		pw = pw * (float)player[0].hp / PLAYER_HP;
		tw = tw * (float)player[0].hp / PLAYER_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[heart_main10]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// 弾の残り数
	for(int i = 0; i < player->reload; i++)
	{
		//ゲージの位置やテクスチャー座標を反映
		float px =  50.0f;						// ゲージの表示位置X
		float py =  300.0f - TEXTURE_HEIGHT * i;	// ゲージの表示位置Y
		float pw =  50.0f;						// ゲージの表示幅
		float ph = TEXTURE_HEIGHT;						// ゲージの表示高さ

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[tama_ui]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// エネミーのHP(城)
	// マテリアル設定
	material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	SetMaterial(material);


	if(enemy[1].use == TRUE)
	{
		// ライフ
		//ゲージの位置やテクスチャー座標を反映
		float px = 650.0f;	// ゲージの表示位置X
		float py = 17.0f;	// ゲージの表示位置Y
		float pw = 250.0f;	// ゲージの表示幅
		float ph = 10.0f;	// ゲージの表示高さ
			  
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		pw = pw * (float)enemy[1].hp / TEMPLE_HP;
		tw = tw * (float)enemy[1].hp / TEMPLE_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// 下敷きのゲージ（枠的な物）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy01]);

		//ゲージの位置やテクスチャー座標を反映
		px = 630.0f;	// ゲージの表示位置X
		py = 15.0f;		// ゲージの表示位置Y
		pw = 300.0f;	// ゲージの表示幅
		ph = 18.0f;		// ゲージの表示高さ

		tw = 1.0f;		// テクスチャの幅
		th = 1.0f;		// テクスチャの高さ
		tx = 0.0f;		// テクスチャの左上X座標
		ty = 0.0f;		// テクスチャの左上Y座標

	   // １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


	}

	// エネミーのHP(目)

	// マテリアル設定
	material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	SetMaterial(material);

	if(enemy[1].use == FALSE)
	{
		// ライフ
		//ゲージの位置やテクスチャー座標を反映
		float px = 650.0f;	// ゲージの表示位置X
		float py = 17.0f;	// ゲージの表示位置Y
		float pw = 250.0f;	// ゲージの表示幅
		float ph = 10.0f;	// ゲージの表示高さ
			  
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		pw = pw * (float)enemy[0].hp / ENEMY_HP;
		tw = tw * (float)enemy[0].hp / ENEMY_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		
		// 下敷きのゲージ（枠的な物）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy01]);

		//ゲージの位置やテクスチャー座標を反映
		 px = 630.0f;	// ゲージの表示位置X
		 py = 15.0f;	// ゲージの表示位置Y
		 pw = 300.0f;	// ゲージの表示幅
		 ph = 18.0f;	// ゲージの表示高さ

		 tw = 1.0f;	// テクスチャの幅
		 th = 1.0f;	// テクスチャの高さ
		 tx = 0.0f;	// テクスチャの左上X座標
		 ty = 0.0f;	// テクスチャの左上Y座標

	   // １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


	}


	// ボスのHP(目)

	// マテリアル設定
	material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	SetMaterial(material);

	if (boss->use == TRUE)
	{
		// ライフ
		//ゲージの位置やテクスチャー座標を反映
		float px = 650.0f;	// ゲージの表示位置X
		float py = 17.0f;	// ゲージの表示位置Y
		float pw = 250.0f;	// ゲージの表示幅
		float ph = 10.0f;	// ゲージの表示高さ

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		pw = pw * (float)boss->hp / ENEMYBOSS_HP;
		tw = tw * (float)boss->hp / ENEMYBOSS_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// 下敷きのゲージ（枠的な物）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[energy01]);

		//ゲージの位置やテクスチャー座標を反映
		px = 630.0f;	// ゲージの表示位置X
		py = 15.0f;	// ゲージの表示位置Y
		pw = 300.0f;	// ゲージの表示幅
		ph = 18.0f;	// ゲージの表示高さ

		tw = 1.0f;	// テクスチャの幅
		th = 1.0f;	// テクスチャの高さ
		tx = 0.0f;	// テクスチャの左上X座標
		ty = 0.0f;	// テクスチャの左上Y座標

	  // １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);
}


//=============================================================================
// スコアを加算する
// 引数:add :追加する点数。マイナスも可能
//=============================================================================
void AddScore(int add)
{
	g_Score += add;
	if (g_Score > SCORE_MAX)
	{
		g_Score = SCORE_MAX;
	}

}


int GetScore(void)
{
	return g_Score;
}

