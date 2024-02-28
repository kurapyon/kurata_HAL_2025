//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "Level.h"
#include "bg.h"
#include "playereffect.h"
#include "player.h"
#include "cut.h"
#include "tutorialenemy.h"
#include "map.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// 斬撃サイズ
#define TEXTURE_HEIGHT				(100)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(5)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値
#define COOLTIME					(25)	// 攻撃の間隔

#define ATCPAWER					(2)	// 攻撃力


enum
{
	CUTTING_DIR_UP,
	CUTTING_DIR_RIGHT,
	CUTTING_DIR_DOWN,
	CUTTING_DIR_LEFT,
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/cutting.png"
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static CUTTING	g_Cutting[CUTTING_MAX];	// バレット構造体

int cooltime = COOLTIME;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCutting(void)
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


	// バレット構造体の初期化
	for (int i = 0; i < CUTTING_MAX; i++)
	{
		g_Cutting[i].use   = FALSE;			// 未使用（使われていない斬撃）
		g_Cutting[i].w     = TEXTURE_WIDTH;
		g_Cutting[i].h     = TEXTURE_HEIGHT;
		g_Cutting[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_Cutting[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Cutting[i].texNo = 1;

		g_Cutting[i].countAnim = 0;
		g_Cutting[i].patternAnim = 0;

		g_Cutting[i].move = XMFLOAT3(0.0f, -CUTTING_SPEED, 0.0f);	// 移動量を初期化
		g_Cutting[i].xmove = XMFLOAT3(-CUTTING_SPEED, 0.0f, 0.0f);	//斬撃横向き用の移動量初期化
		g_Cutting[i].bulletlife = 0;
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCutting(void)
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
// 更新処理
//=============================================================================
void UpdateCutting(void)
{
	if (cooltime < COOLTIME)
	{
		cooltime ++;
	}


	PLAYER* player = GetPlayer();
	CUTTING* cutting = GetCutting();

	int cuttingCount = 0;				// 処理したバレットの数

	for (int i = 0; i < CUTTING_MAX; i++)
	{
		if (g_Cutting[i].use == TRUE)	// このバレットが使われている？
		{				
			
				g_Cutting[i].bulletlife--;
				if (g_Cutting[i].bulletlife == 0)
				{
					g_Cutting[i].use = FALSE;
				}

			

			
			// Yes
			// アニメーション  
			g_Cutting[i].countAnim++;
			if ((g_Cutting[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				g_Cutting[i].patternAnim = (g_Cutting[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			if (g_Cutting[i].dir == CUTTING_DIR_DOWN)
			{
				// バレットの移動処理
				XMVECTOR pos = XMLoadFloat3(&g_Cutting[i].pos) ;

				if (g_Cutting[i].bulletmode == 1)
				{
					g_Cutting[i].move = XMFLOAT3(0.0f,CUTTING_SPEED,0.0f);
					XMVECTOR move = XMLoadFloat3(&g_Cutting[i].move);
					pos += move;
				}

				XMStoreFloat3(&g_Cutting[i].pos, pos);
			}
			else if (g_Cutting[i].dir == CUTTING_DIR_UP)
			{
				// バレットの移動処理
				XMVECTOR pos = XMLoadFloat3(&g_Cutting[i].pos);
				//XMVECTOR move = XMLoadFloat3(&g_Cutting[i].move);
				//pos += move;


				if (g_Cutting[i].bulletmode == 1)
				{
					g_Cutting[i].move = XMFLOAT3(0.0f, -CUTTING_SPEED, 0.0f);
					XMVECTOR move = XMLoadFloat3(&g_Cutting[i].move);
					pos += move;
				}


				XMStoreFloat3(&g_Cutting[i].pos, pos);
			}
			else if (g_Cutting[i].dir == CUTTING_DIR_LEFT)
			{
				XMVECTOR pos = XMLoadFloat3(&g_Cutting[i].pos);
				//XMVECTOR xmove = XMLoadFloat3(&g_Cutting[i].xmove);
				//pos += xmove;
				


				if (g_Cutting[i].bulletmode == 1)
				{
					g_Cutting[i].move = XMFLOAT3(-CUTTING_SPEED,1.0f, 0.0f);
					XMVECTOR move = XMLoadFloat3(&g_Cutting[i].move);
					pos += move;
				}

				XMStoreFloat3(&g_Cutting[i].pos, pos);
			}
			else if (g_Cutting[i].dir == CUTTING_DIR_RIGHT)
			{
				XMVECTOR pos = XMLoadFloat3(&g_Cutting[i].pos);
				//XMVECTOR xmove = XMLoadFloat3(&g_Cutting[i].xmove);
				//
				//pos -= xmove;


				if (g_Cutting[i].bulletmode == 1)
				{
					g_Cutting[i].move = XMFLOAT3(CUTTING_SPEED, 1.0f, 0.0f);
					XMVECTOR move = XMLoadFloat3(&g_Cutting[i].move);
					pos += move;
				}

				XMStoreFloat3(&g_Cutting[i].pos, pos);
			}



			// 画面外まで進んだ？
			BG* bg = GetBG();
			if (g_Cutting[i].pos.y < (-g_Cutting[i].h/2))		// 自分の大きさを考慮して画面外か判定している
			{
				g_Cutting[i].use = FALSE;
			}
			if (g_Cutting[i].pos.y > (bg->h + g_Cutting[i].h/2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_Cutting[i].use = FALSE;
			}




			// 当たり判定処理
			{
				ENEMY* enemy = GetEnemy();

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (enemy[j].use == TRUE)
					{
							BOOL ans = CollisionBB(g_Cutting[i].pos, g_Cutting[i].w, g_Cutting[i].h,
										enemy[j].pos, enemy[j].w, enemy[j].h);

							if (ans == TRUE)
							{
								// 攻撃音
								PlaySound(SOUND_LABEL_cut01);

								// 当たった時の処理(エネミーのライフを減らす,攻撃を受けたフラグを立てる)
								enemy[j].life -= ATCPAWER;

								// エネミーの色を変える
								enemy[j].receive = TRUE;

								// 経験値を加える
								GuagePlus(1);

							}

							// もし、エネミーのライフが0ならエネミーを消す
							if (enemy[j].life <= 0)
							{
								enemy[j].use = FALSE;

							}


						//}
					}
				}

				BOSS* boss = GetBoss();
					// 生きてるボスと当たり判定をする
					if (boss[0].boss_use == TRUE)
					{

							BOOL ans_boss = CollisionBB(g_Cutting[i].pos, g_Cutting[i].w, g_Cutting[i].h,
								boss[0].boss_pos, boss[0].boss_w, boss[0].boss_h);

							if (ans_boss == TRUE)
							{
								PlaySound(SOUND_LABEL_cut01);

								// 当たった時の処理
								boss[0].boss_life -= ATCPAWER;

								// ボスの色を変える
								boss[0].boss_receive = TRUE;

								// 経験値を増やす
								GuagePlus(1);


							}

							// もし、エネミーのライフが0ならエネミーを消す
							if (boss[0].boss_life <= 0)
							{
								boss[0].boss_use = FALSE;

							}


					}

					TUTORIAL_ENEMY* tutorial = GetTutorial();
					for (int t = 0; t < TUTORIAL_ENEMY_MAX; t++)
					{
						// 生きてるエネミーと当たり判定をする
						if (tutorial[t].use == TRUE)
						{
							BOOL ans_tutorial = CollisionBB(g_Cutting[i].pos, g_Cutting[i].w, g_Cutting[i].h,
								tutorial[t].pos, tutorial[t].w, tutorial[t].h);

							// 当たっている？
							if (ans_tutorial == TRUE)
							{
								PlaySound(SOUND_LABEL_cut01);

								// 当たった時の処理(エネミーのライフを減らす,攻撃を受けたフラグを上げる)
								tutorial[t].life -= ATCPAWER;

								tutorial[t].receive = TRUE;

								// もし、エネミーのライフがtならエネミーを消す
								if (tutorial[t].life <= 0)
								{
									tutorial[t].use = FALSE;

								}

							}
						}

					}


			}


			cuttingCount++;
		}
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCutting(void)	// カット演出はアニメーション内で行う。エフェクトは付ける
{
	// なし
}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
CUTTING *GetCutting(void)
{
	return &g_Cutting[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetCutting(XMFLOAT3 pos)
{
	PLAYER* player = GetPlayer();

	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < CUTTING_MAX; i++)
	{
		if (g_Cutting[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			if (cooltime != COOLTIME)
			{
				return;
			}

				g_Cutting[i].bulletlife = 20;
			cooltime = 0;
			g_Cutting[i].use = TRUE;		// 使用状態へ変更する
			g_Cutting[i].dir = CUTTING_DIR_UP;

			if (player[0].dir == PLAYER_DIR_UP)	// プレイヤーの向きによって変える
			{

				g_Cutting[i].pos = pos;			// 座標をセット
				g_Cutting[i].dir = CUTTING_DIR_UP;
				g_Cutting[i].rot = XMFLOAT3(0.0f, 0.0f, 3.14f * 0.5f);

			}
			else if (player[0].dir == PLAYER_DIR_DOWN)
			{

				g_Cutting[i].pos = pos;			// 座標をセット
				g_Cutting[i].dir = CUTTING_DIR_DOWN;
				g_Cutting[i].rot = XMFLOAT3(0.0f, 0.0f, 3.14f * 1.5f);

			}
			else if (player[0].dir == PLAYER_DIR_LEFT)
			{
				pos.x -= TEXTURE_WIDTH / 2;
				g_Cutting[i].pos = pos;			// 座標をセット
				g_Cutting[i].dir = CUTTING_DIR_LEFT;
				g_Cutting[i].rot = XMFLOAT3(0.0f, 0.0f,0.0f);

			}
			else if (player[0].dir == PLAYER_DIR_RIGHT)
			{
				pos.x += TEXTURE_WIDTH / 2;
				g_Cutting[i].pos = pos;			// 座標をセット
				g_Cutting[i].dir = CUTTING_DIR_RIGHT;
				g_Cutting[i].rot = XMFLOAT3(0.0f, 0.0f, 3.14f);

			}
			return;							// 1発セットしたので終了する
		}
	}
}

