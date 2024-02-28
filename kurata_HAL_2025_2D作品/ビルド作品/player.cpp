//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "player.h"
#include "input.h"
#include "bg.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "Level.h"
#include "file.h"
#include "map.h"
#include "fade.h"
#include "cut.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(150/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(150/2)	// キャラサイズ
#define TEXTURE_MAX					(10)	// テクスチャの数
#define PLAYER_TEXTURE_MAX			(4)		// プレイヤーのテクスチャーの数

#define TEXTURE_PATTERN_DIVIDE_X	(4)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X * TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

// プレイヤーの画面内配置座標
#define PLAYER_DISP_X				(SCREEN_WIDTH / 2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT / 2 + TEXTURE_HEIGHT)

// ジャンプ処理
#define	PLAYER_JUMP_CNT_MAX			(40)		// 40フレームで着地する
#define	PLAYER_JUMP_Y_MAX			(300.0f)	// ジャンプの高さ

// プレイヤーのHP
#define MAX_HP						(5000)

// プレイヤーのスタミナ
#define MAX_ENERGY					(50)

// 重力
#define PLAYER_GRAVITY				(8.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//void DrawPlayerOffset(int no);
enum
{
	// 主人公
	TEXTURE_WALK,
	TEXTURE_CUT,
	TEXTURE_JUMP,
	TEXTURE_DEFENSE,

	//	影
	TEXTURE_SHADOW000,

	// ライフ
	TEXTURE_HEART_MAIN10,
	TEXTURE_HEARTBASE10,

	// スタミナゲージ
	TEXTURE_ENERGY01,
	TEXTURE_ENERGY,
	TEXTURE_BRACh,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/shujinnkou_walk.png",		// プレイヤー(歩き)
	"data/TEXTURE/shujinnkou_cut.png",		// プレイヤー(攻撃)
	"data/TEXTURE/shujinnkou_jump.png",		// プレイヤー(ジャンプ)
	"data/TEXTURE/shujinnkou_defense.png",	// プレイヤー(守り)

	"data/TEXTURE/shadow.jpg",				// 丸影

	"data/TEXTURE/heart_main10.png",		// HPバー
	"data/TEXTURE/heartbase10.png",			// HPバー(外枠)

	"data/TEXTURE/energy01.png",			// スタミナゲージの枠組み
	"data/TEXTURE/energy.png",				// スタミナゲージ
	"data/TEXTURE/brach.png",				// スタミナゲージ
};


static BOOL		g_Load = FALSE;				// 初期化を行ったかのフラグ
static PLAYER	g_Player[PLAYER_MAX];		// プレイヤー構造体
int				g_PlayerCnt;				// 死んだプレイヤーを数える
static float	g_jumppowr = 0;				// 浮力

// プレイヤーのアニメーション
static TEX_PLAYER		 g_PlayerTexState[PLAYER_TEXTURE_MAX]
{
	//  no,  x, y, MAX,   w,     h,    wait		順番通りに書くこと

	{   0,   3, 1,  3, 100.0f, 100.0f,	4},// TEXTURE_WALK
	{   1,   4, 1,  4, 100.0f, 100.0f,	4},// TEXTURE_CUT
	{   2,   1, 1,  1, 100.0f, 100.0f,	4},// TEXTURE_JUMP
	{   3,   5, 2, 10, 100.0f, 100.0f,	4},// TEXTURE_DEFENSE
};

// アニメーションパターン
int g_playerstate;

// HP・スタミナバーの描画調整
float pw_log = 0.0f;
float tw_log = 0.0f;

// poseメニューの切り替え


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
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

	g_PlayerCnt = 0;		// 死んだプレイヤーを数える
	
	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].use = TRUE;
		g_Player[i].pos = XMFLOAT3(150.0f, 500.0f, 0.0f);	// 中心点から表示
		g_Player[i].pos_old = g_Player[i].pos;				// プレイヤーの座標の保存

		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;

		g_Player[i].move = XMFLOAT3(4.0f, PLAYER_ACCEL_Y_DOWN, 0.0f);		// 移動量

		g_Player[i].dir = PLAYER_DIR_DOWN;					// 下向きにしとくか
		g_Player[i].moving = FALSE;							// 移動中フラグ
		g_Player[i].pos_old = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 座標の保存

		// 守りフラグ
		g_Player[i].defense = FALSE;

		// 攻撃を受けたかどうかの判定
		g_Player[i].receive = FALSE;

		// チェンジタイムの初期化
		g_Player[i].receivewait = 0;

		// ジャンプの初期化
		g_Player[i].jump = FALSE;
		g_Player[i].jumpCnt = 0;
		g_Player[i].jumpY = 0.0f;
		g_Player[i].jumpYMax = PLAYER_JUMP_Y_MAX;

		// プレイヤーの無敵時間
		g_Player[i].invincibleCnt= 0;

		// ダメージを受けたら無敵状態になる
		g_Player[i].isDamaged = FALSE;

		// プレイヤーライフ
		g_Player[i].life = MAX_HP;

		// プレイヤーのスタミナ
		g_Player[i].energy = MAX_ENERGY;

		// プレイヤーのレベルアップ
		g_Player[i].levelup = FALSE;

		// 守り状態になってから何秒経過したか数える
		g_Player[i].defense_time = 0.0f;
	}

	// アニメーションパターン
	g_playerstate = TEXTURE_WALK;

	// ポーズメニューの切り替え

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{
	BG* bg = GetBG();



	
		PlayerUpdate();
	


	// 現状をセーブする
	if (GetKeyboardTrigger(DIK_S))
	{
		SaveData();
	}


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{

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

	float scrollX = GetScrollX();
	float scrollY = GetScrollY();


	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// このプレイヤーが使われている？
		{									// Yes

			{	// 影表示
				SetBlendState(BLEND_MODE_SUBTRACT);	// 減算合成

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SHADOW000]);

				float px = g_Player[i].pos.x - scrollX;	// プレイヤーの表示位置X
				float py = g_Player[i].pos.y - scrollY;	// プレイヤーの表示位置Y
				float pw = g_Player[i].w;				// プレイヤーの表示幅
				float ph = g_Player[i].h/4;				// プレイヤーの表示高さ
				py += 50.0f;							// 足元に表示

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				SetBlendState(BLEND_MODE_ALPHABLEND);	// 半透明処理を元に戻す

			}

			// プレイヤー本人の描画
			
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_PlayerTexState[g_playerstate].id]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_Player[i].pos.x - scrollX;						// プレイヤーの表示位置X
			float py = g_Player[i].pos.y - scrollY;						// プレイヤーの表示位置Y
			float pw = g_PlayerTexState[g_playerstate].w;				// プレイヤーの表示幅
			float ph = g_PlayerTexState[g_playerstate].h;				// プレイヤーの表示高さ

			py += g_Player[i].jumpY;									// ジャンプ中の高さを足す

			// アニメーション用
			float tw = 1.0f / g_PlayerTexState[g_playerstate].animex;										// テクスチャの幅
			float th = 1.0f / g_PlayerTexState[g_playerstate].animey;										// テクスチャの高さ
			float tx = (float)(g_Player[i].patternAnim % g_PlayerTexState[g_playerstate].animex) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Player[i].patternAnim / g_PlayerTexState[g_playerstate].animex) * th;	// テクスチャの左上Y座標

			// 反転処理
			if (g_Player[i].dir == PLAYER_DIR_LEFT)
			{
				// -1をかけると反転
				tw *= -1;

				// 表示位置をずれないようにしている
				tx += 1.0f / g_PlayerTexState[g_playerstate].animex;

			}

			// 攻撃を受けたら色を変える
			 
			// 通常時
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Player[i].rot.z);

			g_Player[i].receivewait++;

			// もし攻撃を受けたら
			if (g_Player[i].receive == TRUE)
			{
					// 青色に変える
					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
						g_Player[i].rot.z);

					// 時間過ぎたら元の色に戻す
					if (g_Player[i].receivewait >= PLAYER_COLOR_CHANGE_TIME)
					{
						g_Player[i].receive = FALSE;
						g_Player[i].receivewait = 0;

					}

			}

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);


			// HPゲージ
			SetHpGauge();


			// スタミナゲージ
			SetStaminaGauge();

		}
	}


}


//=============================================================================
// Player構造体の先頭アドレスを取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}

// レベルアップ
void SetLevelUP(void) {

	g_Player[0].levelup = TRUE;

}

// ダメージ
void SetDamage(int damage) {

	g_Player[0].life -= damage;

}

// PlayerUpdate関数
void PlayerUpdate(void)
{

	BG* bg = GetBG();
	LEVEL* level = GetLevel();

	for (int i = 0; i < PLAYER_MAX; i++)
	{

		// 生きてるプレイヤーだけ処理をする
		if (g_Player[i].use == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			g_Player[0].pos_old = g_Player[0].pos;


			// アニメーション  
			if (!(g_Player[i].moving == FALSE && g_playerstate == TEXTURE_WALK))
			{
				g_Player[i].countAnim += 1.0f;
				if (g_Player[i].countAnim > ANIM_WAIT)
				{
					g_Player[i].countAnim = 0.0f;

					// アニメションが終わるタイミング
					if ((g_Player[i].patternAnim + 1) >= g_PlayerTexState[g_playerstate].animemax)
					{
						// 攻撃モーションかジャンプだったら
						if (g_playerstate == TEXTURE_CUT || g_playerstate == TEXTURE_JUMP)
						{
							g_playerstate = TEXTURE_WALK;
						}
					}
					//// パターンの切り替え
					g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % g_PlayerTexState[g_playerstate].animemax;

				}
			}


			// プレイヤー操作

			// プレイヤーのスピード
			float speed = g_Player[i].move.x;

			g_Player[i].moving = FALSE;

			// ダッシュ処理
			if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, BUTTON_R))
			{
				speed *= 2;
			}

			// ジャンプ処理
			// ジャンプ中
			if (g_Player[i].defense == FALSE)
			{
				if (g_Player[i].jump == TRUE)
				{

					g_Player[i].pos.y -= g_jumppowr;

					if (g_jumppowr >= 2.0f)
					{
						g_jumppowr -= 1.0f;
					}
					else
					{
						g_jumppowr = 0.0f;
					}
				}
				else if ((g_Player[i].jump == FALSE) && (GetKeyboardTrigger(DIK_F) || IsButtonTriggered(0, BUTTON_A)))
				{
					// ジャンプしていないとき(まだ地面にいる)
					g_playerstate = TEXTURE_JUMP;

					g_Player[i].jump = TRUE;
					g_jumppowr = 28.0f;
				}


			}


			// 左右移動
			// 守織状態ではないときのみ、歩くことができる
			if (g_Player[i].defense == FALSE)
			{
				// キーボード
				if (GetKeyboardPress(DIK_RIGHT))
				{
					// 歩くアニメーションに戻す
					g_playerstate = TEXTURE_WALK;

					g_Player[i].pos.x += speed;
					g_Player[i].dir = PLAYER_DIR_RIGHT;
					g_Player[i].moving = TRUE;

				}
				else if (GetKeyboardPress(DIK_LEFT))
				{
					// 歩くアニメーションに戻す
					g_playerstate = TEXTURE_WALK;

					g_Player[i].pos.x -= speed;
					g_Player[i].dir = PLAYER_DIR_LEFT;
					g_Player[i].moving = TRUE;

				}

				//(ゲームパッド)
				if (IsButtonPressed(0, BUTTON_RIGHT))
				{
					// 歩くアニメーションに戻す
					g_playerstate = TEXTURE_WALK;

					g_Player[i].pos.x += speed;
					g_Player[i].dir = PLAYER_DIR_RIGHT;
					g_Player[i].moving = TRUE;
				}
				else if (IsButtonPressed(0, BUTTON_LEFT))
				{
					// 歩くアニメーションに戻す
					g_playerstate = TEXTURE_WALK;

					g_Player[i].pos.x -= speed;
					g_Player[i].dir = PLAYER_DIR_LEFT;
					g_Player[i].moving = TRUE;
				}

			}



			// 近距離攻撃 (キー入力，ゲームパッド)
			// 守り状態ではないときのみ、攻撃できる
			if (g_Player[i].defense == FALSE)
			{
				if (GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_B))
				{
					g_playerstate = TEXTURE_CUT;

					XMFLOAT3 pos = g_Player[i].pos;
					pos.y += g_Player[i].jumpY;

					SetCutting(pos);

				}
			}

			// 遠距離攻撃
			// 守り状態ではないときのみ、攻撃できる
			if (g_Player[i].defense == FALSE)
			{
				if (level[i].level >= 5)
				{
					if (g_Player[i].energy >= 0)
					{
						if (GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_L))
						{
							XMFLOAT3 bullet_pos = g_Player[i].pos;
							bullet_pos.y += g_Player[i].jumpY;

							SetBullet(bullet_pos);

							g_Player[i].energy -= 10.0f;

						}
					}
				}
			}

			// 守り処理(もし守り状態なら攻撃を受けない。ただし、エネルギーは消費する)
			if (g_Player[i].energy >= 0)
			{
				g_Player[i].defense_time++;

				// Aキーもしくは、R2ボタンを押したときに守り状態になる
				if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_R2))
				{
					g_playerstate = TEXTURE_DEFENSE;

					g_Player[i].defense = TRUE;

					g_Player[i].energy -= 0.1f;
				}

				// Aキーもしくは、R2ボタンを離したら守り状態を解除する
				if (GetKeyboardRelease(DIK_A) || IsButtonRelease(0, BUTTON_R2) || g_Player[i].energy <= 0)
				{
					g_Player[i].defense = FALSE;

					g_playerstate = TEXTURE_WALK;

				}
			}


			// もし、エネルギーがマックスエネルギーより少なかったら
			if (g_Player[i].energy < MAX_ENERGY)
			{
				// 値の分ずつ増やす
				g_Player[i].energy += 0.1f;

			}



			// 当たり判定
			// 上辺
			{
				float y = g_Player[i].pos.y - g_Player[i].h / 2;

				float sx = g_Player[i].pos.x - g_Player[i].w / 2 + 10.0f;
				XMFLOAT3 spos = XMFLOAT3(sx, y, 0.0f);

				float ex = g_Player[i].pos.x + g_Player[i].w / 2 - 10.0f;
				XMFLOAT3 epos = XMFLOAT3(ex, y, 0.0f);

				if (CollisionEdgeAndMapChip(spos, epos, UPPER))
				{
					g_Player[i].pos.y = g_Player[i].pos_old.y;
					//g_Player[i].move.y = 0.0f;
					g_Player[i].jump = FALSE;

				}
				else
				{
					//g_Player[i].move.y = PLAYER_ACCEL_Y_DOWN;
					g_Player[i].jump = TRUE;

				}
			}
			//下辺
			{
				float y = g_Player[i].pos.y + g_Player[i].h / 2;

				float sx = g_Player[i].pos.x - g_Player[i].w / 2 + 10.0f;
				XMFLOAT3 spos = XMFLOAT3(sx, y, 0.0f);

				float ex = g_Player[i].pos.x + g_Player[i].w / 2 - 10.0f;
				XMFLOAT3 epos = XMFLOAT3(ex, y, 0.0f);

				if (CollisionEdgeAndMapChip(spos, epos, LOWER))
				{
					if (g_Player[i].pos.y > g_Player[i].pos_old.y)
					{
						g_Player[i].pos.y = g_Player[i].pos_old.y;
					}

					// ジャンプを実行したフレームでジャンプフラグをオフにしないため
					// ジャンプ力を変えたかったらグラビティとジャンプパワーを変える
					if (g_jumppowr <= 0.0f)
					{
						g_Player[i].jump = FALSE;
					}

				}
				else
				{
					g_Player[i].pos.y += PLAYER_GRAVITY;
				}
			}
			//左辺
			{
				float x = g_Player[i].pos.x - g_Player[i].w / 2;

				float sy = g_Player[i].pos.y - g_Player[i].h / 2 + 5.0f;
				XMFLOAT3 spos = XMFLOAT3(x, sy, 0.0f);

				float ey = g_Player[i].pos.y + g_Player[i].h / 2 - 10.0f;
				XMFLOAT3 epos = XMFLOAT3(x, ey, 0.0f);

				if (CollisionEdgeAndMapChip(spos, epos, LEFT))
				{
					g_Player[i].pos.x = g_Player[i].pos_old.x;
				}
			}
			//右辺
			{
				float x = g_Player[i].pos.x + g_Player[i].w / 2;

				float sy = g_Player[i].pos.y - g_Player[i].h / 2 + 5.0f;
				XMFLOAT3 spos = XMFLOAT3(x, sy, 0.0f);

				float ey = g_Player[i].pos.y + g_Player[i].h / 2 - 10.0f;
				XMFLOAT3 epos = XMFLOAT3(x, ey, 0.0f);

				if (CollisionEdgeAndMapChip(spos, epos, RIGHT))
				{
					g_Player[i].pos.x = g_Player[i].pos_old.x;

				}
			}




			// MAP外チェック

			if (g_Player[i].pos.x < 0.0f)
			{
				g_Player[i].pos.x = 0.0f;
			}

			if (g_Player[i].pos.x > FIELD_WIDTH)
			{
				g_Player[i].pos.x = FIELD_WIDTH;
			}

			if (g_Player[i].pos.y < 0.0f)
			{
				g_Player[i].pos.y = 0.0f;
			}

			if (g_Player[i].pos.y > FIELD_HEIGHT)
			{
				g_Player[i].pos.y = FIELD_HEIGHT;
			}

			// プレイヤーの立ち位置からMAPのスクロール座標を計算する
			bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
			if (bg->pos.x < 0) bg->pos.x = 0;
			if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

			bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
			if (bg->pos.y < 0) bg->pos.y = 0;
			if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;



			LEVEL* level = GetLevel();

		}

		// ダメージを受けたら無敵状態
		if (g_Player[i].isDamaged == TRUE)
		{
			g_Player[i].invincibleCnt++;
		}

		// 無敵時間の初期化
		if (g_Player[i].invincibleCnt >= PLAYER_INVINCIBLE_FRAME)
		{
			g_Player[i].invincibleCnt = 0;
			g_Player[i].isDamaged = FALSE;
		}

		// もしプレイヤーの体力が0ならゲームオーバー
		if (g_Player[i].life <= 0)
		{
			g_PlayerCnt++;
		}

		// プレイヤーが全員死んだら
		if (g_PlayerCnt == PLAYER_MAX)
		{
			g_Player[i].use = FALSE;
			SetFadeMode(FADE_OUT, MODE_GAMEOVER_RESULT);
		}

	}


}



// HPゲージ
void SetHpGauge(void)
{

		// 下敷きのゲージ（枠的な物）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_HEARTBASE10]);

		//ゲージの位置やテクスチャー座標を反映
		float px = 10.0f;	// ゲージの表示位置X
		float py = 10.0f;	// ゲージの表示位置Y
		float pw = 300.0f;// ゲージの表示幅
		float ph = 30.0f;	// ゲージの表示高さ
			  
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
			  
	   // １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// ライフ
		//ゲージの位置やテクスチャー座標を反映
		 px = 10.0f;	// ゲージの表示位置X
		 py = 10.0f;	// ゲージの表示位置Y
		 pw = 300.0f;	// ゲージの表示幅
		 ph = 30.0f;	// ゲージの表示高さ

		 tw = 1.0f;		// テクスチャの幅
		 th = 1.0f;		// テクスチャの高さ
		 tx = 0.0f;		// テクスチャの左上X座標
		 ty = 0.0f;		// テクスチャの左上Y座標



		pw = pw * (float)g_Player[0].life / MAX_HP;
		tw = tw * (float)g_Player[0].life / MAX_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_HEART_MAIN10]);


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
}

// スタミナゲージ
void SetStaminaGauge(void)
{
	{


		//ゲージの位置やテクスチャー座標を反映
		float px = 20.0f;					// ゲージの表示位置X
		float py = 60.0f;					// ゲージの表示位置Y
		float pw = 265.0;					// ゲージの表示幅
		float ph = 14.0f;					// ゲージの表示高さ

		float tw = 1.0f;					// テクスチャの幅
		float th = 1.0f;					// テクスチャの高さ
		float tx = 0.0f;					// テクスチャの左上X座標
		float ty = 0.0f;					// テクスチャの左上Y座標

		pw = pw * (float)g_Player[0].energy / MAX_ENERGY;
		tw = tw * (float)g_Player[0].energy / MAX_ENERGY;

		//pw_log = (float)(g_Player[0].energy / MAX_ENERGY);
		//tw_log = tw;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_ENERGY]);


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// 黒い背景
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BRACh]);


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);



		// 枠組み
		//ゲージの位置やテクスチャー座標を反映
		px = 10.0f;				// ゲージの表示位置X
		py = 60.0f;				// ゲージの表示位置Y
		pw = 290.0;				// ゲージの表示幅
		ph = 18.0f;				// ゲージの表示高さ

		tw = 1.0f;				// テクスチャの幅
		th = 1.0f;				// テクスチャの高さ
		tx = 0.0f;				// テクスチャの左上X座標
		ty = 0.0f;				// テクスチャの左上Y座標



	   // テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_ENERGY01]);


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);



	}

}