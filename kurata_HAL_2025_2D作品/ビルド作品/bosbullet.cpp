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

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// キャラサイズ(横)
#define TEXTURE_HEIGHT				(100)	// キャラサイズ(横)
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

enum
{
	enemy_bullet00,
};

enum 
{
	enemy,
	boss,

};
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/enemy_bullet00.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static BULLETE	g_Bulletbos[BOS_BULLETE_MAX];	// バレット構造体

// どっちで使うかのチェック変数
int g_check = 0;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBulletBos(void)
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
	for (int i = 0; i < BOS_BULLETE_MAX; i++)
	{
		g_Bulletbos[i].use   = FALSE;								// 未使用（発射されていない弾）
		g_Bulletbos[i].w     = TEXTURE_WIDTH;
		g_Bulletbos[i].h     = TEXTURE_HEIGHT;
		g_Bulletbos[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bulletbos[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bulletbos[i].texNo = 0;
		g_Bulletbos[i].P0 = XMFLOAT3(0.0f, 0.0f, 0.0f);				// 制御点P0
		g_Bulletbos[i].P1 = XMFLOAT3(1084.0f, 300.0f, 0.0f);		// 制御点P0
		g_Bulletbos[i].s = 0.0f;

		g_Bulletbos[i].countAnim = 0;
		g_Bulletbos[i].patternAnim = 0;

		g_Bulletbos[i].move = XMFLOAT3(0.0f, -BULLETE_SPEED, 0.0f);	// 移動量を初期化
		
		g_Bulletbos[i].hit = 0;										// 最初はプレイヤー
		g_Bulletbos[i].tex = 0;										// テクスチャーの識別
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBulletBos(void)
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
void UpdateBulletBos(void)
{
	int bulletCount = 0;				// 処理したバレットの数

	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

	for (int i = 0; i < BOS_BULLETE_MAX; i++)
	{
		if (g_Bulletbos[i].use == TRUE)	// このバレットが使われている？
		{								// Yes
			// アニメーション  
			g_Bulletbos[i].countAnim++;
			if ((g_Bulletbos[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				g_Bulletbos[i].patternAnim = (g_Bulletbos[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// ベジェ曲線関連
			g_Bulletbos[i].s += 0.01f;
			if (g_Bulletbos[i].s > 1.0f)
			{
				g_Bulletbos[i].use = FALSE;
				g_Bulletbos[i].s = 0.0f;
			}

			// バレットの移動処理
			XMVECTOR pos  = XMLoadFloat3(&g_Bulletbos[i].pos);
			XMVECTOR move = XMLoadFloat3(&g_Bulletbos[i].move);
			pos += move;
			XMStoreFloat3(&g_Bulletbos[i].pos, pos);

			// 画面外まで進んだ？
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			if (g_Bulletbos[i].pos.y < (-g_Bulletbos[i].h/2))				// 自分の大きさを考慮して画面外か判定している
			{
				g_Bulletbos[i].use = false;
			}
			if (g_Bulletbos[i].pos.y > (FIELD_HEIGHT + g_Bulletbos[i].h/2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_Bulletbos[i].use = false;
			}


			XMVECTOR vold_pos = XMLoadFloat3(&g_Bulletbos[i].pos);

			XMFLOAT3 vp = XMFLOAT3(0.0f, 604.0f, 0.0f);

			// ボス用とエネミー用で分けている
			if (enemy)
			{
				// ベジェ曲線
				// バレットの移動処理(ポジション)ここをかえるとかわる
				// 座標計算
				XMVECTOR vP0 = XMLoadFloat3(&enemy[2].pos);
				XMVECTOR vP1 = XMLoadFloat3(&enemy[0].pos);
				XMVECTOR vP2 = XMLoadFloat3(&vp);

				//XMFLOAT3 migishita = XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
				//XMVECTOR vP2 = XMLoadFloat3(&migishita);

				float s = g_Bulletbos[i].s;

				XMVECTOR vAns = QuadraticBezierCurve(vP0, vP1, vP2, s);

				XMStoreFloat3(&g_Bulletbos[i].pos, vAns);

				// 弾の回転
				XMVECTOR vec = vAns - vold_pos;		// 移動後と移動前の差分を求めている
				float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);

				// 回転させている
				g_Bulletbos[i].rot.z = angle + (3.14 * 2) / 4;


			}

			if (boss)
			{
				// ベジェ曲線
				// バレットの移動処理(ポジション)ここをかえるとかわる
				// 座標計算
				XMVECTOR vP0 = XMLoadFloat3(&boss[0].boss_pos);
				XMVECTOR vP1 = XMLoadFloat3(&player[0].pos);
				XMVECTOR vP2 = XMLoadFloat3(&vp);


				float s = g_Bulletbos[i].s;

				XMVECTOR vAns = QuadraticBezierCurve(vP0, vP1, vP2, s);

				XMStoreFloat3(&g_Bulletbos[i].pos, vAns);

				// 弾の回転
				XMVECTOR vec = vAns - vold_pos;		// 移動後と移動前の差分を求めている
				float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);

				// 回転させている
				g_Bulletbos[i].rot.z = angle + (3.14 * 2) / 4;


			}

			



			// 当たり判定処理
			// プレイヤーの数分当たり判定を行う
			for (int p = 0; p < ENEMY_MAX; p++)
			{
				// 生きてるエネミーと当たり判定をする
				if (player[p].use == TRUE)
				{
					BOOL ans = CollisionBB(player[p].pos, player[p].w, player[p].h, g_Bulletbos[i].pos, g_Bulletbos[i].w, g_Bulletbos[i].h);

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

								g_Bulletbos[i].use = FALSE;

								// エフェクト発生
								SetEnemyEffect(player[p].pos.x, player[p].pos.y, 30, TEXTURE_EFFECT00);
							}
						}

						// プレイヤーが守り状態なら、FALSEにする
						if (player[p].defense == TRUE)
						{
							g_Bulletbos[i].use = FALSE;
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
void DrawBulletBos(void)
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
	for (int i = 0; i < BOS_BULLETE_MAX; i++)
	{
		if (g_Bulletbos[i].tex == 0)
		{
			if (g_Bulletbos[i].use == TRUE)		// このバレットが使われている？
			{									// Yes
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy_bullet00]);

				//バレットの位置やテクスチャー座標を反映
				float px = g_Bulletbos[i].pos.x - scrollX;	// バレットの表示位置X
				float py = g_Bulletbos[i].pos.y - scrollY;	// バレットの表示位置Y
				float pw = g_Bulletbos[i].w;		// バレットの表示幅
				float ph = g_Bulletbos[i].h;		// バレットの表示高さ

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				float tx = (float)(g_Bulletbos[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
				float ty = (float)(g_Bulletbos[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					g_Bulletbos[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

		}

	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLETE *GetBossBullet(void)
{
	return &g_Bulletbos[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetBulletBos(int check, BOOL tex)
{

	ENEMY* enemy = GetEnemy();

	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BOS_BULLETE_MAX; i++)
	{
		if (g_Bulletbos[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Bulletbos[i].tex = tex;			// バレットの識別
			g_Bulletbos[i].use = TRUE;			// 使用状態へ変更する
			//g_Bulletbos[i].pos = bosspos;		// 座標をセット
			g_check = check;

			return;								// 1発セットしたので終了する

		}
	}
}

//=============================================================================
// ベジェ曲線関連
//=============================================================================

XMVECTOR LinearInterpolation(XMVECTOR P0, XMVECTOR P1, float s)
{
	XMVECTOR A0;
	A0 = P0 + (P1 - P0) * s;
	return A0;
}



XMVECTOR QuadraticBezierCurve(XMVECTOR p0, XMVECTOR p1, XMVECTOR p2, float s)
{
	XMVECTOR a0 = LinearInterpolation(p0, p1, s);
	XMVECTOR a1 = LinearInterpolation(p1, p2, s);
	XMVECTOR b0 = LinearInterpolation(a0, a1, s);

	return b0;
}

