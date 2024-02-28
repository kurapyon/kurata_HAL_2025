//=============================================================================
//
// ���C������ [main.cpp]
// Author : GP11A132 09 �q�c�E��
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
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"DirectX11"			// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int	g_Mode = MODE_TITLE;					// �N�����̉�ʂ�ݒ�

BOOL g_LoadGame = FALSE;					// NewGame

// �Q�[���I��������
BOOL g_Loop;

// pause�ϐ�
BOOL g_pause = FALSE;

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
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
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																			// �E�B���h�E�̍����W
		CW_USEDEFAULT,																			// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,										// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// �E�B���h�E���[�h���t���X�N���[�����[�h���̏���
	BOOL mode = TRUE;

	int id = MessageBox(NULL, "Window���[�h�Ńv���C���܂����H", "�N�����[�h", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:			// Yes�Ȃ�Window���[�h�ŋN��
		mode = TRUE;
		break;
	case IDNO:			// No�Ȃ�t���X�N���[�����[�h�ŋN��
		mode = FALSE;	// ���ɂ���ē����Ȃ���������
		break;
	case IDCANCEL:		// CANCEL�Ȃ�I��
	default:
		return -1;
		break;
	}

	// ����������(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	

	g_Loop = TRUE;
	// ���b�Z�[�W���[�v
	while(g_Loop)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������ �\���������̂�����
				// �v���C���[�̍��W(X,Y), �v���C���[�̖��G����, �v���C���[�̃��C�t��\�������� 
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

	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
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
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �`��̏�����
	InitRenderer(hInstance, hWnd, bWindow);

	// �J�����̏�����
	InitCamera();

	// ���C�g��L����
	SetLightEnable(FALSE);

	// �w�ʃ|���S�����J�����O
	SetCullingMode(CULL_MODE_BACK);

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	// �T�E���h�����̏�����
	InitSound(hWnd);

	// �t�F�[�h�����̏�����
	InitFade();


	// �ŏ��̃��[�h���Z�b�g
	SetMode(g_Mode);	// ������SetMode�̂܂܂ŁI

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	// �I���̃��[�h���Z�b�g
	SetMode(MODE_MAX);

	// �t�F�[�h�̏I������
	UninitFade();

	// �T�E���h�̏I������
	UninitSound();

	// ���͂̏I������
	UninitInput();

	// �J�����̏I������
	UninitCamera();

	// �����_���[�̏I������
	UninitRenderer();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͂̍X�V����
	UpdateInput();

	// �J�����X�V
	UpdateCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̍X�V
		UpdateTitle();
		break;

	case MODE_EXPLANATION:		// ������ʂ̍X�V
		UpdateExplanation();
		break;

	case MODE_TUTORIAL:		// �`���[�g���A��
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


	case MODE_GAME:			// �Q�[����ʂ̍X�V

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


	case MODE_CLEAR_RESULT:		// �N���A���U���g��ʂ̍X�V
		UpdateClearResult();
		break;

	case MODE_GAMEOVER_RESULT:	// �Q�[���I�[�o�[���U���g�̍X�V

		UpdateGameOverresult();
		break;
	}

	UpdateFade();			// �t�F�[�h�̍X�V����
}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	SetCamera();

	// 2D�̕���`�悷�鏈��
	SetViewPort(TYPE_FULL_SCREEN);

	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);


	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̕`��
		DrawTitle();
		break;

	case MODE_EXPLANATION:	// �Q�[������

			DrawExplanation();

		break;

	case MODE_TUTORIAL:		// �`���[�g���A��
		DrawBG();
		DrawMap();
		DrawTutorial();
		DrawEnemyBullet();
		DrawPlayer();
		DrawCutting();
		DrawPause();
		break;

	case MODE_GAME:			// �Q�[����ʂ̕`��/�d�Ȃ鏇�Ԃ��ӎ����Ă�
		DrawBG();
		DrawEnemy();		// �����ă}�b�v�̉��ɏd�˂Ă���
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


	case MODE_CLEAR_RESULT:		// �N���A���U���g��ʂ̕`��
		DrawClearResult();
		break;

	case MODE_GAMEOVER_RESULT:	// �Q�[���I�[�o�[���U���g�̕`��

		DrawGameOverresult();
		break;
	}


	DrawFade();				// �t�F�[�h��ʂ̕`��


#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
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
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	// ���[�h��ς���O�ɑS������������������Ⴄ
	StopSound();			// �܂��Ȃ��~�߂�

	// ���[�h��ς���O�ɑS������������������Ⴄ

	// �^�C�g����ʂ̏I������
	UninitTitle();

	// �Q�[�������̏I������
	UninitExplanation();

	// BG�̏I������
	UninitBG();

	// �`���[�g���A���G�l�~�[�̏I������
	UninitTutorial();

	//	field���I������
	UninitMap();

	// �v���C���[�̏I������
	UninitPlayer();

	// ���x���̏I������
	UninitLevel();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// pause���j���[�̏I������
	UninitPause();

	// �J�b�e�B���O�̏I������
	UninitCutting();

	// �o���b�g�̏I������
	UninitBullet();
	UninitBulletBos();
	UninitEnemyBullet();
	UninitBulletAtc();
	UninitStrikeBullet();

	// �X�R�A�̏I������
	UninitLevel();

	// ���U���g�̏I������
	UninitClearResult();
	UninitGameOverresult();

	// �v���C���[�G�t�F�N�g�̏I������
	UninitPlayerEffect();

	// �G�l�~�[�G�t�F�N�g�̏I������
	UninitEnemyEffect();

	g_Mode = mode;	// ���̃��[�h���Z�b�g���Ă���

	switch (g_Mode)
	{
	case MODE_TITLE:
		// �^�C�g����ʂ̏�����
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_EXPLANATION:	// �Q�[�������̏�����
		InitExplanation();
		PlaySound(SOUND_LABEL_BGM_explanation);

		break;

	case MODE_TUTORIAL:		// �`���[�g���A��
		InitPause();
		InitTutorial();
		InitBG();
		InitMap();
		InitEnemyBullet();
		InitPlayer();
		InitCutting();

		break;


	case MODE_GAME:
		// �Q�[����ʂ̏�����
		// �Ȃ̕ύX
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


		// ���[�h�Q�[���������炷�ׂĂ̏��������I�������ɃZ�[�u�f�[�^��ǂݍ���
		if (g_LoadGame == TRUE)
		{
			LoadData();
			g_LoadGame = FALSE;		// ���[�h��������t���O��Clear����
		}

		break;

	case MODE_CLEAR_RESULT:	// �N���A���U���g

		InitClearResult();
		PlaySound(SOUND_LABEL_BGM_clear);

		break;

	case MODE_GAMEOVER_RESULT:	// �Q�[���I�[�o�[���U���g

		InitGameOverresult();
		PlaySound(SOUND_LABEL_BGM_gameover);

		break;

	case MODE_MAX:
		break;
	}
}

//=============================================================================
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


//=============================================================================
// �j���[�Q�[�������[�h�Q�[�������Z�b�g����
//=============================================================================
void SetLoadGame(BOOL flg)
{
	g_LoadGame = flg;
}

BOOL GetPause(void)
{
	return g_pause;
}

// pause�̐؂�ւ�
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