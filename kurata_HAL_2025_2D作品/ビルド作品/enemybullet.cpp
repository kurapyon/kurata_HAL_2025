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
#include "enemyeffect.h"
#include "map.h"
#include "bosbullet.h"
#include "fade.h"
#include "enemybullet.h"
#include "input.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMY_BULLET_TEXTURE_WIDTH								(100)																			// キャラサイズ
#define ENEMY_BULLET_TEXTURE_HEIGHT								(100)																			// 
#define ENEMY_BULLET_TEXTURE_MAX								(2)																				// テクスチャの数
#define ENEMY_BULLET_SPEED										(10.0f)																			// スピード

// 弾を消す時間
#define ENEMY_BULLET_TIME										(ENEMY_WAIT)

// 青玉バレット
#define TEXTURE_PATTERN_DIVIDE_X								(1)																				// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y								(1)																				// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM										(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)								// アニメーションパターン数
#define ANIM_WAIT												(4)																				// アニメーションの切り替わるWait値

// エネミーバレット
#define ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_X					(4)																				// アニメパターンのテクスチャ内分割数（X)
#define ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_Y					(1)																				// アニメパターンのテクスチャ内分割数（Y)
#define ENEMY_BULLET_ANIM_PATTERN_NUM							(ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_X * ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ENEMY_BULLET_ANIM_WAIT									(4)																				// アニメーションの切り替わるWait値

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
static ID3D11ShaderResourceView	*g_Texture[ENEMY_BULLET_TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/enemy_bullet00.png",
	"data/TEXTURE/enemy_bullet01.png",
};

static BOOL		g_Load = FALSE;							// 初期化を行ったかのフラグ
static ENEMYBULLETE	g_Bulletenemy[ENEMY_BULLETE_MAX];	// バレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemyBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < ENEMY_BULLET_TEXTURE_MAX; i++)
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
	for (int i = 0; i < ENEMY_BULLETE_MAX; i++)
	{
		g_Bulletenemy[i].use		 = FALSE;									// 未使用（発射されていない弾）
		g_Bulletenemy[i].w			 = ENEMY_BULLET_TEXTURE_WIDTH;
		g_Bulletenemy[i].h			 = ENEMY_BULLET_TEXTURE_HEIGHT;
		g_Bulletenemy[i].pos		 = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bulletenemy[i].rot		 = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bulletenemy[i].texNo		 = 0;
		g_Bulletenemy[i].countAnim	 = 0;
		g_Bulletenemy[i].patternAnim = 0;
		g_Bulletenemy[i].move		 = XMFLOAT3(0.0f, 0.0f, 0.0f);				// 移動量を初期化(バレットの飛ぶ方向)
		g_Bulletenemy[i].hit		 = 0;										// 最初はプレイヤー
		g_Bulletenemy[i].tex		 = 0;										// テクスチャーの識別
		g_Bulletenemy[i].time		 = 0;										// 弾が発射されてからの時間

	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemyBullet(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < ENEMY_BULLET_TEXTURE_MAX; i++)
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
void UpdateEnemyBullet(void)
{
	int bulletCount = 0;				// 処理したバレットの数

	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();


	for (int i = 0; i < ENEMY_BULLETE_MAX; i++)
	{
		// バレットのスピードの初期化
		//(ここで初期化しないと弾が段々速くなってしまうため)
		g_Bulletenemy[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);

		if (g_Bulletenemy[i].use == TRUE)	// このバレットが使われている？
		{									// Yes

			// テクスチャーの識別
			if (g_Bulletenemy == 0)
			{
				// アニメーション  
				g_Bulletenemy[i].countAnim++;
				if ((g_Bulletenemy[i].countAnim % ANIM_WAIT) == 0)
				{
					// パターンの切り替え
					g_Bulletenemy[i].patternAnim = (g_Bulletenemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}

			}

			if (g_Bulletenemy[i].tex == 1)
			{
				// アニメーション  
				g_Bulletenemy[i].countAnim++;
				if ((g_Bulletenemy[i].countAnim % ENEMY_BULLET_ANIM_WAIT) == 0)
				{
					// パターンの切り替え
					g_Bulletenemy[i].patternAnim = (g_Bulletenemy[i].patternAnim + 1) % ENEMY_BULLET_ANIM_PATTERN_NUM;
				}

			}

			for(int homing = 0; homing <= 60; homing++)
			{
				// バレットのしっかりしたホーミング

				PLAYER* player = GetPlayer();

				//計算したい情報を変換して、最後に元に戻す。
				XMVECTOR epos = XMLoadFloat3(&g_Bulletenemy[i].pos);

				// 一回で(x.y.z)を計算できる。自分とプレイヤーの差を求めている
				XMVECTOR vec = XMLoadFloat3(&player[0].pos_old) - epos;

				// 角度を求めている
				float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);

				// スピードを求めている
				float speed = ENEMY_BULLET_SPEED;

				// コサインはX、サインはY
				g_Bulletenemy[i].pos.x += cosf(angle) * speed;
				g_Bulletenemy[i].pos.y += sinf(angle) * speed;

				// 回転させている
				g_Bulletenemy[i].rot.z = angle / 4;

				// バレットの移動処理
				XMVECTOR pos = XMLoadFloat3(&g_Bulletenemy[i].pos);
				XMVECTOR move = XMLoadFloat3(&g_Bulletenemy[i].move);
				pos += move;
				XMStoreFloat3(&g_Bulletenemy[i].pos, pos);

			}




			// 画面外まで進んだ？
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			if (g_Bulletenemy[i].pos.y < (-g_Bulletenemy[i].h/2))		// 自分の大きさを考慮して画面外か判定している
			{	
				g_Bulletenemy[i].use = false;
			}	
			if (g_Bulletenemy[i].pos.y > (FIELD_HEIGHT + g_Bulletenemy[i].h/2))	// 自分の大きさを考慮して画面外か判定している
			{	
				g_Bulletenemy[i].use = false;
			}


				// 攻撃処理
				// プレイヤーの数分当たり判定を行う
				for (int p = 0; p < ENEMY_MAX; p++)
				{
					// 生きてるプレイヤーと当たり判定をする
					if (player[p].use == TRUE)
					{
						// 当たり判定
						BOOL ans = CollisionBB(player[p].pos, player[p].w, player[p].h, g_Bulletenemy[i].pos, g_Bulletenemy[i].w, g_Bulletenemy[i].h);

						// 当たっている？
						if (ans == TRUE)
						{
							// プレイヤーが守り状態ではないかつ、無敵時間外ならダメージを与える
							if (player[p].defense == FALSE && (player[p].invincibleCnt >= PLAYER_INVINCIBLE_FRAME || player[p].invincibleCnt == 0))
							{

								// 当たった時の処理(ダメージを与える)
								SetDamage(10);

								// プレイヤーに当たったらTRUEにする
								player[p].receive = TRUE;

								// バレットを未使用にする
								g_Bulletenemy[i].use = FALSE;

								// エフェクト発生
								SetEnemyEffect(player[p].pos.x, player[p].pos.y, 30, TEXTURE_EFFECT00);
							}

							// プレイヤーが守り状態の時に当たったらFALSEにする
							if (player[p].defense == TRUE)
							{
								g_Bulletenemy[i].use = FALSE;
							}
						}
					}
				}

				// もし一定時間過ぎたらFALSEする
				g_Bulletenemy[i].time++;

				if (g_Bulletenemy[i].time >= ENEMY_BULLET_TIME)
				{
					g_Bulletenemy[i].use = FALSE;

					g_Bulletenemy[i].time = 0;
				}

			bulletCount++;
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemyBullet(void)
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

	for (int i = 0; i < ENEMY_BULLETE_MAX; i++)
	{
		if (g_Bulletenemy[i].tex == 0)
		{

			// ０番なら
			if (g_Bulletenemy[i].use == TRUE)		// このバレットが使われている？
			{									// Yes
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy_bullet00]);

				//バレットの位置やテクスチャー座標を反映
				float px = g_Bulletenemy[i].pos.x - scrollX;	// バレットの表示位置X
				float py = g_Bulletenemy[i].pos.y - scrollY;	// バレットの表示位置Y
				float pw = g_Bulletenemy[i].w;		// バレットの表示幅
				float ph = g_Bulletenemy[i].h;		// バレットの表示高さ

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				float tx = (float)(g_Bulletenemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
				float ty = (float)(g_Bulletenemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Bulletenemy[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

		}

		// １番なら
		if (g_Bulletenemy[i].tex == 1)
		{
			if (g_Bulletenemy[i].use == TRUE)		// このバレットが使われている？
			{									// Yes
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy_bullet01]);

				//バレットの位置やテクスチャー座標を反映
				float px = g_Bulletenemy[i].pos.x - scrollX;	// バレットの表示位置X
				float py = g_Bulletenemy[i].pos.y - scrollY;	// バレットの表示位置Y
				float pw = g_Bulletenemy[i].w;		// バレットの表示幅
				float ph = g_Bulletenemy[i].h;		// バレットの表示高さ

				float tw = 1.0f / ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
				float th = 1.0f / ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				float tx = (float)(g_Bulletenemy[i].patternAnim % ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
				float ty = (float)(g_Bulletenemy[i].patternAnim / ENEMY_BULLET_TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Bulletenemy[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

		}

	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
ENEMYBULLETE *GetEnemyBullet(void)
{
	return &g_Bulletenemy[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetEnemyBullet(XMFLOAT3 enemypos, BOOL tex)
{

	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < ENEMY_BULLETE_MAX; i++)
	{

		// 未使用状態のバレットを見つける
		if (g_Bulletenemy[i].use == FALSE)			
		{
			g_Bulletenemy[i].tex = tex;				// テクスチャーの識別
			g_Bulletenemy[i].use = TRUE;			// 使用状態へ変更する
			g_Bulletenemy[i].pos = enemypos;		// 座標をセット
			return;									// 1発セットしたので終了する
		}
	}
}

