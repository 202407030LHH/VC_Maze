// mazeGame_project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "mazeGame_project.h"

#include "time.h"

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

/// 맵 크기 키워서 테스트
int g_maze[MAZE_ROWS][MAZE_COLS];

int g_destX, g_destY;
BOOL g_isDest;
BOOL g_destClear;
BOOL g_openDest;

HWND g_hWnd;
BOOL g_maze_clear;

#define START	 1
#define STOP	 0
#define CLEAR	 2
#define OUT		 3

int g_isGame;

WCHAR g_isGameText[100];

// 타이머 상태 상수 정의


// 전역 변수
volatile int g_timerState = STOP; // 현재 타이머 상태 (volatile 필수)
volatile int g_playTime = 0;            // 게임 시간 (초 단위)

HANDLE g_hTimerThread;           // 타이머 쓰레드 핸들
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

	SelectObject(hdc, hOldBrush);
	SelectObject(hdc, hOldPen);

	// 만든 오브젝트 삭제
	DeleteObject(WallBrush);
	DeleteObject(RoadBrush);
	DeleteObject(ItemBrush);
	DeleteObject(CharBrush);
	DeleteObject(NullPen);
	ReleaseDC(g_hWnd, hdc);

	g_maze_clear = TRUE;
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
	else if (g_isGame == OUT) { wsprintfW(g_isGameText, L"현재 게임 상태 : 타임아웃!"); }
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
		
		if (g_playTime >= 60)
		{
			MessageBox(g_hWnd, L"시간 종료!", L"게임을 종료합니다", MB_OK);
			g_timerState = OUT;
			Sleep(500);
		}

	}
	return 0;
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
	}
	break;
	/// [!]쓰레드 테스트

	/// 구현 로직
	/// 이동할 좌표의 값을 임시변수 nx, ny에 할당
	/// 키보드의 값을 입력 받을 경우, 이동할 칸에 할당된 변수의 값(0,1)을 비교
	/// 길이라면 이동 후, 임시변수의 값을 변경.
	/// 로직 제작 시 오래 걸린 이유
	/// 칸에 할당된 변수와 캐릭터의 값을 비교하는 로직을 작성하지 못함
	/// x,y 좌표 이해를 잘 하지 못했음.

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case '1': g_isGame = START;
		{
			GameText();
			g_timerState = START;
			MessageBox(g_hWnd, L"다시 플레이가 가능합니다!", L"플레이 상태로 돌입합니다.", MB_OK);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
		case '2': g_isGame = STOP;
		{
			GameText();
			g_timerState = STOP;
			MessageBox(g_hWnd, L"이어서 하려면 1를 입력해주세요", L"정지 상태로 돌입합니다.", MB_OK);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
		case '8': g_isGame = CLEAR;
		{
			GameText();
			g_playTime = 0; // 시간 0으로 리셋
			g_timerState = CLEAR;
			MessageBox(g_hWnd, L"게임을 클리어하였습니다!", L"클리어 상태로 돌입합니다.", MB_OK);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
		}
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
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
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
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		if (!g_maze_clear) GetMap(hdc);

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
