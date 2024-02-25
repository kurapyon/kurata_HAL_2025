//=============================================================================
//
// メッシュ地面の処理 [meshfield.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "model.h"
#include "meshfield.h"
#include "renderer.h"
#include "player.h"
//#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX		(2)				// テクスチャの数

// 波動方程式関連	何回も書くのが大変だからマクロ定義
// マクロでとオーバーヘッドが起きない

// COORD(z, x)→これでワンセット
#define COORD(z, x) (z) * (g_nNumBlockXField + 1) + (x) //配列番号を求める
#define DAMP_PARAM  (0.001f)							// 減衰どれだけすぐに波が静かになるか デルタTをTにしている
#define T			(0.1f)								// 波動方程式の定数 値が大きくなるとすぐ消えるらしい

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11Buffer					*g_IndexBuffer = NULL;	// インデックスバッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;				// テクスチャ番号

static XMFLOAT3		g_posField;								// ポリゴン表示位置の中心座標
static XMFLOAT3		g_rotField;								// ポリゴンの回転角

static int			g_nNumBlockXField, g_nNumBlockZField;	// ブロック数
static int			g_nNumVertexField;						// 総頂点数	
static int			g_nNumVertexIndexField;					// 総インデックス数
static int			g_nNumPolygonField;						// 総ポリゴン数
static float		g_fBlockSizeXField, g_fBlockSizeZField;	// ブロックサイズ

static char* g_TextureName[] = {
	"data/TEXTURE/wave001.png",
	"data/TEXTURE/wave002.png",
};


// 波の処理

static VERTEX_3D	*g_Vertex = NULL;

// 波の高さ = sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
static XMFLOAT3		g_Center;					// 波の発生場所
static float		g_Time = 0.0f;				// 波の経過時間
static float		g_wave_frequency  = 2.0f;	// 波の周波数
static float		g_wave_correction = 0.02f;	// 波の距離補正
static float		g_wave_amplitude  = 20.0f;	// 波の振幅

// 過去の点の高さ(配列の最初の点みたいなもの)
static float* pointOLD  = NULL;

// 現在の点の高さ
static float* pointNOW  = NULL;

// 未来の点の高さ
static float* pointNEXT = NULL;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	// ポリゴン表示位置の中心座標を設定
	g_posField = pos;

	g_rotField = rot;

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// ブロック数の設定
	g_nNumBlockXField = nNumBlockX;
	g_nNumBlockZField = nNumBlockZ;

	// 頂点数の設定
	g_nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// インデックス数の設定
	g_nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// ポリゴン数の設定
	g_nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// ブロックサイズの設定
	g_fBlockSizeXField = fBlockSizeX;
	g_fBlockSizeZField = fBlockSizeZ;


	// 頂点情報をメモリに作っておく（波の為）
	// 波の処理
	// 波の高さ = sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
	g_Vertex = new VERTEX_3D[g_nNumVertexField];
	g_Center = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 波の発生場所
	g_Time = 0.0f;								// 波の経過時間(＋とーとで内側外側になる)
	g_wave_frequency = 1.0f;					// 波の周波数（上下運動の速さ）
	g_wave_correction = 0.02f;					// 波の距離補正（変えなくても良いと思う）
	g_wave_amplitude = 30.0f;					// 波の振幅(波の高さ)

	for (int z = 0; z < (g_nNumBlockZField + 1); z++)
	{
		for (int x = 0; x < (g_nNumBlockXField + 1); x++)
		{
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x = -(g_nNumBlockXField / 2.0f) * g_fBlockSizeXField + x * g_fBlockSizeXField;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField - z * g_fBlockSizeZField;

			float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
			float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

			// 波紋の中心点からの距離を得る
			float len = (float)sqrt(dx * dx + dz * dz);

			// 波の高さを、sin関数で得る
			// 波の高さ　= sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = sinf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;


			// 法線の設定
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Normal = XMFLOAT3(0.0f, 1.0, 0.0f);

			// 反射光の設定
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			float texSizeX = 1.0f;
			float texSizeZ = 1.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.x = texSizeX * x;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.y = texSizeZ * z;
		}

	}

	// 波動方程式
	
	// 何個頂点を使うか
	pointOLD  = new float[g_nNumVertexField];
	pointNOW  = new float[g_nNumVertexField];
	pointNEXT = new float[g_nNumVertexField];

	// 波動方程式の初期化
	{
		for (int i = 0; i < g_nNumVertexField; i++)
		{
			pointOLD[i]  = 0.0f;
			pointNOW[i]  = 0.0f;
			pointNEXT[i] = 0.0f;

		}
	}

	// 波の初期位置
	{
		for (int z = 1; z < g_nNumBlockZField - 1; z++)
		{
			for (int x = 1; x < g_nNumBlockXField - 1; x++)
			{
				float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
				float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

				//波紋の中心点からの距離を得る
				float len = (float)sqrt(dx * dx + dz * dz);
				float a = exp(-len / 10) * 50;
				pointOLD[COORD(z, x)] = a;

			}
		}
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertexField;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// インデックスバッファ生成
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * g_nNumVertexIndexField;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_IndexBuffer);


	{//頂点バッファの中身を埋める

		// 頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	{//インデックスバッファの中身を埋める

		// インデックスバッファのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short *pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for(int nCntVtxZ = 0; nCntVtxZ < g_nNumBlockZField; nCntVtxZ++)
		{
			if(nCntVtxZ > 0)
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1);
				nCntIdx++;
			}

			for(int nCntVtxX = 0; nCntVtxX < (g_nNumBlockXField + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
			}

			if(nCntVtxZ < (g_nNumBlockZField - 1))
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + g_nNumBlockXField;
				nCntIdx++;
			}
		}

		GetDeviceContext()->Unmap(g_IndexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
	// インデックスバッファの解放
	if (g_IndexBuffer) {
		g_IndexBuffer->Release();
		g_IndexBuffer = NULL;
	}

	// 頂点バッファの解放
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}

	// new演算子を使ったら、deleteが必要
	if (pointOLD)
	{
		delete[] pointOLD;
		pointOLD = NULL;
	}

	if (pointNOW)
	{
		delete[] pointNOW;
		pointNOW = NULL;
	}

	if (pointNEXT)
	{
		delete[] pointNEXT;
		pointNEXT = NULL;
	}




}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{

	PLAYER* player = GetPlayer();

	// 波動方程式
	if (GetKeyboardTrigger(DIK_1))
	{
		//波の初期位置
		{
			for (int z = 1; z < g_nNumBlockZField - 1; z++)
			{
				for (int x = 1; x < g_nNumBlockXField - 1; x++)
				{
					float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - player->pos.x;
					float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - player->pos.z;

					//波紋の中心点からの距離を得る
					float len = (float)sqrt(dx * dx + dz * dz);

					// ここの定数を変えると波が静かになる・ 波の強さ
					float a = exp(-len / 5) * 5;

					// ここで波を作っている
					pointNOW[COORD(z, x)] += a;
				}
			}


		}

		for (int z = 0; z < g_nNumBlockZField; z++)
		{
			pointNOW[COORD(z, 0)] = pointNOW[COORD(z, g_nNumBlockXField - 1)] = 0.0f;
		}

		for (int x = 0; x < g_nNumBlockXField; x++)
		{
			pointNOW[COORD(0, x)] = pointNOW[COORD(g_nNumBlockZField - 1, x)] = 0.0f;
		}
	}

	//端っこ以外の点 一回り小さい配列を参照している 隣がいないとかを防ぐためらしい
	for (int z = 1; z < g_nNumBlockZField - 1; z++)
	{
		// 今と過去の点から未来の点を導き出してる
		for (int x = 1; x < g_nNumBlockXField - 1; x++)
		{
			// 減衰の式
			float damp = -DAMP_PARAM * (pointNOW[COORD(z, x)] - pointOLD[COORD(z, x)]);

			// 波動方程式の中身
			pointNEXT[COORD(z, x)] =
				2.0f * pointNOW[COORD(z, x)] - pointOLD[COORD(z, x)]
				+ T * (pointNOW[COORD(z, x + 1)] + pointNOW[COORD(z, x - 1)]
					+ pointNOW[COORD(z + 1, x)] + pointNOW[COORD(z - 1, x)]
					- 4.0f * pointNOW[COORD(z, x)]) + damp;

		}
	}

	////境界条件	通常の波
	//for (int z = 0; z < g_nNumBlockZField; z++)
	//{
	//	pointNEXT[COORD(z, 0)] = pointNEXT[COORD(z, g_nNumBlockXField - 1)] = 0.0f;
	//}
	//for (int x = 0; x < g_nNumBlockXField; x++)
	//{
	//	pointNEXT[COORD(0, x)] = pointNEXT[COORD(g_nNumBlockZField - 1, x)] = 0.0f;
	//}

	////境界条件	指定した範囲外に波を絶たせる
	for (int z = g_nNumBlockZField/4; z < g_nNumBlockZField * 3/4; z++)
	{
	    for (int x = g_nNumBlockXField/4; x < g_nNumBlockXField * 3 / 4; x++)
	    {
	        pointNEXT[COORD(z, x)] = pointNEXT[COORD(z, x)] = 0.0f;
	    }
	}


	//// プレイヤーの周りに波を絶たせない
	//-player->pos.x;
	//-player->pos.z;
	//int px = ( g_posField.x + player->pos.x) / g_fBlockSizeXField + g_nNumBlockXField/2;
	//int pz = (g_posField.z - player->pos.z)/ g_fBlockSizeZField + g_nNumBlockZField/2;

	//for (int z = pz - 3; z < pz + 3; z++)
	//{
	//    for (int x = px - 3; x < px + 3; x++)
	//    {
	//        if (z < 0 || z >= g_nNumBlockZField)
	//        {
	//            continue;
	//        }
	//        if (x < 0 || x >= g_nNumBlockXField)
	//        {
	//            continue;
	//        }

	//        pointNEXT[COORD(z, x)] = pointNEXT[COORD(z, x)] = 0.0f;
	//    }
	//}


	//点の更新 1フレームごとに点の更新をしている
	for (int i = 0; i < g_nNumVertexField; i++)
	{
		pointOLD[i] = pointNOW[i];
		pointNOW[i] = pointNEXT[i];
	}

	//頂点に代入
	for (int z = 0; z < g_nNumBlockZField; z++)
	{
		for (int x = 1; x < g_nNumBlockXField - 1; x++)
		{
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = pointNOW[z * (g_nNumBlockXField + 1) + x];

		}

	}

	// SetWaveで引数にplayer.pos、定数で関数を作る GetKeyの中を関数に

	
	// ---------使わない↓----------------//
	
	//// 波の処理
	//float dt = 0.03f;

	//for (int z = 0; z < g_nNumBlockZField; z++)
	//{
	//	for (int x = 0; x < g_nNumBlockXField; x++)
	//	{
	//		float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
	//		float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

	//		// 波紋の中心点からの距離を得る
	//		float len = (float)sqrt(dx * dx + dz * dz);

	//		// 波の高さを、sin関数で得る
	//		//  　波の高さ　= sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
	//		g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = sinf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;
	//		//g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;
	//	}

	//}
	//g_Time += dt;

	//--------------------------//

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	// 全頂点情報を毎回上書きしているのはDX11ではこの方が早いからです
	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshField(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	material.noTexSampling = 2;
	SetMaterial(material);

	// テクスチャ設定(最初の0 → t0)
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
	GetDeviceContext()->PSSetShaderResources(1, 1, &g_Texture[g_TexNo + 1]);



	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);


	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_nNumVertexIndexField, 0, 0);
}



