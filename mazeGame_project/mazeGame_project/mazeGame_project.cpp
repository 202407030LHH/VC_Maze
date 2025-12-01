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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
#define MAZE_ROWS 31
#define MAZE_COLS 25
#define MAZE_BOX_SIZE 30

#define WALL 0
#define ROAD 1
#define CHAR 2
#define ITEM 3
#define DEST 4
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

int width, height;
// 우선 순위 과제
// 벽에 닿았을 때 움직이지 않게 하기
// 현재 생각중인 방법 
// 1. g_me의 좌표를 저장해두고 키보드 입력 시 길이 아닌 경우 break; 함수로 구현하기 쉬울 것 같음
// 과제 해결

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
void SetMap()
{
	srand(time(NULL));
	width = MAZE_ROWS;
	height = MAZE_COLS;

	int destX = rand() % MAZE_ROWS;
	int destY = rand() % MAZE_COLS;

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

	/// 모든 구역을 길로 초기화
	for (int i = 0; i < width ;i++)
	{
		for (int j = 0; j < height; j++)
		{
			g_maze[i][j] = ROAD;
		}
	}

	/// 상단, 오른쪽 벽면 테두리 만드는 코드
	for (int i = 0; i < width; i++)
	{
		/// height -1을 해야만 맨 마지막 칸을 WALL 선언할 수 있음.
		g_maze[i][0] = WALL;
		g_maze[i][height - 1] = WALL;
	}

	/// 하단, 왼쪽 벽면 테두리 만드는 코드
	for (int i = 0; i < height; i++)
	{
		/// width -1을 해야만 맨 마지막 칸을 WALL 선언할 수 있음.
		g_maze[0][i] = WALL;
		g_maze[width - 1][i] = WALL;
	}

	/// 랜덤한 위치의 벽을 만드는 코드
	for (int i = 2; i < width - 2; i += 2)
	{
		for (int j = 2; j < height - 2; j += 2)
		{
			g_maze[i][j] = WALL; // 격자 지점에 벽 생성

			// 상하좌우 중 랜덤하게 한 방향으로 벽 연장
			int direction = rand() % 4;
			switch (direction)
			{
			case 0: if (j > 2) g_maze[i][j - 1] = WALL; break;				// 상
			case 1: if (j < height - 3) g_maze[i][j + 1] = WALL; break;		// 하
			case 2: if (i > 2) g_maze[i - 1][j] = WALL; break;				// 좌
			case 3: if (i < width - 3) g_maze[i + 1][j] = WALL; break;		// 우
			}
		}
	}
	/// 출발지는 고정
	g_maze[1][1] = CHAR;

	/// 도착지는 랜덤
	while (g_maze[destX][destY] == WALL)
	{
		destX = rand() % MAZE_ROWS;
		destY = rand() % MAZE_COLS;
	}

}

void GetMap(HDC Whdc)
{
	// 1. GDI 객체 생성 (한 번만)
	HBRUSH WallBrush = CreateSolidBrush(RGB(180,180,180));      // 검정색 (벽)
	HBRUSH RordBrush = CreateSolidBrush(RGB(40,40,40)); // 흰색 (길)
	HBRUSH DestBrush = CreateSolidBrush(RGB(9, 105, 215));  // 도착점
	HBRUSH ItemBrush = CreateSolidBrush(RGB(255, 215, 0));   // 아이템: 금색
	HBRUSH CharBrush = CreateSolidBrush(RGB(0, 255, 0));     // 캐릭터: 형광 연두색

	// 펜을 NULL_PEN으로 설정하고 나중에 원래 펜 복원 (외곽선 제거)
	HPEN NullPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(Whdc, NullPen);

	HBRUSH CurrentBrush;
	HBRUSH hOldBrush = (HBRUSH)SelectObject(Whdc, RordBrush); // 흰색으로 초기 브러시 설정

	for (int row = 0; row < MAZE_COLS; row++) // 세로 줄 수
	{
		for (int col = 0; col < MAZE_ROWS; col++) // 가로 칸 수
		{
			int x = col * MAZE_BOX_SIZE;
			int y = row * MAZE_BOX_SIZE;

			// [수정] g_maze[행][열] 접근
			if (g_maze[row][col] == WALL) CurrentBrush = WallBrush;
			else if (g_maze[row][col] == DEST) CurrentBrush = DestBrush;
			else if (g_maze[row][col] == CHAR) CurrentBrush = CharBrush;
			else if (g_maze[row][col] == ITEM) CurrentBrush = ItemBrush;
			else CurrentBrush = RordBrush;

			SelectObject(Whdc, CurrentBrush);
			Rectangle(Whdc, x, y, x + MAZE_BOX_SIZE, y + MAZE_BOX_SIZE);
		}
	}
	// 원래 펜과 브러쉬로 복구
	SelectObject(Whdc, hOldBrush);
	SelectObject(Whdc, hOldPen);

	// 만든 오브젝트 삭제
	DeleteObject(WallBrush);
	DeleteObject(RordBrush);
	DeleteObject(DestBrush);
	DeleteObject(ItemBrush);
	DeleteObject(CharBrush);
	DeleteObject(NullPen);
}

void SetItem()
{
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		SetMap();
		/// 아이템의 위치 찾기
		for (int i = 0; i < MAZE_ROWS; i++)
		{
			for (int j = 0; j < MAZE_COLS; j++)
			{
				if (g_maze[i][j] == CHAR)
				{
					g_me_x = i;
					g_me_y = j;

					g_me.left = g_me_x * MAZE_BOX_SIZE;
					g_me.top = g_me_y * MAZE_BOX_SIZE;
					g_me.right = g_me.left + MAZE_BOX_SIZE;
					g_me.bottom = g_me.top + MAZE_BOX_SIZE;
				}
			}
		}
	}
	break;

	/// 구현 로직
	/// 이동할 좌표의 값을 임시변수 nx, ny에 할당
	/// 키보드의 값을 입력 받을 경우, 이동할 칸에 할당된 변수의 값(0,1)을 비교
	/// 길이라면 이동 후, 임시변수의 값을 변경.
	/// 로직 제작 시 오래 걸린 이유
	/// 칸에 할당된 변수와 캐릭터의 값을 비교하는 로직을 작성하지 못함
	/// x,y 좌표 이해를 잘 하지 못했음.


	case WM_KEYDOWN:
	{
		// 1. 임시 변수에 현재 위치 저장
		int nx = g_me_x; // 행 (Row)
		int ny = g_me_y; // 열 (Col)

		// 2. 키 입력에 따른 '예상' 위치 계산
		if (wParam == VK_LEFT)
		{
			ny--;
		}
		else if (wParam == VK_RIGHT)
		{
			ny++;
		}
		else if (wParam == VK_UP)
		{
			nx--;
		}
		else if (wParam == VK_DOWN)
		{
			nx++;
		}

		// 3. 통합 경계 및 충돌 검사
		// (범위를 벗어나지 않고, 벽이 아닌 경우에만 이동 수행)
		if (nx >= 0 && nx < MAZE_ROWS && ny >= 0 && ny < MAZE_COLS && g_maze[nx][ny] != WALL)
		{
			bool isItem = (g_maze[nx][ny] == ITEM);

			// 4. 이동이 확정되었을 때만 데이터 갱신
			g_maze[g_me_x][g_me_y] = ROAD; // 기존 위치 지우기

			// 좌표 및 인덱스 업데이트
			g_me_x = nx;
			g_me_y = ny;

			// 픽셀 좌표 재계산 (이게 더 안전함)
			g_me.left = g_me_y * MAZE_BOX_SIZE;
			g_me.top = g_me_x * MAZE_BOX_SIZE;
			g_me.right = g_me.left + MAZE_BOX_SIZE;
			g_me.bottom = g_me.top + MAZE_BOX_SIZE;

			g_maze[g_me_x][g_me_y] = CHAR; // 새 위치에 캐릭터 표시

			if (isItem)
			{
				int nextItemRow;
				int nextItemCol;
				
				do
				{
					nextItemRow = rand() % MAZE_ROWS;
					nextItemCol = rand() % MAZE_COLS;
				} while (g_maze[nextItemRow][nextItemCol] != ROAD);

				g_maze[nextItemRow][nextItemCol] = ITEM;

				g_itemRow = nextItemRow;
				g_itemCol = nextItemCol;

				g_itemScore++;
			}
				// 화면 갱신 요청
				InvalidateRect(hWnd, NULL, TRUE);
		}
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
		HDC hdc = BeginPaint(hWnd, &ps); // [중요] GetDC 대신 BeginPaint 사용

		// 맵 그리기 (GetMap 함수 내부 로직이 올바르다면 호출만으로 충분)
		GetMap(hdc);

		// 텍스트 출력
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		WCHAR scoreText[30];
		wsprintfW(scoreText, L"Get Item : %d", g_itemScore);
		TextOut(hdc, 750, 30, scoreText, wcslen(scoreText));

		EndPaint(hWnd, &ps); // [중요] EndPaint 필수
	}
	break;
	case WM_DESTROY:
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
