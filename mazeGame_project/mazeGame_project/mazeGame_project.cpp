// mazeGame_project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "mazeGame_project.h"

#include <time.h>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MAZEGAMEPROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAZEGAMEPROJECT));

	MSG msg;

	// 기본 메시지 루프입니다:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


void GameText();

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAZEGAMEPROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1 );
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MAZEGAMEPROJECT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	HWND hWnd = CreateWindowW(szWindowClass, L"202407030_기말고사과제", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 1500, 900, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
/// g_me = 캐릭터, g_maze_TF = 벽인지 아닌지 여부 확인
RECT g_me;

/// x와 y좌표 전역변수화

int g_me_x, g_me_y;


/// 아이템의 좌표
int g_itemRow, g_itemCol;
int g_newItemRow, g_newItemCol;
int g_itemScore;

/// 0 : 벽/검정색 , 1 : 길/흰색, 3 : 출발, 4 : 아이템, 5 : 도착지점
/// 맵 크기 키워서 테스트

int g_maze[MAZE_ROWS][MAZE_COLS];

int g_destX, g_destY;
BOOL g_isDest;
BOOL g_destClear;
BOOL g_openDest;

int g_isGame;

WCHAR g_isGameText[100];

// 전역 변수
volatile int g_timerState = STOP; // 현재 타이머 상태 (volatile 필수)
volatile int g_playTime = 0;            // 게임 시간 (초 단위)

HANDLE g_hTimerThread;           // 타이머 쓰레드 핸들
HANDLE g_hStartButton;
HANDLE g_hResetButton;
HANDLE g_hPurseButton;

BOOL g_helpButton;

HWND g_hWnd;

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
void GetMap(HDC hdc)
{
	HBRUSH WallBrush = CreateSolidBrush(RGB(180, 180, 180));      // 검정색 (벽)
	HBRUSH RoadBrush = CreateSolidBrush(RGB(40, 40, 40)); // 흰색 (길)
	HBRUSH ItemBrush = CreateSolidBrush(RGB(255, 215, 0));   // 아이템: 금색
	HBRUSH CharBrush = CreateSolidBrush(RGB(0, 255, 0));     // 캐릭터: 형광 연두색

	// 펜을 NULL_PEN으로 설정하고 나중에 원래 펜 복원 (외곽선 제거)
	HPEN NullPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(hdc, NullPen);

	HBRUSH CurrentBrush;
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, RoadBrush); // 흰색으로 초기 브러시 설정

	/// 맵 구현 
	for (int i = 0; i < MAZE_ROWS; i++)
	{
		for (int j = 0; j < MAZE_COLS; j++)
		{
			int x = i * MAZE_BOX_SIZE;
			int y = j * MAZE_BOX_SIZE;

			if (g_maze[i][j] == ROAD) CurrentBrush = RoadBrush;
			else if (g_maze[i][j] == WALL) CurrentBrush = WallBrush;
			else if (g_maze[i][j] == CHAR) CurrentBrush = CharBrush;
			else if (g_maze[i][j] == ITEM) CurrentBrush = ItemBrush;
			else CurrentBrush = WallBrush;

			SelectObject(hdc, CurrentBrush);
			Rectangle(hdc, x, y, x + MAZE_BOX_SIZE, y + MAZE_BOX_SIZE);
		}
	}

	SelectObject(hdc, hOldBrush);
	SelectObject(hdc, hOldPen);

	// 만든 오브젝트 삭제
	DeleteObject(WallBrush);
	DeleteObject(RoadBrush);
	DeleteObject(ItemBrush);
	DeleteObject(CharBrush);
	DeleteObject(NullPen);
}
void SetMap()
{
	srand(time(NULL));
	/// 캐릭터 위치 및 크기 조정
	g_me_x = 1;
	g_me_y = 1;
	g_maze[g_me_x][g_me_y] = CHAR;
	g_me.left = 1 * MAZE_BOX_SIZE;
	g_me.top = 1 * MAZE_BOX_SIZE;
	g_me.right = g_me.left * MAZE_BOX_SIZE;
	g_me.bottom = g_me.top * MAZE_BOX_SIZE;

	/// 초기화
	for (int i = 0; i < MAZE_ROWS; i++)
	{
		for (int j = 0; j < MAZE_COLS; j++)
		{
			g_maze[i][j] = ROAD;
		}
	}

	//// 내부 랜덤 벽 생성
	for (int i = 2; i < MAZE_ROWS - 2; i += 2)
	{
		for (int j = 2; j < MAZE_COLS - 2; j += 2)
		{
			g_maze[i][j] = WALL;
			int direction = rand() % 4;
			switch (direction)
			{
			case 0: if (j > 2) g_maze[i][j - 1] = WALL; break;
			case 1: if (j < MAZE_ROWS - 3) g_maze[i][j + 1] = WALL; break;
			case 2: if (i > 2) g_maze[i - 1][j] = WALL; break;
			case 3: if (i < MAZE_COLS - 3) g_maze[i + 1][j] = WALL; break;
			}
		}
	}

	// 테두리 로직
	for (int i = 0; i < MAZE_ROWS; i++) {
		g_maze[i][0] = WALL;
		g_maze[i][MAZE_COLS - 1] = WALL;
	}
	for (int i = 0; i < MAZE_COLS; i++) {
		g_maze[0][i] = WALL;				/// 우
		g_maze[MAZE_ROWS - 1][i] = WALL;
	}

	/// 랜덤으로 벽 뚫기
	for (int i = 0; i < MAZE_ROWS; i++)
	{
		int x = (rand() % (MAZE_ROWS - 2)) + 1;
		int y = (rand() % (MAZE_COLS - 2)) + 1;
		g_maze[x][y] = ROAD;
	}

	g_maze[1][1] = CHAR;

	/// 아이템 설정
	int nextItemRow = g_newItemRow;
	int nextItemCol = g_newItemCol;

	while (g_maze[nextItemRow][nextItemCol] == WALL)
	{
		nextItemRow = rand() % MAZE_ROWS;
		nextItemCol = rand() % MAZE_COLS;
	}
	g_maze[nextItemRow][nextItemCol] = ITEM;
	g_newItemRow = nextItemRow;
	g_newItemCol = nextItemCol;

}

/// 스레드의 핸들을 보관할 전역 변수

HANDLE g_hThread;

typedef struct data_set
{
	HWND m_hWnd;
	LPARAM m_lParam;
	WPARAM m_wParam;
}DS, * PDS;


DWORD WINAPI MoveChar(LPVOID ThWnd)
{
	srand(time(NULL));

	// PDS ds = (PDS)ThWnd;
	PDS ds = (PDS)ThWnd;

	// 2. [중요] 데이터를 백업 (메모리 해제 전에 값 꺼내기)
	WPARAM key = ds->m_wParam;
	HWND hWnd = ds->m_hWnd;
	HDC hdc = GetDC(hWnd);

	delete ds;

	int nx = g_me_x;
	int ny = g_me_y;
	int itemScore = g_itemScore;
	/// 캐릭터 구성
	HBRUSH CharBrush = CreateSolidBrush(RGB(0, 255, 0));     // 캐릭터: 형광 연두색
	HBRUSH RoadBrush = CreateSolidBrush(RGB(40, 40, 40)); // 흰색 (길)
	HBRUSH ItemBrush = CreateSolidBrush(RGB(255, 215, 0));   // 아이템: 금색

	switch (key)
	{
	case VK_LEFT:	nx--;				break;
	case VK_RIGHT:	nx++;				break;
	case VK_UP:		ny--;				break;
	case VK_DOWN:	ny++;				break;
	}
	// 3. 통합 경계 및 충돌 검사 (범위를 벗어나지 않고, 벽이 아닌 경우에만 이동 수행)
	if (g_maze[nx][ny] != WALL)
	{
		bool isItem = (g_maze[nx][ny] == ITEM);
		bool isDest = (g_maze[nx][ny] == DEST);

		g_maze[g_me_x][g_me_y] = ROAD; // 기존 위치 지우기

		int roadX = g_me_x * MAZE_BOX_SIZE;
		int roadY = g_me_y * MAZE_BOX_SIZE;
		SelectObject(hdc, RoadBrush);
		Rectangle(hdc, roadX - 1, roadY - 1, roadX + MAZE_BOX_SIZE, roadY + MAZE_BOX_SIZE);

		// 좌표 및 인덱스 업데이트
		g_me_x = nx;
		g_me_y = ny;

		// 픽셀 좌표 재계산 (이게 더 안전함)
		g_me.left = g_me_x * MAZE_BOX_SIZE;
		g_me.top = g_me_y * MAZE_BOX_SIZE;
		g_me.right = g_me.left + MAZE_BOX_SIZE;
		g_me.bottom = g_me.top + MAZE_BOX_SIZE;

		SelectObject(hdc, CharBrush);
		g_maze[g_me_x][g_me_y] = CHAR; // 새 위치에 캐릭터 표시
		Rectangle(hdc, g_me.left, g_me.top, g_me.right, g_me.bottom);

		if (isItem)
		{
			int nextItemRow = rand() % MAZE_ROWS;
			int nextItemCol = rand() % MAZE_COLS;

			while (g_maze[nextItemRow][nextItemCol] == WALL)
			{
				nextItemRow = rand() % MAZE_ROWS;
				nextItemCol = rand() % MAZE_COLS;
			}


			int nxItemX = nextItemRow * MAZE_BOX_SIZE;
			int nxItemY = nextItemCol * MAZE_BOX_SIZE;

			g_maze[nextItemRow][nextItemCol] = ITEM;
			SelectObject(hdc, ItemBrush);
			Rectangle(hdc, nxItemX, nxItemY, nxItemX + MAZE_BOX_SIZE, nxItemY + MAZE_BOX_SIZE);

			g_itemRow = nextItemRow;
			g_itemCol = nextItemCol;

			g_itemScore++;
			InvalidateRect(hWnd, NULL, FALSE);
		}

		/// 도착지 활성화
		if (g_itemScore >= TARGET_SCORE) { g_openDest = TRUE; }
		if (g_openDest && g_destClear == FALSE)
		{
			HBRUSH DestBrush = CreateSolidBrush(RGB(9, 105, 215));  // 도착점
			SelectObject(hdc, DestBrush);

			/// 도착지 설정
			g_destX = rand() % MAZE_ROWS;
			g_destY = rand() % MAZE_COLS;

			while (g_maze[g_destX][g_destY] == WALL)
			{
				g_destX = rand() % MAZE_ROWS;
				g_destY = rand() % MAZE_COLS;
			}
			g_maze[g_destX][g_destY] = DEST;
			int destX = g_destX * MAZE_BOX_SIZE;
			int destY = g_destY * MAZE_BOX_SIZE;
			Rectangle(hdc, destX, destY, destX + MAZE_BOX_SIZE, destY + MAZE_BOX_SIZE);
			g_destClear = TRUE;
			DeleteObject(DestBrush);
		}

		if (isDest)
		{
			g_timerState = CLEAR;
			g_isGame = CLEAR;
			WCHAR buffer[256];
			wsprintfW(buffer, L" %d개를 획득하였습니다!", g_itemScore);
			MessageBox(hWnd, buffer, L"게임 클리어!", MB_OK);
			g_isGame = CLEAR;
			GameText();
		}

		SelectObject(hdc, RoadBrush);
		DeleteObject(CharBrush);
		DeleteObject(RoadBrush);
		DeleteObject(ItemBrush);
	}

	ReleaseDC(hWnd, hdc);
	return 0;
}

void GameText()
{
	if (g_isGame == START) { wsprintfW(g_isGameText, L"현재 게임 상태 : 실행중"); }
	else if (g_isGame == STOP) { wsprintfW(g_isGameText, L"현재 게임 상태 : 정지중"); }
	else if (g_isGame == CLEAR) { wsprintfW(g_isGameText, L"현재 게임 상태 : 클리어!"); }
	else if (g_isGame == TIMEOUT) { wsprintfW(g_isGameText, L"현재 게임 상태 : 타임아웃!"); }
}
void GetGameText(HDC hdc)
{
	// 1. 글자가 출력될 위치 계산 (TextOut 좌표와 동일하게)
	int textX = MAZE_ROWS * MAZE_BOX_SIZE + 30;
	int textY = 30;

	// 2. 지울 영역(사각형) 설정 (글자 길이만큼 넉넉하게 잡음)
	RECT textRect = { textX, textY, textX + 300, textY + 100 };

	// 3. 검은색 브러시로 해당 영역을 칠해서 '지우개' 역할 수행
	FillRect(hdc, &textRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);

	/// 타이머 메세지 출력
	WCHAR timeText[50];
	wsprintf(timeText, L"Time : %d sec", g_playTime);
	TextOut(hdc, textX, textY, timeText, wcslen(timeText));

	/// 점수 메세지 출력
	WCHAR scoreText[30];
	wsprintfW(scoreText, L"획득한 아이템 : %d", g_itemScore);
	TextOut(hdc, textX, textY + 30, scoreText, wcslen(scoreText));

	/// 게임 상태 메세지 출력 
	TextOut(hdc, textX, textY + 60, g_isGameText, wcslen(g_isGameText));
}
void ResetGame(HWND hWnd)
{
	// 1. 타이머 잠시 정지
	g_timerState = STOP;

	// 2. 변수 초기화
	g_itemScore = 0;
	g_playTime = 0;
	g_isGame = START;

	// 도착지 관련 변수도 초기화 (필요하다면)
	g_isDest = FALSE;
	g_destClear = FALSE;
	g_openDest = FALSE;

	// 3. 맵 재생성 (새로운 랜덤 맵)
	SetMap();

	// 4. 플레이어 위치 초기화 (SetMap에서 (1,1)에 CHAR를 두므로 거기에 맞춤)
	g_me_x = 1;
	g_me_y = 1;

	// RECT 좌표 재계산
	g_me.left = g_me_x * MAZE_BOX_SIZE;
	g_me.top = g_me_y * MAZE_BOX_SIZE;
	g_me.right = g_me.left + MAZE_BOX_SIZE;
	g_me.bottom = g_me.top + MAZE_BOX_SIZE;

	// 5. 타이머 다시 시작
	g_timerState = START;

	// 6. 화면 갱신 (배경까지 싹 지우고 다시 그림)
	InvalidateRect(hWnd, NULL, TRUE);
}

DWORD WINAPI TimerProc(LPVOID lpParam)
{
	HWND hWnd = (HWND)lpParam; // 메인 윈도우 핸들 받아오기

	while (true)
	{
		// 1. 프로그램이 종료되면 쓰레드도 탈출 (안전장치)
		// (별도의 종료 플래그를 사용해도 됩니다)
		if (!IsWindow(hWnd)) break;

		// 2. 상태에 따른 동작
		if (g_timerState == START)
		{
			Sleep(1000); // 1초 대기
			g_playTime++; // 시간 증가
			InvalidateRect(hWnd, NULL, FALSE);
		}
		else
		{
			InvalidateRect(hWnd, NULL, FALSE);
			Sleep(100);
		}

		if (g_playTime >= TARGET_TIME && g_timerState != TIMEOUT)
		{
			g_timerState = TIMEOUT;
			g_isGame = TIMEOUT;
			GameText();
			MessageBox(g_hWnd, L"시간 종료!", L"게임을 종료합니다", MB_OK);
		}

	}
	return 0;
}

void GameHelp(HDC hdc)
{
	if (g_helpButton) {
		RECT hr{ 100, 100, 1200, 500 };
		HBRUSH helpBg = CreateSolidBrush(RGB(0, 0, 0));
		FillRect(hdc, &hr, helpBg);
		DeleteObject(helpBg);

		SetTextColor(hdc, RGB(255, 255, 255));

		int x = 150;
		int y = 150;

		const wchar_t* h1 = L"[도움말]";
		const wchar_t* h2 = L"방향키 : 뱀 이동";
		const wchar_t* h3 = L"SPACE  : 게임 시작";
		const wchar_t* h4 = L"P      : 일시정지 / 해제";
		const wchar_t* h5 = L"R      : 즉시 재시작";
		const wchar_t* h6 = L"H      : 이 도움말 열기/닫기";
		const wchar_t* h7 = L"ESC    : 게임 종료";

		TextOutW(hdc, x, y, h1, lstrlenW(h1)); y += 24;
		TextOutW(hdc, x, y, h2, lstrlenW(h2)); y += 20;
		TextOutW(hdc, x, y, h3, lstrlenW(h3)); y += 20;
		TextOutW(hdc, x, y, h4, lstrlenW(h4)); y += 20;
		TextOutW(hdc, x, y, h5, lstrlenW(h5)); y += 20;
		TextOutW(hdc, x, y, h6, lstrlenW(h6)); y += 20;
		TextOutW(hdc, x, y, h7, lstrlenW(h7));
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	g_hWnd = hWnd;
	switch (message)
	{
	case WM_CREATE:
	{
		SetMap();
		g_isGame = STOP;
		g_hTimerThread = STOP;
		wsprintfW(g_isGameText, L"현재 게임 상태 : 대기중");
		g_hTimerThread = CreateThread(NULL, 0, TimerProc, hWnd, 0, NULL);
		g_hStartButton = CreateWindow(L"BUTTON", L"게임 시작", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, MAZE_ROWS * MAZE_BOX_SIZE + 30, 120, 100, 30, hWnd, (HMENU)BT_GAMESTART, hInst, nullptr);
		g_hResetButton = CreateWindow(L"BUTTON", L"리셋 버튼", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, MAZE_ROWS * MAZE_BOX_SIZE + 30, 160, 100, 30, hWnd, (HMENU)BT_GAMERESET, hInst, nullptr);
		g_hPurseButton = CreateWindow(L"BUTTON", L"정지 버튼", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, MAZE_ROWS * MAZE_BOX_SIZE + 30, 200, 100, 30, hWnd, (HMENU)BT_GAMEPAUSE, hInst, nullptr);
	}
	break;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case '1':
		{
			if (g_timerState == TIMEOUT) break;
			g_isGame = START;
			GameText();
			g_timerState = START;
			MessageBox(g_hWnd, L"다시 플레이가 가능합니다!", L"플레이 상태로 돌입합니다.", MB_OK);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
		case '2':
		{
			g_isGame = STOP;
			GameText();
			g_timerState = STOP;
			MessageBox(g_hWnd, L"이어서 하려면 1를 입력해주세요", L"정지 상태로 돌입합니다.", MB_OK);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
		case '8':
		{
			g_isGame = CLEAR;
			GameText();
			g_playTime = 0; // 시간 0으로 리셋
			g_timerState = CLEAR;
			MessageBox(g_hWnd, L"게임을 클리어하였습니다!", L"클리어 상태로 돌입합니다.", MB_OK);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
		/*
		case 'H':
		case 'h':
		{
			g_helpButton = !g_helpButton;

			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
		*/
		// [수정] 지역 변수(DS ds) 대신 동적 할당(new DS) 사용
		PDS pData = new DS;
		pData->m_hWnd = hWnd;
		pData->m_lParam = lParam;
		pData->m_wParam = wParam;

		// &ds 대신 pData 자체를 넘김
		if (g_isGame == START) g_hThread = CreateThread(NULL, 0, MoveChar, pData, 0, NULL);

		// 핸들은 닫아주는 것이 좋습니다 (쓰레드 종료와 무관)
		if (g_hThread) CloseHandle(g_hThread);

		}
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case BT_GAMESTART:
			g_isGame = START;
			g_timerState = START;
			SetFocus(hWnd);
			GameText();
			InvalidateRect(hWnd, NULL, FALSE);
			/// MessageBox(hWnd, L"클릭 되었습니다!", L"button", NULL);
			break;
		case BT_GAMERESET:
			if (MessageBox(hWnd, L"게임을 리셋하시겠습니까?", L"알림", MB_OKCANCEL) == IDOK)
			{
				ResetGame(hWnd); // 초기화 함수 호출
				GameText();      // 텍스트 업데이트
				g_isGame = STOP;
				g_timerState = STOP;
			}
			SetFocus(hWnd);
			GameText();
			break;
		case BT_GAMEPAUSE:
			g_isGame = STOP;
			g_timerState = STOP;
			SetFocus(hWnd);
			GameText();
			InvalidateRect(hWnd, NULL, FALSE);
			/// MessageBox(hWnd, L"클릭 되었습니다!", L"button", NULL);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		/// 현재 생긴 이슈
		/// 캐릭터가 이동했을때 잔상이 생김
		/// 
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

		GetMap(hdc);
		GetGameText(hdc);
		GameHelp(hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:

		if (g_hTimerThread)
		{
			CloseHandle(g_hTimerThread);
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
	}

	// 정보 대화 상자의 메시지 처리기입니다.
	INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		UNREFERENCED_PARAMETER(lParam);
		switch (message)
		{
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
		return (INT_PTR)FALSE;
	}
