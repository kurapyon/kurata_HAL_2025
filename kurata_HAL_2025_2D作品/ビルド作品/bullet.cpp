//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "bullet.h"
#include "player.h"
#include "enemy.h"
#include "collision.h"
#include "Level.h"
#include "bg.h"
#include "playereffect.h"
#include "map.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// キャラサイズ

#define TEXTURE_HEIGHT				(100)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(8)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
enum 
{
	player_bullet,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/player_bullet.png",

};

static BOOL		g_Load = FALSE;					// 初期化を行ったかのフラグ
static BULLET	g_Bullet[PLAYER_BULLET_MAX];	// バレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
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
	for (int i = 0; i < PLAYER_BULLET_MAX; i++)
	{
		g_Bullet[i].use			= FALSE;								// 未使用（発射されていない弾）
		g_Bullet[i].w			= TEXTURE_WIDTH;
		g_Bullet[i].h			= TEXTURE_HEIGHT;
		g_Bullet[i].pos			= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].rot			= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo		= 0;
		g_Bullet[i].P0			= XMFLOAT3(0.0f, 0.0f, 0.0f);			// 制御点P0
		g_Bullet[i].P1			= XMFLOAT3(0.0f, 0.0f, 0.0f);			// 制御点P1
		g_Bullet[i].s			= 0.0f;
		g_Bullet[i].countAnim	= 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move = XMFLOAT3(0.0f, -PLAYER_BULLET_SPEED, 0.0f);	// 移動量を初期化
		
		g_Bullet[i].hit = 0;											// 最初はプレイヤー
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
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
void UpdateBullet(void)
{
	int bulletCount = 0;				// 処理したバレットの数

	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

	for (int i = 0; i < PLAYER_BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)	// このバレットが使われている？
		{								// Yes
			// アニメーション  
			g_Bullet[i].countAnim++;
			if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}


			XMVECTOR vold_pos = XMLoadFloat3(&g_Bullet[i].pos);


			
			// バレットの移動処理
			XMVECTOR pos = XMLoadFloat3(&g_Bullet[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_Bullet[i].move);
			pos += move;
			XMStoreFloat3(&g_Bullet[i].pos, pos);



			// 画面外まで進んだ？
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			if (g_Bullet[i].pos.y < (-g_Bullet[i].h/2))		// 自分の大きさを考慮して画面外か判定している
			{
				g_Bullet[i].use = false;
			}
			if (g_Bullet[i].pos.y > (FIELD_HEIGHT + g_Bullet[i].h/2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_Bullet[i].use = false;
			}


			// 当たり判定処理(エネミー)
			{
				ENEMY* enemy = GetEnemy();
				PLAYER* player = GetPlayer();

				// エネミーの数分当たり判定を行う
				// 生きてるエネミーと当たり判定をする
					if (enemy[0].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							enemy[0].pos, enemy[0].w, enemy[0].h);
						// 当たっている？
						if (ans == TRUE)
						{

							enemy[0].life -= 10;

							// 経験値追加
							GuagePlus(1);

							// エフェクト発生
							SetPlayerEffect(enemy[0].pos.x, enemy[0].pos.y, 30, TEXTURE_EFFECT01);
						}

						// もしエネミーの体力が0になったら
						if (enemy[0].life <= 0)
						{
							enemy[0].use = FALSE;
						}
					}
			}


			// 当たり判定処理(Boos)
			{
				BOSS* boss = GetBoss();
				PLAYER* player  = GetPlayer();

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < BOSS_MAX; j++)
				{
						// 生きてるエネミーと当たり判定をする
						if (boss[j].boss_use == TRUE)
						{
							BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
								boss[j].boss_pos, boss[j].boss_w, boss[j].boss_h);
							// 当たっている？
							if (ans == TRUE)
							{
								// 当たった時の処理
								boss[j].boss_life -= 5;
								
								// 経験値追加
								GuagePlus(1);


								// エフェクト発生
								SetPlayerEffect(boss[j].boss_pos.x, boss[j].boss_pos.y, 30, TEXTURE_EFFECT01);
							}

							// もしボスの体力が0になったら
							if (boss[j].boss_life <= 0)
							{
								boss[j].boss_use = FALSE;


							}
						}
				}
			}
			bulletCount++;
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
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

	float scrollX = GetScrollX();
	float scrollY = GetScrollY();
	float tht;

	for (int i = 0; i < PLAYER_BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[player_bullet]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Bullet[i].pos.x - scrollX;	// バレットの表示位置X
			float py = g_Bullet[i].pos.y - scrollY;	// バレットの表示位置Y
			float pw = g_Bullet[i].w;				// バレットの表示幅
			float ph = g_Bullet[i].h;				// バレットの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// テクスチャー上下反転処理
			tht = th * -1;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, 
				px, py, pw, ph, 
				tx, ty, tw, tht,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_Bullet[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetBullet(XMFLOAT3 pos)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < PLAYER_BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Bullet[i].use = TRUE;			// 使用状態へ変更する
			g_Bullet[i].pos = pos;			// 座標をセット
			return;							// 1発セットしたので終了する
		}
	}
}


