// mazeGame_project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "mazeGame_project.h"

/// 추가된 헤더파일
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
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
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

/// g_me = 캐릭터, g_maze_TF = 벽인지 아닌지 여부 확인
RECT g_me;
BOOL g_maze_TF[MAZE_ROWS][MAZE_COLS];

/// 아이템 선언 및 아이템 위치 생성
RECT g_item;
int g_item_rows, g_item_cols;

/// x와 y좌표 전역변수화
int g_me_x, g_me_y;

/// 아이템 획득 시 점수 획득 변수 생성
int g_item_score;

/// 도착지점 rect 변수 생성
RECT g_dest;

/// 도착지점 활성화 여부
BOOL g_dest_set;
BOOL g_dest_setClear;



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

Map g_maze_area;
int maze_map[MAZE_ROWS][MAZE_COLS];
Object g_object;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		/// 테스트
		g_maze_area.SetMap();
		g_object.SetItem();
		/// 획득 점수 초기화
		g_item_score = 0;

		/// 랜덤한 위치에 생성할 예정이기 때문에 0으로 초기화
		g_dest = { 0, };

		/// 도착지점 생성 여부 확인 변수 초기화
		g_dest_set = FALSE;
		g_dest_setClear = FALSE;
	}
	break;
	case WM_KEYDOWN:
	{
		g_object.SetPlayer(wParam,hWnd);
	}
	/// 구현 로직
	/// 이동할 좌표의 값을 임시변수 nx, ny에 할당
	/// 키보드의 값을 입력 받을 경우, 이동할 칸에 할당된 변수의 값(0,1)을 비교
	/// 길이라면 이동 후, 임시변수의 값을 변경.
	/// 로직 제작 시 오래 걸린 이유
	/// 칸에 할당된 변수와 캐릭터의 값을 비교하는 로직을 작성하지 못함
	/// x,y
	/// 좌표 이해를 잘 하지 못했음.
	

		/// 내 캐릭터와 아이템이 겹쳤는지 확인
		/// 해결 못하고 그냥 렉트의 좌표값을 비교
		/*
		if (nx == g_item_rows && ny == g_item_cols)
		{
			g_item_score++;

			/// 도착하는 지점의 좌표를 랜덤으로 만들기 위함.
			int destX = rand() % MAZE_ROWS;
			int destY = rand() % MAZE_COLS;
			if (g_item_score >= TARGET_SCORE)
			{
				g_dest_set = TRUE;
			}
			if (g_dest_set && g_dest_setClear == FALSE)				/// 도착지점 활성화 된 경우 setClear로 생성이 불가능하게 막아둠.  -> 크리에이트에서 생성해두고 조건 달성 시 오픈. 이미 무작위이기 때문에 플레이어는 무작위로 생성되었다고 느낄듯
			{
				while (g_maze[destX][destY] == 1)
				{
					destX = rand() % MAZE_ROWS;
					destY = rand() % MAZE_COLS;
				}
				g_maze[destX][destY] = 5;
				g_dest_setClear = TRUE;
			}
			/// 아이템 위치를 만들기 위해 좌표값 저장
			int newItemX = rand() % MAZE_ROWS;
			int newItemY = rand() % MAZE_COLS;

			/// [ ! ] 원하는 대로 작동은 하나 현재 이동할때마다 InvaldateRect가 사용되서 모든 공간에서 생성함.
			/// 반복문 이전에 아이템의 위치를 생성하였음.
			while (g_maze[newItemX][newItemY] == 0)
			{
				newItemX = rand() % MAZE_ROWS;
				newItemY = rand() % MAZE_COLS;
			}
			/// 아이템 전역 인덱스를 새 위치로 업데이트
			g_item_rows = newItemX;
			g_item_cols = newItemY;

			/// 생성이 완료됨을 확인하기 위해 1-> 4로 변경
			/// [ ! ] 해결 X, 현재 문제, 렉트가 겹쳐져도 현상 유지. 해결해야함.

			g_maze[g_me_x][g_me_y] = 1; // 플레이어가 서 있는 곳을 길로 되돌리고
			g_maze[g_item_rows][g_item_cols] = 4;

		}
		/// WM_PAINT를 호출
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
		*/
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
		/// 미로 선언

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

		g_maze_area.GetMap(hdc);
		g_object.DrawPlayer(hdc);
		g_object.GetItem(hdc);
		/// 아이템을 먹을 시 점수 증가 및 출력 텍스트	
		WCHAR scoreText[30];
		wsprintfW(scoreText, L"획득한 점수 : %d", g_item_score);
		TextOut(hdc, 750, 30, scoreText, wcslen(scoreText));

		/// 만든 오브젝트 삭제
		EndPaint(hWnd, &ps);
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
