//=============================================================================
//
// メイン処理 [main.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "debugproc.h"
#include "input.h"

#include "title.h"
#include "explanation.h"
#include "bg.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "Level.h"
#include "clearresult.h"
#include "gameoverresult.h"
#include "sound.h"
#include "fade.h"

#include "file.h"

#include "playereffect.h"
#include "map.h"
#include "bosbullet.h"
#include "enemyeffect.h"
#include "cut.h"
#include "enemybullet.h"
#include "bossatc.h"
#include "tutorialenemy.h"
#include "strikebullet.h"
#include "pause.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"DirectX11"			// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

int	g_Mode = MODE_TITLE;					// 起動時の画面を設定

BOOL g_LoadGame = FALSE;					// NewGame

// ゲーム終了させる
BOOL g_Loop;

// pause変数
BOOL g_pause = FALSE;

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																			// ウィンドウの左座標
		CW_USEDEFAULT,																			// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,										// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// ウィンドウモードかフルスクリーンモードかの処理
	BOOL mode = TRUE;

	int id = MessageBox(NULL, "Windowモードでプレイしますか？", "起動モード", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:			// YesならWindowモードで起動
		mode = TRUE;
		break;
	case IDNO:			// Noならフルスクリーンモードで起動
		mode = FALSE;	// 環境によって動かない事がある
		break;
	case IDCANCEL:		// CANCELなら終了
	default:
		return -1;
		break;
	}

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	

	g_Loop = TRUE;
	// メッセージループ
	while(g_Loop)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する 表示したいのを書く
				// プレイヤーの座標(X,Y), プレイヤーの無敵時間, プレイヤーのライフを表示させる 
				BOSS* boss = GetBoss();
				PLAYER* player = GetPlayer();
				ENEMY* enemy = GetEnemy();

				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " PI:%d, PL:%d, EL:%d, EI:%d, PX:%d, PY:%d", player[0].invincibleCnt, player[0].life, (int)enemy[0].life, enemy[0].enemy_invincibleCnt,
																										(int)player[0].pos.x, (int)player[0].pos.y);

				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// 描画の初期化
	InitRenderer(hInstance, hWnd, bWindow);

	// カメラの初期化
	InitCamera();

	// ライトを有効化
	SetLightEnable(FALSE);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	// サウンド処理の初期化
	InitSound(hWnd);

	// フェード処理の初期化
	InitFade();


	// 最初のモードをセット
	SetMode(g_Mode);	// ここはSetModeのままで！

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// 終了のモードをセット
	SetMode(MODE_MAX);

	// フェードの終了処理
	UninitFade();

	// サウンドの終了処理
	UninitSound();

	// 入力の終了処理
	UninitInput();

	// カメラの終了処理
	UninitCamera();

	// レンダラーの終了処理
	UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

	// カメラ更新
	UpdateCamera();

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の更新
		UpdateTitle();
		break;

	case MODE_EXPLANATION:		// 説明画面の更新
		UpdateExplanation();
		break;

	case MODE_TUTORIAL:		// チュートリアル
		if (g_pause == FALSE)
		{
			UpdatTutorial();
			UpdateBG();
			UpdateMap();
			UpdateEnemyBullet();
			UpdatePlayer();
			UpdateCutting();
		}

		UpdatePause();

		break;


	case MODE_GAME:			// ゲーム画面の更新

		if (g_pause == FALSE)
		{
			UpdateBG();
			UpdateMap();
			UpdatePlayer();
			UpdateEnemy();
			UpdateCutting();
			UpdateEnemyBullet();
			UpdateBulletBos();
			UpdateStrikeBullet();
			UpdateBullet();
			UpdateBulletAtc();
			UpdatePlayerEffect();
			UpdateEnemyEffect();
			UpdateLevel();

		}
		
		UpdatePause();

		break;


	case MODE_CLEAR_RESULT:		// クリアリザルト画面の更新
		UpdateClearResult();
		break;

	case MODE_GAMEOVER_RESULT:	// ゲームオーバーリザルトの更新

		UpdateGameOverresult();
		break;
	}

	UpdateFade();			// フェードの更新処理
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();

	SetCamera();

	// 2Dの物を描画する処理
	SetViewPort(TYPE_FULL_SCREEN);

	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);


	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の描画
		DrawTitle();
		break;

	case MODE_EXPLANATION:	// ゲーム説明

			DrawExplanation();

		break;

	case MODE_TUTORIAL:		// チュートリアル
		DrawBG();
		DrawMap();
		DrawTutorial();
		DrawEnemyBullet();
		DrawPlayer();
		DrawCutting();
		DrawPause();
		break;

	case MODE_GAME:			// ゲーム画面の描画/重なる順番を意識してね
		DrawBG();
		DrawEnemy();		// あえてマップの下に重ねている
		DrawMap();
		DrawBulletBos();
		DrawStrikeBullet();
		DrawEnemyBullet();
		DrawBulletAtc();
		DrawBullet();		
		DrawPlayer();
		DrawCutting();
		DrawPlayerEffect();
		DrawEnemyEffect();
		DrawLevel();
		DrawPause();
		break;


	case MODE_CLEAR_RESULT:		// クリアリザルト画面の描画
		DrawClearResult();
		break;

	case MODE_GAMEOVER_RESULT:	// ゲームオーバーリザルトの描画

		DrawGameOverresult();
		break;
	}


	DrawFade();				// フェード画面の描画


#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();
#endif

	// バックバッファ、フロントバッファ入れ替え
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}

void GameLoop(void)
{
	g_Loop = FALSE;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif



//=============================================================================
// モードの設定
//=============================================================================
void SetMode(int mode)
{
	// モードを変える前に全部メモリを解放しちゃう
	StopSound();			// まず曲を止める

	// モードを変える前に全部メモリを解放しちゃう

	// タイトル画面の終了処理
	UninitTitle();

	// ゲーム説明の終了処理
	UninitExplanation();

	// BGの終了処理
	UninitBG();

	// チュートリアルエネミーの終了処理
	UninitTutorial();

	//	fieldも終了処理
	UninitMap();

	// プレイヤーの終了処理
	UninitPlayer();

	// レベルの終了処理
	UninitLevel();

	// エネミーの終了処理
	UninitEnemy();

	// pauseメニューの終了処理
	UninitPause();

	// カッティングの終了処理
	UninitCutting();

	// バレットの終了処理
	UninitBullet();
	UninitBulletBos();
	UninitEnemyBullet();
	UninitBulletAtc();
	UninitStrikeBullet();

	// スコアの終了処理
	UninitLevel();

	// リザルトの終了処理
	UninitClearResult();
	UninitGameOverresult();

	// プレイヤーエフェクトの終了処理
	UninitPlayerEffect();

	// エネミーエフェクトの終了処理
	UninitEnemyEffect();

	g_Mode = mode;	// 次のモードをセットしている

	switch (g_Mode)
	{
	case MODE_TITLE:
		// タイトル画面の初期化
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_EXPLANATION:	// ゲーム説明の初期化
		InitExplanation();
		PlaySound(SOUND_LABEL_BGM_explanation);

		break;

	case MODE_TUTORIAL:		// チュートリアル
		InitPause();
		InitTutorial();
		InitBG();
		InitMap();
		InitEnemyBullet();
		InitPlayer();
		InitCutting();

		break;


	case MODE_GAME:
		// ゲーム画面の初期化
		// 曲の変更
		PlaySound(SOUND_LABEL_BGM_boss01);
	
		InitPause();
		InitBG();
		InitMap();
		InitLevel();
		InitPlayer();
		InitEnemy();
		InitCutting();
		InitStrikeBullet();
		InitEnemyBullet();
		InitBulletBos();
		InitBullet();
		InitBulletAtc();
		InitPlayerEffect();
		InitEnemyEffect();
		break;


		// ロードゲームだったらすべての初期化が終わった後にセーブデータを読み込む
		if (g_LoadGame == TRUE)
		{
			LoadData();
			g_LoadGame = FALSE;		// ロードしたからフラグをClearする
		}

		break;

	case MODE_CLEAR_RESULT:	// クリアリザルト

		InitClearResult();
		PlaySound(SOUND_LABEL_BGM_clear);

		break;

	case MODE_GAMEOVER_RESULT:	// ゲームオーバーリザルト

		InitGameOverresult();
		PlaySound(SOUND_LABEL_BGM_gameover);

		break;

	case MODE_MAX:
		break;
	}
}

//=============================================================================
// モードの取得
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


//=============================================================================
// ニューゲームかロードゲームかをセットする
//=============================================================================
void SetLoadGame(BOOL flg)
{
	g_LoadGame = flg;
}

BOOL GetPause(void)
{
	return g_pause;
}

// pauseの切り替え
void SetPause(void)
{
	if (g_pause == FALSE)
	{
		g_pause = TRUE;
	}
	else
	{
		g_pause = FALSE;
	}
}