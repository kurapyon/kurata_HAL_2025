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
#include "sound.h"
#include "strikebullet.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************

// ボスのマックス
#define BOSS_ENEMY_MAX				(1)

// テクスチャの数
#define ENEMY_TEXTURE_MAX			(6)		

// エネミー キャラサイズ
#define ENEMY_TEXTURE_WIDTH			(600)	
#define ENEMY_TEXTURE_HEIGHT		(800)	

#define WEPON_TEXTURE_WIDTH			(200)	
#define WEPON_TEXTURE_HEIGHT		(400)	

// ボス用 キャラサイズ
#define BOSS_TEXTURE_WIDTH			(400)	
#define BOSS_TEXTURE_HEIGHT			(700)	

// 被ダメージに色を変えている時間
#define TEXTURE_COLOR_CHANGE_TIME	(200)

// エネミーの体力関係
// Max値
#define ENEMY_MAX_HP				(1000)

// 1回目の必殺技
#define ENEMY_HP_1					(500)
#define ENEMY_HP_2					(400)

// 2回目
#define ENEMY_HP_3					(200)
#define ENEMY_HP_4					(100)


// ボスの体力関係
// モード１(MAX値)
#define BOSS_HP_X1					(5000)

// モード２
#define BOSS_HP_X2					(4000)

// モード3
#define BOSS_HP_X3					(3000)

// モード4
#define BOSS_HP_X4					(2000)

// モード5
#define BOSS_HP_X5					(1000)

//// 強制ゲームオーバー
//#define DEATHBLOW_TIME				(1800)

// アニメーション
// エネミー
#define ENEMY_TEXTURE_PATTERN_DIVIDE_X				(4)																	// アニメパターンのテクスチャ内分割数（X)
#define ENEMY_TEXTURE_PATTERN_DIVIDE_Y				(1)																	// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM							(ENEMY_TEXTURE_PATTERN_DIVIDE_X * ENEMY_TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT									(5)																	// アニメーションの切り替わるWait値


// ウェポン
#define WEPON_TEXTURE_PATTERN_DIVIDE_X				(1)																	// アニメパターンのテクスチャ内分割数（X)
#define WEPON_TEXTURE_PATTERN_DIVIDE_Y				(1)																	// アニメパターンのテクスチャ内分割数（Y)
#define WEPON_ANIM_PATTERN_NUM						(WEPON_TEXTURE_PATTERN_DIVIDE_X * WEPON_TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define WEPON_ANIM_WAIT								(5)																	// アニメーションの切り替わるWait値

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

// エネミーの表現
enum 
{
	enemy,
	wepon01,
	wepon02,
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

// ボスのアニメーション
enum 
{
	boss_state_walk,
	boss_state_atc,
	boss_state_fly,
};

// どっちで使うか判別する
enum 
{
	enemy_check,
	boss_check,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[ENEMY_TEXTURE_MAX] = { NULL };	// テクスチャ情報

// 5
static char *g_TexturName[ENEMY_TEXTURE_MAX] = {
	"data/TEXTURE/Boss00.png",
	"data/TEXTURE/Wepon00.png",
	"data/TEXTURE/Wepon01.png",
	"data/TEXTURE/Boss01.png",
	"data/TEXTURE/Boss02.png",
	"data/TEXTURE/Boss03.png",

};

// 初期化を行ったかのフラグ
static BOOL		g_Load = FALSE;		

// エネミー構造体
static ENEMY	g_Enemy[ENEMY_MAX];		

// ボス構造体
static BOSS		g_Boss[BOSS_MAX];		

// ボスのアニメーション
static BOSS_TEX		 g_texState[3]
{
	//  id,  x, y,MAX, w,      h,    wait  順番通りに書くこと
	{   3,   4, 1, 4, 400.0f, 700.0f, 4},
	{   4,  11, 1,11, 400.0f, 700.0f, 4},
	{   5,   8, 1, 8, 400.0f, 700.0f, 4},

};

// アニメーションパターン
int g_state;

// ランダムで攻撃
static int		g_atc;

// ボス戦に移行する
BOOL			g_round;

// エネミーの数を数える
static int		g_EnemyCnt = ENEMY_MAX;


static INTERPOLATION_DATA g_MoveTbl0[] = {
	//座標									回転率							拡大率				時間
	{ XMFLOAT3(200.0f,   300.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	500 },
	{ XMFLOAT3(1600.0f,  300.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	500 },
};



// 1個
static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,	
};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device *pDevice = GetDevice();


	//テクスチャ生成
	for (int i = 0; i < ENEMY_TEXTURE_MAX; i++)
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
		
	// ランダムで攻撃させる
	g_atc = 0;
	
	// ボス戦に移行する
	g_round = FALSE;

	// エネミー構造体の初期化(ボス、ウェポン)
	g_EnemyCnt = 0;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].use					= TRUE;
		g_Enemy[i].ans					= FALSE;
		g_Enemy[i].pos					= XMFLOAT3(0.0f ,0.0f, 0.0f);				// 中心点から表示
		g_Enemy[i].rot					= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl					= XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].w					= ENEMY_TEXTURE_WIDTH;						// テクスチャーの幅
		g_Enemy[i].h					= ENEMY_TEXTURE_HEIGHT;						// テクスチャーの高さ
		g_Enemy[i].countAnim			= 0;
		g_Enemy[i].patternAnim			= 0;
		g_Enemy[i].dir					= ENEMY_DIR_LEFT;							// エネミーの向き(0:Xマイナス、左		1:Xプラス、右)
		g_Enemy[i].receive				= FALSE;									// 攻撃を受けたかどうかの判定
		g_Enemy[i].id					= 0;										// 管理番号(ID)
		g_Enemy[i].move					= XMFLOAT3(4.0f, 0.0f, 0.0f);				// 移動量

		g_Enemy[i].time					= 0.0f;										// 線形補間用のタイマーをクリア
		g_Enemy[i].tblNo				= 0;										// 再生する行動データテーブルNoをセット
		g_Enemy[i].tblMax				= 0;										// 再生する行動データテーブルのレコード数をセット
		g_Enemy[i].receivewait			= 0;										// 被ダメージ時に色を変える時間

		g_EnemyCnt++;
	}

	g_Enemy[0].life = ENEMY_MAX_HP;							// エネミーのライフ
	g_Enemy[1].life = 0;								// エネミーのライフ
	g_Enemy[2].life = 0;								// エネミーのライフ

	g_Enemy[0].pos = XMFLOAT3(1312.0f, 500.0f, 0.0f);	// 初期座標
	g_Enemy[1].pos = XMFLOAT3(1000.0f, 200.0f, 0.0f);   // 初期座標
	g_Enemy[2].pos = XMFLOAT3(1500.0f, 200.0f, 0.0f);  	// 初期座標

	// ボスのアニメーションの初期化
	g_state = boss_state_walk;


	// ボス構造体の初期化
	for (int i = 0; i < BOSS_MAX; i++)
	{
		
		g_Boss[i].boss_use					= FALSE;
		g_Boss[i].boss_ans					= FALSE;
		g_Boss[i].boss_atcposition			= FALSE;								// 攻撃ポジションについたかどうかの判定
		g_Boss[i].boss_pos					= XMFLOAT3(200.0f, 300.0f, 0.0f);		// 中心点から表示
		g_Boss[i].boss_rot					= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].bosss_cl					= XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Boss[i].boss_w					= BOSS_TEXTURE_WIDTH;					// テクスチャーの幅
		g_Boss[i].boss_h					= BOSS_TEXTURE_HEIGHT;					// テクスチャーの高さ
		g_Boss[i].boss_life					= BOSS_HP_X1;								// エネミーのライフ
		g_Boss[i].boss_countanim			= 0;
		g_Boss[i].boss_patternAnim			= 0;
		g_Boss[i].boss_dir					= ENEMY_DIR_RIGHT;						// エネミーの向き
		g_Boss[i].boss_receive				= FALSE;								// 攻撃を受けたかどうかの判定
		g_Boss[i].boss_id					= 0;									// 管理番号(ID)
		g_Boss[i].boss_move					= XMFLOAT3(4.0f, 0.0f, 0.0f);			// 移動量
		g_Boss[i].boss_wait					= 0;									// ボスウェイト
		g_Boss[i].boss_time					= 0.0f;									// 線形補間用のタイマーをクリア
		g_Boss[i].boss_tblNo				= 0;									// 再生する行動データテーブルNoをセット
		g_Boss[i].boss_tblMax				= 0;									// 再生する行動データテーブルのレコード数をセット
		g_Boss[i].boss_receivewait			= 0;									// 被ダメージ時に色を変える時間
		g_Boss[i].boss_check				= FALSE;								// 必殺技を打つ準備
		g_Boss[i].boss_deathblow			= 0.0f;									// 必殺技を打つ
	}
	

		
	//  0番だけ線形補間で動かしてみる
	g_Boss[0].boss_time		 =0.0f;													// 線形補間用のタイマーをクリア
	g_Boss[0].boss_tblNo	 =0;													// 再生するアニメデータの先頭アドレスをセット
	g_Boss[0].boss_tblMax	 = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < ENEMY_TEXTURE_MAX; i++)
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
void UpdateEnemy(void)
{
	// チュートリアル
	BG* bg = GetBG();

	// poseメニューが表示されたら、動きを止める
	if (bg->pose == FALSE)
	{
		// エネミー戦
		if (g_round == FALSE)
		{

			ENEMY_UPDATE();
		}

		// ボス
		if (g_round == TRUE)
		{


			BOSS_UPDATE();

		}

	}
	
#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
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
	// エネミー用
	if (g_round == FALSE)
	{
		ENEMY_DRAW();
	}

	// ボス戦
	if (g_round == TRUE)
	{
		BOSS_DRAW();
	}
}


//=============================================================================
// エネミー関数とボス関数
//=============================================================================
void ENEMY_UPDATE(void)
{


	g_EnemyCnt = 0;	// 生きてるエネミーの数

		// 1ステージ目(その後ボス戦)
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		// 生きてるエネミーだけ処理をする
		if (g_Enemy[i].use == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Enemy[i].pos;

			// アニメーション  
			g_Enemy[i].countAnim += 1.0f;
			if (g_Enemy[i].countAnim > ANIM_WAIT)
			{
				g_Enemy[i].countAnim = 0.0f;
				// パターンの切り替え
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}


			// ボスの向きの調整
			PLAYER* player = GetPlayer();

			// ボスがプレイヤーのX座標より右にいたら,左に向かせる
			if (player[0].pos.x > g_Enemy[i].pos.x)
			{
				g_Enemy[i].dir = ENEMY_DIR_RIGHT;
			}

			// もしボスがプレイヤーのX座標より左にいたら、右に向かせる
			if (player[0].pos.x < g_Enemy[i].pos.x)
			{
				g_Enemy[i].dir = ENEMY_DIR_LEFT;
			}

				// 常時攻撃させる
				g_Enemy[i].wait++;
				if (g_Enemy[i].wait >= ENEMY_WAIT)
				{

						// 上下８方向に攻撃
						XMFLOAT3 enemypos = g_Enemy[1].pos;
						enemypos.y += enemypos.y / 2;

						SetStrikeBullet(enemypos, enemy_bullet00);

				}

				// 一定の体力になったら攻撃させる
				{
					// 1回目
					if (ENEMY_HP_1 >= g_Enemy[0].life && g_Enemy[0].life >= ENEMY_HP_2)
					{
						//// ベジェ曲線
						XMFLOAT3 bosspos = g_Enemy[2].pos;
						SetBulletBos(enemy_check, 0);
					}

					// 2回目
					if (ENEMY_HP_3 >= g_Enemy[0].life && g_Enemy[0].life >= ENEMY_HP_4)
					{
						//// ベジェ曲線
						XMFLOAT3 bosspos = g_Enemy[2].pos;
						SetBulletBos(enemy_check, 0);
					}
				}
		}



		PLAYER* player = GetPlayer();

		// エネミーが倒れたら、次に行く
		if (g_Enemy[i].life <= 0)
		{
			g_EnemyCnt++;	// 死んだ数
		}

		// エネミーを倒したら、ボスモードに行く
		if (g_EnemyCnt == ENEMY_MAX)
		{
			// エフェクト発生
			SetEnemyEffect(g_Enemy[0].pos.x, g_Enemy[0].pos.y, 30, TEXTURE_EFFECT);

			// ボスをTRUEにして、ボスを出現させる
			g_Boss[0].boss_use = TRUE;
			g_round = TRUE;

			// 曲を止める
			StopSound(SOUND_LABEL_BGM_boss01);
			// 曲を流す
			PlaySound(SOUND_LABEL_BGM_enemy00);


		}
	}


}


void BOSS_UPDATE(void)
{

	for (int i = 0; i < BOSS_ENEMY_MAX; i++)
	{
		// 生きてるエネミーだけ処理をする
		if (g_Boss[i].boss_use == TRUE)
		{

			
			PLAYER* player = GetPlayer();


			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 boss_pos_old = g_Boss[i].boss_pos;

			// アニメーション  
			g_Boss[i].boss_countanim += 1.0f;
			if (g_Boss[i].boss_countanim > ANIM_WAIT)
			{

				g_Boss[i].boss_countanim = 0.0f;

				// アニメーションパターンが終わるタイミング
				if ((g_Boss[i].boss_patternAnim + 1) >= g_texState[g_state].animemax)
				{
					// 攻撃モーションか飛行攻撃だったら通常用のアニメーションにする
					if (g_state == boss_state_atc || g_state == boss_state_fly)
					{
						g_state = boss_state_walk;
					}
				}


				// パターンの切り替え
				g_Boss[i].boss_patternAnim = (g_Boss[i].boss_patternAnim + 1) % g_texState[g_state].animemax;
			}



			// ボスの向きの調整

			// ボスがプレイヤーのX座標より右にいたら,左に向かせる
			if (player[0].pos.x > g_Boss[i].boss_pos.x)
			{
				g_Boss[i].boss_dir = ENEMY_DIR_LEFT;
			}

			// もしボスがプレイヤーのX座標より左にいたら、右に向かせる
			if (player[0].pos.x < g_Boss[i].boss_pos.x)
			{
				g_Boss[i].boss_dir = ENEMY_DIR_RIGHT;
			}



			// ボスの攻撃処理（ライフの残りによって、攻撃の種類を増やして強くしていく)
			g_Boss[0].boss_wait += 1;


			// ホーミング攻撃と移動のみ
			if ( (BOSS_HP_X1 >= g_Boss[i].boss_life) && (g_Boss[i].boss_life > BOSS_HP_X2) )
			{

				XMFLOAT3 bosspos = g_Boss[0].boss_pos;
				bosspos.y -= g_Boss[0].boss_pos.y / 2;

				// 常時攻撃させる(8方向攻撃)
				SetStrikeBullet(bosspos, enemy_bullet01);

			}

			// 必殺技を出す(ベジェ曲線)
			if ( (BOSS_HP_X2 >= g_Boss[i].boss_life) && (g_Boss[i].boss_life > BOSS_HP_X3) )
			{
				// 移動させない
				g_Boss[i].boss_atcposition = TRUE;

				// アニメションを変える
				g_state = boss_state_atc;

				PlaySound(SOUND_LABEL_bossatc);

				XMFLOAT3 bosspos = g_Boss[0].boss_pos;
				bosspos.y -= g_Boss[0].boss_pos.y / 2;

				// 常時攻撃させる(8方向攻撃)
				SetStrikeBullet(bosspos, enemy_bullet01);

				// 必殺技(ベジェ曲線)
				SetBulletBos(boss_check, 0);

			}

			// 八方向攻撃の調整

			// ホーミング攻撃と上からの攻撃のみ
			if ( (BOSS_HP_X3 > g_Boss[i].boss_life) && (g_Boss[i].boss_life > BOSS_HP_X4) )
			{
				// 移動させない
				g_Boss[i].boss_atcposition = TRUE;

				XMFLOAT3 bosspos = g_Boss[0].boss_pos;
				bosspos.y -= g_Boss[0].boss_pos.y / 2;

				// 常時攻撃させる(8方向攻撃)
				SetStrikeBullet(bosspos, enemy_bullet01);


				// ボスウェイトの時間より多くなったら攻撃させる(上からの攻撃)
				if (g_Boss[0].boss_wait >= BOSS_WAIT)
				{
					g_state = boss_state_fly;
					SetBulletAtc(enemy_bullet01);
					g_Boss[0].boss_wait = 0;

				}

			}

			// ホーミング攻撃と上からの攻撃と移動攻撃
			if ( (BOSS_HP_X4 > g_Boss[i].boss_life) && (g_Boss[i].boss_life > 0) )
			{

				// アニメションを変える
				g_state = boss_state_atc;

				// 移動させる
				g_Boss[i].boss_atcposition = FALSE;

				// 常時攻撃させる(8方向攻撃)
				XMFLOAT3 bosspos = g_Boss[0].boss_pos;
				bosspos.y -= g_Boss[0].boss_pos.y / 2;

				SetStrikeBullet(bosspos, enemy_bullet01);


				// 上からの攻撃
				g_state = boss_state_fly;
				SetBulletAtc(enemy_bullet01);
				g_Boss[0].boss_wait = 0;

			}


			if (g_Boss[i].boss_atcposition == FALSE)
			{
				// 線形補完 移動処理
				if (g_Boss[i].boss_tblMax > 0)								// 線形補間を実行する？
				{	// 線形補間の処理
					int nowNo = (int)g_Boss[i].boss_time;					// 整数分であるテーブル番号を取り出している
					int maxNo = g_Boss[i].boss_tblMax;						// 登録テーブル数を数えている
					int nextNo = (nowNo + 1) % maxNo;						// 移動先テーブルの番号を求めている
					INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Boss[i].boss_tblNo];	// 行動テーブルのアドレスを取得

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);		// XMVECTORへ変換
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);		// XMVECTORへ変換
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);		// XMVECTORへ変換

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

					float nowTime = g_Boss[i].boss_time - nowNo;			// 時間部分である少数を取り出している

					Pos *= nowTime;											// 現在の移動量を計算している
					Rot *= nowTime;											// 現在の回転量を計算している
					Scl *= nowTime;											// 現在の拡大率を計算している

					// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
					XMStoreFloat3(&g_Boss[i].boss_pos, nowPos + Pos);

					// 計算して求めた回転量を現在の移動テーブルに足している
					XMStoreFloat3(&g_Boss[i].boss_rot, nowRot + Rot);

					// 計算して求めた拡大率を現在の移動テーブルに足している
					XMStoreFloat3(&g_Boss[i].bosss_cl, nowScl + Scl);
					g_Boss[i].boss_w = ENEMY_TEXTURE_WIDTH * g_Boss[i].bosss_cl.x;
					g_Boss[i].boss_h = ENEMY_TEXTURE_HEIGHT * g_Boss[i].bosss_cl.y;

					// frameを使て時間経過処理をする
					g_Boss[i].boss_time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
					if ((int)g_Boss[i].boss_time >= maxNo)			// 登録テーブル最後まで移動したか？
					{
						g_Boss[i].boss_time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
					}

				}

			}
		}
	}

	// ボスを倒したらクリア画面に行く
	if (g_Boss[0].boss_life <= 0)
	{
		// エフェクト発生
		SetEnemyEffect(g_Boss[0].boss_pos.x, g_Boss[0].boss_pos.y, 30, TEXTURE_EFFECT);

		SetFadeMode(FADE_OUT, MODE_CLEAR_RESULT);
	}

}

void ENEMY_DRAW(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)
		{
			// エネミー
			float scrollX = GetScrollX();
			float scrollY = GetScrollY();

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_Enemy[enemy].pos.x - scrollX;	// エネミーの表示位置X
			float py = g_Enemy[enemy].pos.y - scrollY;	// エネミーの表示位置Y
			float pw = g_Enemy[enemy].w;				// エネミーの表示幅
			float ph = g_Enemy[enemy].h;				// エネミーの表示高さ

			 //アニメーション用
			float tw = 1.0f / ENEMY_TEXTURE_PATTERN_DIVIDE_X;									// テクスチャの幅
			float th = 1.0f / ENEMY_TEXTURE_PATTERN_DIVIDE_Y;									// テクスチャの高さ
			float tx = (float)(g_Enemy[enemy].patternAnim % ENEMY_TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Enemy[enemy].patternAnim / ENEMY_TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

				// もしプレイヤーの左にいたら反転させる
			if (g_Enemy[enemy].dir == ENEMY_DIR_LEFT)
			{
				// -1をかけると反転
				tw *= -1;

				// 表示位置をずれないようにしている
				tx += 1.0f / g_texState[g_state].animex;

			}

			// 攻撃を受けたら色を変える
			
			// 通常時
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[0].rot.z);


			g_Enemy[enemy].receivewait++;

			// もし、攻撃を受けたら赤色にする
			if (g_Enemy[enemy].receive == TRUE)
			{
				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
					g_Enemy[0].rot.z);
				
				// もしカラーチェンジタイムを超えたら色を戻す
				if (g_Enemy[enemy].receivewait >= TEXTURE_COLOR_CHANGE_TIME)
				{
					// 元の色に戻す
					g_Enemy[enemy].receive = FALSE;
					g_Enemy[enemy].receivewait += 0;

				}
			}

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);



			// ウェポン01
			
			 scrollX = GetScrollX();
			 scrollY = GetScrollY();

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[wepon01]);

			g_Enemy[wepon01].w = WEPON_TEXTURE_WIDTH;
			g_Enemy[wepon01].h = WEPON_TEXTURE_HEIGHT;

			//エネミーの位置やテクスチャー座標を反映
			 px = g_Enemy[wepon01].pos.x - scrollX;	// エネミーの表示位置X
			 py = g_Enemy[wepon01].pos.y - scrollY;	// エネミーの表示位置Y
			 pw = g_Enemy[wepon01].w;				// エネミーの表示幅
			 ph = g_Enemy[wepon01].h;				// エネミーの表示高さ

			 //アニメーション用
			 tw = 1.0f / WEPON_TEXTURE_PATTERN_DIVIDE_X;										// テクスチャの幅
			 th = 1.0f / WEPON_TEXTURE_PATTERN_DIVIDE_Y;										// テクスチャの高さ
			 tx = (float)(g_Enemy[wepon01].patternAnim % WEPON_TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			 ty = (float)(g_Enemy[wepon01].patternAnim / WEPON_TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[1].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);



			// ウェポン02
						
			// エネミー
			 scrollX = GetScrollX();
			 scrollY = GetScrollY();

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[wepon02]);

			g_Enemy[wepon02].w = WEPON_TEXTURE_WIDTH;
			g_Enemy[wepon02].h = WEPON_TEXTURE_HEIGHT;

			//エネミーの位置やテクスチャー座標を反映
			 px = g_Enemy[wepon02].pos.x - scrollX;	// エネミーの表示位置X
			 py = g_Enemy[wepon02].pos.y - scrollY;	// エネミーの表示位置Y
			 pw = g_Enemy[wepon02].w;				// エネミーの表示幅
			 ph = g_Enemy[wepon02].h;				// エネミーの表示高さ

			 //アニメーション用
			 tw = 1.0f / WEPON_TEXTURE_PATTERN_DIVIDE_X;									// テクスチャの幅
			 th = 1.0f / WEPON_TEXTURE_PATTERN_DIVIDE_Y;									// テクスチャの高さ
			 tx = (float)(g_Enemy[wepon02].patternAnim % WEPON_TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			 ty = (float)(g_Enemy[wepon02].patternAnim / WEPON_TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[wepon02].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);


		}
	}

}

void BOSS_DRAW(void)
{
	if (g_Boss[0].boss_use == TRUE)
	{

		float scrollX = GetScrollX();
		float scrollY = GetScrollY();

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_texState[g_state].id]);

		//エネミーの位置やテクスチャー座標を反映
		float px = g_Boss[0].boss_pos.x - scrollX;		// エネミーの表示位置X
		float py = g_Boss[0].boss_pos.y - scrollY;		// エネミーの表示位置Y
		float pw = g_Boss[0].boss_w;					// エネミーの表示幅
		float ph = g_Boss[0].boss_h;					// エネミーの表示高さ

		 //アニメーション用
		float tw = 1.0f / g_texState[g_state].animex;									// テクスチャの幅
		float th = 1.0f / g_texState[g_state].animey;										// テクスチャの高さ
		float tx = (float)(g_Boss[0].boss_patternAnim % g_texState[g_state].animex) * tw;	// テクスチャの左上X座標
		float ty = (float)(g_Boss[0].boss_patternAnim / g_texState[g_state].animex) * th;	// テクスチャの左上Y座標

		// もしプレイヤーの左にいたら反転させる
		if (g_Boss[0].boss_dir == ENEMY_DIR_LEFT)
		{
			// -1をかけると反転
			tw *= -1;

			// 表示位置をずれないようにしている
			tx += 1.0f / g_texState[g_state].animex;

		}

		// 攻撃を受けたら色を変える
		 
		// 通常時
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			g_Boss[0].boss_rot.z);

		// 攻撃を受けたら赤色にする
		
		g_Boss[0].boss_receivewait++;

		// もし攻撃を受けたら色を赤に変える
		if (g_Boss[0].boss_receive == TRUE)
		{
			// 赤
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
				g_Boss[0].boss_rot.z);

			// もしカラーチェンジタイム以上だったら元の色に戻す
			if (g_Boss[0].boss_receivewait >= TEXTURE_COLOR_CHANGE_TIME)
			{
				// 元の色に戻す
				g_Boss[0].boss_receive = FALSE;
				g_Boss[0].boss_receivewait += 0;
			}
		}

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


	}
}






//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}


BOSS* GetBoss(void)
{
	return &g_Boss[0];
}

