//=============================================================================
//
// エフェクト処理 [effect.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "playereffect.h"
#include "player.h"
#include "bg.h"
#include "map.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define EFFECT_TEXTURE_SIZE_X	(100)		// テクスチャサイズ
#define EFFECT_TEXTURE_SIZE_Y	(100)		// 同上

#define EFFECT_TIME_ANIMATION			(1)	// アニメーションの切り替わるカウント

#define EMISSION_FULL 0		//パーティクル全生成フラグ
#define EMISSION_RATE 5		//パーティクルの生成間隔(duration/EMISSION_RATEの数分エフェクトが出る)

#define EMISSION_WIDTH  50	//パーティクル生成範囲（横幅）
#define EMISSION_HEIGHT 50	//パーティクル生成範囲（高さ）


#define TEXTURE_MAX					(2)		// テクスチャの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void ResetPlayerParticle(int i, int n);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/levelup01.png",
	"data/TEXTURE/effect01.png",

};

//// アニメパターン数(X,Y)
//int animepatternX;
//int animepatternY;
//// アニメパターン数の合計
int animepatternNUM;

static BOOL		g_Load = FALSE;								// 初期化を行ったかのフラグ

// エフェクト構造体
static PLAYER_EFFECT	effectWk[TEXTURE_MAX][PLAYER_NUM_EFFECTS];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayerEffect(void)
{
	ID3D11Device* pDevice = GetDevice();

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


	// 初期化処理
	for (int j = 0; j < TEXTURE_MAX; j++)
	{
		for (int i = 0; i < PLAYER_NUM_EFFECTS; i++)
		{
			effectWk[j][i].use = false;
			effectWk[j][i].elapsed = 0;

			for (int n = 0; n < PLAYER_NUM_PARTS; n++)
			{
				ResetPlayerParticle(i, n);
			}
		}

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// パーティクルのリセット
//=============================================================================
void ResetPlayerParticle(int i, int n) 
{
	for (int j = 0; j < TEXTURE_MAX; j++)
	{

		effectWk[j][i].pParticle[n].pos = XMFLOAT3(effectWk[j][i].pos.x + rand() % EMISSION_WIDTH - (EMISSION_WIDTH / 2), effectWk[j][i].pos.y + rand() % EMISSION_HEIGHT - (EMISSION_HEIGHT / 2), 0.0f);	// 座標データを初期化
		effectWk[j][i].pParticle[n].move = XMFLOAT3(0.0f, 0.0f, 0.0f);					// 移動量

		effectWk[j][i].pParticle[n].PatternAnim = 0;									// アニメパターン番号をランダムで初期化
		effectWk[j][i].pParticle[n].CountAnim = 0;										// アニメカウントを初期化
		effectWk[j][i].pParticle[n].liveTime = 2000;
		effectWk[j][i].pParticle[n].isFinish = 0;
	}
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayerEffect(void)
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
void UpdatePlayerEffect(void)
{
	for (int j = 0; j < TEXTURE_MAX; j++)
	{

		for (int i = 0; i < PLAYER_NUM_EFFECTS; i++)
		{
			if (effectWk[j][i].use)
			{
				if (effectWk[j][i].isEnding) {
					effectWk[j][i].use = FALSE;
					continue;
				}

				//エミットが有効であればエフェクト作成処理を行う
				if (effectWk[j][i].isRemoveOnFinish == FALSE)
				{

					//エフェクト作成レートの増加処理
					if (effectWk[j][i].effectCount < PLAYER_NUM_PARTS)
						effectWk[j][i].emitCounter++;

					//バッファに空きがあり、追加タイミングが来ていれば新たなエフェクトを追加する
					while ((effectWk[j][i].effectCount < PLAYER_NUM_PARTS) && (effectWk[j][i].emitCounter > EMISSION_RATE))
					{
						//全体追加フラグがONであれば空き領域全てに新たなエフェクトを追加する
						if (EMISSION_FULL)
							effectWk[j][i].effectCount = PLAYER_NUM_PARTS;
						else
							effectWk[j][i].effectCount++;

						//エフェクト作成レートの初期化
						effectWk[j][i].emitCounter = 0;
					}

					effectWk[j][i].elapsed++;

					//時間超過
					if ((effectWk[j][i].duration != -1) && (effectWk[j][i].duration < effectWk[j][i].elapsed)) {
						effectWk[j][i].isRemoveOnFinish = TRUE;
					}
				}

				int effectIndex = 0;

				//エフェクトの更新処理(後ほど改善する)
				while (effectIndex < effectWk[j][i].effectCount)
				{
					if (effectWk[j][i].pParticle[effectIndex].liveTime > 0)
					{
						//生存フレーム減少
						effectWk[j][i].pParticle[effectIndex].liveTime--;

						//アニメパターン進行
						if (++effectWk[j][i].pParticle[effectIndex].CountAnim > EFFECT_TIME_ANIMATION) {
							//アニメパターンが最大値に達した場合でも終了(30を変更する)
							if (++effectWk[j][i].pParticle[effectIndex].PatternAnim >= (30 - 1))
							{
								effectWk[j][i].pParticle[effectIndex].PatternAnim = 30 - 1;
								effectWk[j][i].pParticle[effectIndex].liveTime = 0;
							}

							effectWk[j][i].pParticle[effectIndex].CountAnim = 0;
						}

						effectIndex++;
					}
					else {
						if (effectWk[j][i].isRemoveOnFinish) {
							if (effectWk[j][i].pParticle[effectIndex].isFinish == 0)
							{
								effectWk[j][i].pParticle[effectIndex].isFinish = 1;
								effectWk[j][i].numFinish++;
							}
							//終了処理
							if (effectWk[j][i].numFinish == effectWk[j][i].effectCount)
							{
								effectWk[j][i].isEnding = TRUE;
								break;
							}
							effectIndex++;
						}
						else {
							//バッファを初期化する
							ResetPlayerParticle(i, effectIndex);

							//末尾でなければインデックスを詰める
							if (effectIndex != (effectWk[j][i].effectCount - 1))
							{
								effectWk[j][i].pParticle[effectIndex] = effectWk[j][i].pParticle[effectWk[j][i].effectCount - 1];
								ResetPlayerParticle(i, (effectWk[j][i].effectCount - 1));
							}
							effectWk[j][i].effectCount--;
						}
					}
				}
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayerEffect(void)
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

		BG* bg = GetBG();

		PLAYER* player = GetPlayer();
		float scrollX = GetScrollX();
		float scrollY = GetScrollY();


			for (int i = 0; i < PLAYER_NUM_EFFECTS; i++)
			{
				if (effectWk[TEXTURE_LVUP][i].use == TRUE)		// このエフェクトが使われている？
				{		// Yes
					// テクスチャ設定
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_LVUP]);

					// レベルアップエフェクト
					for (int n = 0; n < effectWk[TEXTURE_LVUP][i].effectCount; n++)
					{
						if (effectWk[TEXTURE_LVUP][i].pParticle[n].isFinish == 0) 
						{

							//バレットの位置やテクスチャー座標を反映
							float px = effectWk[TEXTURE_LVUP][i].pParticle[n].pos.x - scrollX;	// エフェクトの表示位置X
							float py = effectWk[TEXTURE_LVUP][i].pParticle[n].pos.y - scrollY;	// エフェクトの表示位置Y

							float pw = EFFECT_TEXTURE_SIZE_X;			// エフェクトの表示幅
							float ph = EFFECT_TEXTURE_SIZE_Y;			// エフェクトの表示高さ

							px -= EFFECT_TEXTURE_SIZE_X / 4;
							py -= EFFECT_TEXTURE_SIZE_Y / 4;

							float tw = 1.0f / 10;	// テクスチャの幅
							float th = 1.0f /  1;	// テクスチャの高さ
							float tx = (float)(effectWk[TEXTURE_LVUP][i].pParticle[n].PatternAnim % 10) * tw;	// テクスチャの左上X座標
							float ty = (float)(effectWk[TEXTURE_LVUP][i].pParticle[n].PatternAnim / 10) * th;	// テクスチャの左上Y座標

							// １枚のポリゴンの頂点とテクスチャ座標を設定
							SetSpriteColorRotation(g_VertexBuffer,
								px, py, pw, ph,
								tx, ty, tw, th,
								XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0.0f);

							// ポリゴン描画
							GetDeviceContext()->Draw(4, 0);
						}
					}
				}

				if (effectWk[TEXTURE_EFFECT01][i].use == TRUE)		// このエフェクトが使われている？
				{		// Yes
					// テクスチャ設定
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_EFFECT01]);

					// レベルアップエフェクト
					for (int n = 0; n < effectWk[TEXTURE_EFFECT01][i].effectCount; n++)
					{
						if (effectWk[TEXTURE_EFFECT01][i].pParticle[n].isFinish == 0)
						{

							//バレットの位置やテクスチャー座標を反映
							float px = effectWk[TEXTURE_EFFECT01][i].pParticle[n].pos.x - scrollX;	// エフェクトの表示位置X
							float py = effectWk[TEXTURE_EFFECT01][i].pParticle[n].pos.y - scrollY;	// エフェクトの表示位置Y

							float pw = EFFECT_TEXTURE_SIZE_X;			// エフェクトの表示幅
							float ph = EFFECT_TEXTURE_SIZE_Y;			// エフェクトの表示高さ

							px -= EFFECT_TEXTURE_SIZE_X / 4;
							py -= EFFECT_TEXTURE_SIZE_Y / 4;

							float tw = 1.0f / 10;	// テクスチャの幅
							float th = 1.0f / 1;	// テクスチャの高さ
							float tx = (float)(effectWk[TEXTURE_EFFECT01][i].pParticle[n].PatternAnim % 10) * tw;	// テクスチャの左上X座標
							float ty = (float)(effectWk[TEXTURE_EFFECT01][i].pParticle[n].PatternAnim / 10) * th;	// テクスチャの左上Y座標

							// １枚のポリゴンの頂点とテクスチャ座標を設定
							SetSpriteColorRotation(g_VertexBuffer,
								px, py, pw, ph,
								tx, ty, tw, th,
								XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);

							// ポリゴン描画
							GetDeviceContext()->Draw(4, 0);
						}
					}
				}

			}

		
}


//=============================================================================
// エフェクトのセット
// 
// int duration		エフェクト発生源の生存時間
//=============================================================================
void SetPlayerEffect(float x, float y, int duration, int j)
{

		// もし未使用のエフェクトが無かったら実行しない( =これ以上表示できないって事 )
		for (int i = 0; i < PLAYER_NUM_EFFECTS; i++)
		{
			if (effectWk[j][i].use == false)		// 未使用状態のエフェクトを見つける
			{
				effectWk[j][i].use = true;
				effectWk[j][i].isEnding = false;
				effectWk[j][i].isRemoveOnFinish = FALSE;

				effectWk[j][i].duration = duration;
				effectWk[j][i].pos.x = x;			// 座標をセット
				effectWk[j][i].pos.y = y;			// 座標をセット

				effectWk[j][i].effectCount = 0;
				effectWk[j][i].elapsed = 0;
				effectWk[j][i].emitCounter = EMISSION_RATE;
				effectWk[j][i].numFinish = 0;

				//パーティクルの初期化
				for (int n = 0; n < PLAYER_NUM_PARTS; n++)
				{
					ResetPlayerParticle(i, n);
				}

				return;							// 1個セットしたので終了する
			}
		}
}
