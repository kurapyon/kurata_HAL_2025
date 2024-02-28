//=============================================================================
//
// エネミー処理 [enemy.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "enemy.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "map.h"
#include "bullet.h"
#include "bosbullet.h"
#include "fade.h"
#include "enemyeffect.h"
#include "enemybullet.h"
#include "input.h"
#include "bossatc.h"
#include "tutorialenemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


// テクスチャの数
#define TUTORIAL_TEXTURE_MAX						(5)		

// チュートリアルエネミー キャラサイズ
#define ENEMY_TEXTURE_WIDTH							(100)	
#define ENEMY_TEXTURE_HEIGHT						(100)	


// チュートリアルエネミー
#define TUTORIAL_TEXTURE_PATTERN_DIVIDE_X			(4)																		// アニメパターンのテクスチャ内分割数（X)
#define TUTORIAL_TEXTURE_PATTERN_DIVIDE_Y			(1)																		// アニメパターンのテクスチャ内分割数（Y)
#define TUTORIAL_ANIM_PATTERN_NUM					(TUTORIAL_TEXTURE_PATTERN_DIVIDE_X * TUTORIAL_TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define TUTORIAL_ANIM_WAIT							(4)																		// アニメーションの切り替わるWait値

// ライフ
#define TUTORIAL_ENEMY_LIFE							(100)

// 被ダメージに色を変えている時間
#define TEXTURE_COLOR_CHANGE_TIME					(200)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

// 
enum 
{
	tutorial00,
	tutorial01,
	tutorial02,
	tutorial03,

};
// エネミーの向き
enum
{
	ENEMY_DIR_LEFT,		// 左
	ENEMY_DIR_RIGHT,	// 右

};

// エネミーのバレットのテクスチャー
enum
{
	enemy_bullet00,
	enemy_bullet01,
};



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;						// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TUTORIAL_TEXTURE_MAX] = { NULL };	// テクスチャ情報

// 5
static char *g_TexturName[TUTORIAL_TEXTURE_MAX] = {
	"data/TEXTURE/tutorial00.png",
	"data/TEXTURE/tutorial01.png",
	"data/TEXTURE/tutorial02.png",
	"data/TEXTURE/tutorial03.png",
	"data/TEXTURE/tutorial05.png",

};

// 初期化を行ったかのフラグ
static BOOL		g_Load = FALSE;		

// エネミーの数を数える
int		g_tutorialCnt;

// チュートリアルエネミー構造体
static	TUTORIAL_ENEMY  g_tutorial[TUTORIAL_ENEMY_MAX];





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

	for (int i = 0; i < TUTORIAL_ENEMY_MAX; i++)
	{
		g_tutorial[i].use				= TRUE;
		g_tutorial[i].ans				= FALSE;
		g_tutorial[i].pos				= XMFLOAT3(500.0f + (i * 150.0f), 585.0f, 0.0f);		// 中心点から表示

		g_tutorial[i].rot				= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_tutorial[i].scl				= XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_tutorial[i].w					= ENEMY_TEXTURE_WIDTH;									// テクスチャーの幅
		g_tutorial[i].h					= ENEMY_TEXTURE_HEIGHT;									// テクスチャーの高さ
		g_tutorial[i].countAnim			= 0;
		g_tutorial[i].patternAnim		= 0;
		g_tutorial[i].dir				= ENEMY_DIR_LEFT;										// エネミーの向き(0:Xマイナス、左		1:Xプラス、右)
		g_tutorial[i].receive			= FALSE;												// 攻撃を受けたかどうかの判定
		g_tutorial[i].id				= i;													// 管理番号(ID)
		g_tutorial[i].move			    = XMFLOAT3(4.0f, 0.0f, 0.0f);							// 移動量
		g_tutorial[i].life				= 100;													// ライフ

		g_tutorial[i].time				= 0.0f;													// 線形補間用のタイマーをクリア
		g_tutorial[i].tblNo				= 0;													// 再生する行動データテーブルNoをセット
		g_tutorial[i].tblMax			= 0;													// 再生する行動データテーブルのレコード数をセット
		g_tutorial[i].receivewait		= 0;													// 被ダメージ時に色を変える時間
	}



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
	TUTORIAL_ENEMY_UPDATE();

	
#ifdef _DEBUG	// デバッグ情報を表示する


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


	PLAYER* player = GetPlayer();

	TUTORIAL_ENEMY_DRAW();

}


//=============================================================================
// エネミー用関数とボス関数
//=============================================================================
void TUTORIAL_ENEMY_UPDATE(void)
{
	BG* bg = GetBG();

	if (bg->pose == FALSE)
	{
		// 1ステージ目(その後ボス戦)
		for (int i = 0; i < TUTORIAL_ENEMY_MAX; i++)
		{
			// 生きてるエネミーだけ処理をする
			if (g_tutorial[i].use == TRUE)
			{
				// 地形との当たり判定用に座標のバックアップを取っておく
				XMFLOAT3 pos_old = g_tutorial[i].pos;

				// アニメーション  
				g_tutorial[i].countAnim += 1.0f;
				if (g_tutorial[i].countAnim > TUTORIAL_ANIM_WAIT)
				{
					g_tutorial[i].countAnim = 0.0f;
					// パターンの切り替え
					g_tutorial[i].patternAnim = (g_tutorial[i].patternAnim + 1) % TUTORIAL_ANIM_PATTERN_NUM;
				}


				// ボスの向きの調整
				PLAYER* player = GetPlayer();

				// ボスがプレイヤーのX座標より右にいたら,左に向かせる
				if (player[0].pos.x > g_tutorial[i].pos.x)
				{
					g_tutorial[i].dir = ENEMY_DIR_RIGHT;
				}

				// もしボスがプレイヤーのX座標より左にいたら、右に向かせる
				if (player[0].pos.x < g_tutorial[i].pos.x)
				{
					g_tutorial[i].dir = ENEMY_DIR_LEFT;
				}

				// プレイヤーとの当たり判定、エネミーの攻撃
				{
					PLAYER* player = GetPlayer();

					// エネミーの数分判定する
					for (int j = 0; j < TUTORIAL_ENEMY_MAX; j++)
					{
						// プレイヤーと当たり判定をする
						if (player[j].use == TRUE && player[j].invincibleCnt >= PLAYER_INVINCIBLE_FRAME || player[j].invincibleCnt == 0)
						{
							g_tutorial[i].ans = CollisionBB(g_tutorial[i].pos, g_tutorial[i].w, g_tutorial[i].h,
								player[j].pos, player[j].w, player[j].h);

							// 当たっている？
							if (g_tutorial[i].ans == TRUE)
							{
								// 当たった時の処理

								SetDamage(10);

								player[i].receive = TRUE;

								player[j].isDamaged = TRUE;

							}
						}

					}

				}

				//g_tutorialCnt++;
			}


		}

	}


	// エネミーを全部倒したら、チュートリアル終わり
	if (g_tutorial[0].use == FALSE && g_tutorial[1].use == FALSE && g_tutorial[2].use == FALSE && g_tutorial[3].use == FALSE)
	{
		SetFadeMode(FADE_OUT, MODE_GAME);
	}


}



void TUTORIAL_ENEMY_DRAW(void)
{

	float scrollX = GetScrollX();
	float scrollY = GetScrollY();

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

	//エネミーの位置やテクスチャー座標を反映
	float px = 800.0f  - scrollX;								// エネミーの表示位置X
	float py = 300.0f  - scrollY;								// エネミーの表示位置Y
	float pw = 800.0f;								// エネミーの表示幅
	float ph = 400.0f;								// エネミーの表示高さ

	 //アニメーション用
	float tw = 1.0f;								// テクスチャの幅
	float th = 1.0f;								// テクスチャの高さ
	float tx = 0.0f;								// テクスチャの左上X座標
	float ty = 0.0f;								// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	for (int i = 0; i < TUTORIAL_ENEMY_MAX; i++)
	{
		if (g_tutorial[i].use == TRUE)
		{
			// エネミー
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_tutorial[i].pos.x - scrollX;	// エネミーの表示位置X
			float py = g_tutorial[i].pos.y - scrollY;	// エネミーの表示位置Y
			float pw = g_tutorial[i].w;					// エネミーの表示幅
			float ph = g_tutorial[i].h;					// エネミーの表示高さ

			 //アニメーション用
			float tw = -1 * (1.0f / TUTORIAL_TEXTURE_PATTERN_DIVIDE_X);									// テクスチャの幅
			float th = 1.0f / TUTORIAL_TEXTURE_PATTERN_DIVIDE_Y;									// テクスチャの高さ
			float tx = (float)(g_tutorial[i].patternAnim % TUTORIAL_TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_tutorial[i].patternAnim / TUTORIAL_TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

				// もしプレイヤーの左にいたら反転させる
			if (g_tutorial[i].dir == ENEMY_DIR_LEFT)
			{
				// -1をかけると反転
				tw *= -1;

				// 表示位置をずれないようにしている
				tx += 1.0f / TUTORIAL_TEXTURE_PATTERN_DIVIDE_X;

			}

			// 攻撃を受けたら色を変える
			
			// 通常時
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_tutorial[i].rot.z);


			g_tutorial[i].receivewait++;

			// もし、攻撃を受けたら赤色にする
			if (g_tutorial[i].receive == TRUE)
			{
				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
					g_tutorial[i].rot.z);
				
				// もしカラーチェンジタイムを超えたら色を戻す
				if (g_tutorial[i].receivewait >= TEXTURE_COLOR_CHANGE_TIME)
				{
					// 元の色に戻す
					g_tutorial[i].receive = FALSE;
					g_tutorial[i].receivewait += 0;

				}
			}

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);




		}
	}





}







//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
TUTORIAL_ENEMY* GetTutorial(void)
{
	return &g_tutorial[0];
}


