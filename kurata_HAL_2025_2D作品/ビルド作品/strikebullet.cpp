//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "player.h"
#include "enemy.h"
#include "collision.h"
#include "Level.h"
#include "bg.h"
#include "strikebullet.h"
#include "map.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define STRIKE_TEXTURE_WIDTH				(100)			// キャラサイズ
#define STRIKE_TEXTURE_HEIGHT				(100)			// 
#define STRIKE_TEXTURE_MAX					(2)				// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X			(1)				// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y			(1)				// アニメパターンのテクスチャ内分割数（Y)
#define STRIKE_ANIM_PATTERN_NUM				(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数

#define TEXTURE_PATTERN_BULLET_X			(4)				// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_BULLET_Y			(1)				// アニメパターンのテクスチャ内分割数（Y)
#define STRIKE_ANIM_BULLET_NUM				(TEXTURE_PATTERN_BULLET_X*TEXTURE_PATTERN_BULLET_Y)	// アニメーションパターン数

#define STRIKE_ANIM_WAIT					(4)				// アニメーションの切り替わるWait値

#define BULLET_WAIT							(60)			// バレットの発射のタイミングを合わせている
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
enum 
{
	enemy_bullet00,
	enemy_bullet01,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[STRIKE_TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[STRIKE_TEXTURE_MAX] = {
	"data/TEXTURE/enemy_bullet00.png",
	"data/TEXTURE/enemy_bullet01.png",

};

static BOOL		g_Load = FALSE;					// 初期化を行ったかのフラグ
static STRIKE	g_strike[STRIKE_BULLET_MAX];	// バレット構造体

float			g_bullettime = 0.0f;			// 時間計測

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStrikeBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < STRIKE_TEXTURE_MAX; i++)
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
	for (int i = 0; i < STRIKE_BULLET_MAX; i++)
	{
		g_strike[i].strike_use			= FALSE;								// 未使用（発射されていない弾）
		g_strike[i].strike_w			= STRIKE_TEXTURE_WIDTH;
		g_strike[i].strike_h			= STRIKE_TEXTURE_HEIGHT;
		g_strike[i].strike_pos			= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_strike[i].strike_rot			= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_strike[i].strike_P0			= XMFLOAT3(0.0f, 0.0f, 0.0f);			// 制御点P0
		g_strike[i].strike_P1			= XMFLOAT3(0.0f, 0.0f, 0.0f);			// 制御点P1
		g_strike[i].strike_speed		= 0.0f;
		g_strike[i].strike_countAnim	= 0;
		g_strike[i].strike_patternAnim  = 0;
		g_strike[i].strike_move			= XMFLOAT3(0.0f, - STRIKE_BULLET_SPEED, 0.0f);	// 移動量を初期化
		g_strike[i].strike_hit			= 0;											// 最初はプレイヤー
		g_strike[i].strike_tex			= 0;											// テクスチャーの番号
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStrikeBullet(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < STRIKE_TEXTURE_MAX; i++)
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
void UpdateStrikeBullet(void)
{
	int bulletCount = 0;				// 処理したバレットの数

	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

	for (int i = 0; i < STRIKE_BULLET_MAX; i++)
	{
		if (g_strike[i].strike_use == TRUE)	// このバレットが使われている？
		{								// Yes
			// テクスチャーによって分けている
			if (g_strike[i].strike_tex == enemy_bullet00)
			{
				// アニメーション  
				g_strike[i].strike_countAnim++;
				if ((g_strike[i].strike_countAnim % STRIKE_ANIM_WAIT) == 0)
				{
					// パターンの切り替え
					g_strike[i].strike_patternAnim = (g_strike[i].strike_patternAnim + 1) % STRIKE_ANIM_PATTERN_NUM;
				}

			}

			// テクスチャーによって分けている
			if (g_strike[i].strike_tex == enemy_bullet01)
			{
				// アニメーション  
				g_strike[i].strike_countAnim++;
				if ((g_strike[i].strike_countAnim % STRIKE_ANIM_WAIT) == 0)
				{
					// パターンの切り替え
					g_strike[i].strike_patternAnim = (g_strike[i].strike_patternAnim + 1) % STRIKE_ANIM_BULLET_NUM;
				}

			}


			XMVECTOR vold_pos = XMLoadFloat3(&g_strike[i].strike_pos);			
			// バレットの移動処理
			XMVECTOR pos = XMLoadFloat3(&g_strike[i].strike_pos);
			XMVECTOR move = XMLoadFloat3(&g_strike[i].strike_move);
			pos += move;
			XMStoreFloat3(&g_strike[i].strike_pos, pos);



			// 画面外まで進んだ？
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			if (g_strike[i].strike_pos.y < (-g_strike[i].strike_h/2))		// 自分の大きさを考慮して画面外か判定している
			{
				g_strike[i].strike_use = false;
			}
			if (g_strike[i].strike_pos.y > (FIELD_HEIGHT + g_strike[i].strike_h/2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_strike[i].strike_use = false;
			}


			// 当たり判定処理(エネミー)
			{
				ENEMY* enemy = GetEnemy();
				PLAYER* player = GetPlayer();

				// 当たり判定処理
				// プレイヤーの数分当たり判定を行う
				for (int p = 0; p < ENEMY_MAX; p++)
				{
					// 生きてるエネミーと当たり判定をする
					if (player[p].use == TRUE)
					{
						BOOL ans = CollisionBB(player[p].pos, player[p].w, player[p].h, g_strike[i].strike_pos, g_strike[i].strike_w, g_strike[i].strike_h);

						// 当たっている？
						if (ans == TRUE)
						{
							// プレイヤーが守り状態ではない時かつ、プレイヤーが無敵時間外ならダメージを与える
							if (player[p].defense == FALSE && (player[p].invincibleCnt >= PLAYER_INVINCIBLE_FRAME || player[p].invincibleCnt == 0))
							{
								{
									// 当たった時の処理
									SetDamage(1);

									player[p].receive = TRUE;

									g_strike[i].strike_use = FALSE;

									// エフェクト発生
									//SetEnemyEffect(player[p].pos.x, player[p].pos.y, 30, 0);
								}
							}

							// プレイヤーが守り状態なら、FALSEにする
							if (player[p].defense == TRUE)
							{
								g_strike[i].strike_use = FALSE;
							}
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
void DrawStrikeBullet(void)
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

	for (int i = 0; i < STRIKE_BULLET_MAX; i++)
	{
		if (g_strike[i].strike_use == TRUE)		// このバレットが使われている？
		{										// Yes

			if (g_strike[i].strike_tex == enemy_bullet00)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy_bullet00]);

				//バレットの位置やテクスチャー座標を反映
				float px = g_strike[i].strike_pos.x - scrollX;	// バレットの表示位置X
				float py = g_strike[i].strike_pos.y - scrollY;	// バレットの表示位置Y
				float pw = g_strike[i].strike_w;				// バレットの表示幅
				float ph = g_strike[i].strike_h;				// バレットの表示高さ

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				float tx = (float)(g_strike[i].strike_patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
				float ty = (float)(g_strike[i].strike_patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

				// テクスチャー上下反転処理
				tht = th * -1;

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, tht,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_strike[i].strike_rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}

			if (g_strike[i].strike_tex == enemy_bullet01)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy_bullet01]);

				//バレットの位置やテクスチャー座標を反映
				float px = g_strike[i].strike_pos.x - scrollX;	// バレットの表示位置X
				float py = g_strike[i].strike_pos.y - scrollY;	// バレットの表示位置Y
				float pw = g_strike[i].strike_w;				// バレットの表示幅
				float ph = g_strike[i].strike_h;				// バレットの表示高さ

				float tw = 1.0f / TEXTURE_PATTERN_BULLET_X;	// テクスチャの幅
				float th = 1.0f / TEXTURE_PATTERN_BULLET_Y;	// テクスチャの高さ
				float tx = (float)(g_strike[i].strike_patternAnim % TEXTURE_PATTERN_BULLET_X) * tw;	// テクスチャの左上X座標
				float ty = (float)(g_strike[i].strike_patternAnim / TEXTURE_PATTERN_BULLET_X) * th;	// テクスチャの左上Y座標

				// テクスチャー上下反転処理
				/*tht = th * -1;*/

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_strike[i].strike_rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}

		}
	}
	
}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
STRIKE *GetStrikeBullet(void)
{
	return &g_strike[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetStrikeBullet(XMFLOAT3 pos, int tex)
{
		// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
		for (int i = 0; i < STRIKE_BULLET_MAX; i++)
		{
			// スピード
			g_strike[i].strike_speed = STRIKE_BULLET_SPEED;

			// テクスチャーの番号
			g_strike[i].strike_tex = tex;

			if (g_strike[i].strike_use == FALSE)		// 未使用状態のバレットを見つける
			{
				g_strike[i].strike_use = TRUE;			// 使用状態へ変更する
				g_strike[i].strike_pos = pos;			// 座標をセット

				// 上下８方向に飛ばす
				g_strike[i].strike_move.x = sinf(XM_PI / 4 * i) * g_strike[i].strike_speed;
				g_strike[i].strike_move.y = cosf(XM_PI / 4 * i) * g_strike[i].strike_speed;

				return;									// 1発セットしたので終了する
			}
		}
}


