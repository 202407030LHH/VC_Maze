#include "Map.h"

Map::Map()
{
	/// 변수 초기화
	width = MAZE_ROWS;
	height = MAZE_COLS;

	destX = rand() % MAZE_ROWS;
	destY = rand() % MAZE_COLS;

	itemX = rand() % MAZE_ROWS;
	itemY = rand() % MAZE_COLS;

	/// 맵 초기화
	for (int i = 0; i < MAZE_ROWS; ++i) {
		for (int j = 0; j < MAZE_COLS; ++j) {
			maze_map[i][j] = ROAD;
		}
	}
}

void Map::SetMap()
{
	srand(time(NULL));

	/// 상단, 오른쪽 벽면 테두리 만드는 코드
	for (int i = 0; i < width; i++)
	{
		/// height -1을 해야만 맨 마지막 칸을 WALL 선언할 수 있음.
		maze_map[i][0] = WALL;
		maze_map[i][height-1] = WALL;
	}

	/// 하단, 왼쪽 벽면 테두리 만드는 코드
	for (int i = 0; i < height; i++)
	{
		/// width -1을 해야만 맨 마지막 칸을 WALL 선언할 수 있음.
		maze_map[0][i] = WALL;
		maze_map[width-1][i] = WALL;
	}

	/// 랜덤한 위치의 벽을 만드는 코드
	for (int i = 2; i < width - 2; i += 2)
	{
		for (int j = 2; j < height - 2; j += 2)
		{
			maze_map[i][j] = WALL; // 격자 지점에 벽 생성

			// 상하좌우 중 랜덤하게 한 방향으로 벽 연장
			int direction = rand() % 4;
			switch (direction)
			{
			case 0: if (j > 2) maze_map[i][j - 1] = WALL; break;				// 상
			case 1: if (j < height - 3) maze_map[i][j + 1] = WALL; break;		// 하
			case 2: if (i > 2) maze_map[i - 1][j] = WALL; break;				// 좌
			case 3: if (i < width - 3) maze_map[i + 1][j] = WALL; break;		// 우
			}
		}
	}

	/// 출발지는 고정
	maze_map[1][1] = 3;

	/// 도착지는 랜덤
	while (maze_map[destX][destY] == 1)
	{
		destX = rand() % MAZE_ROWS;
		destY = rand() % MAZE_COLS;
	}

	/// 아이템 위치는 랜덤
	while (maze_map[itemX][itemY] == 1)
	{
		itemX = rand() % MAZE_ROWS;
		itemY = rand() % MAZE_COLS;
	}
}
void Map::GetMap(HDC Whdc)
{
	// 1. GDI 객체 생성 (한 번만)
	HBRUSH WallBrush = CreateSolidBrush(RGB(0, 0, 0));      // 검정색 (벽)
	HBRUSH RordBrush = CreateSolidBrush(RGB(255, 255, 255)); // 흰색 (길)
	HBRUSH ItemBrush = CreateSolidBrush(RGB(105, 5, 35));   // 아이템
	HBRUSH DestBrush = CreateSolidBrush(RGB(9, 105, 215));  // 도착점

	// 펜을 NULL_PEN으로 설정하고 이전 펜 저장 (복구용)
	HPEN NullPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(Whdc, NullPen);

	HBRUSH CurrentBrush;
	HBRUSH hOldBrush = (HBRUSH)SelectObject(Whdc, RordBrush); // 흰색으로 초기 브러시 설정

	for (int i = 0; i < width; i++) // 행 (Y축)
	{
		for (int j = 0; j < height; j++) // 열 (X축)
		{
			// 좌표 계산
			int x = j * MAZE_BOX_SIZE;
			int y = i * MAZE_BOX_SIZE;

			// 브러쉬 선택
			if (maze_map[i][j] == WALL) 
			{
				CurrentBrush = WallBrush;
			}
			else if (maze_map[i][j] == ITEM) 
			{
				CurrentBrush = ItemBrush;
			}
			else if (maze_map[i][j] == DEST)
			{
				CurrentBrush = DestBrush;
			}
			else 
			{
				CurrentBrush = RordBrush;
			}

			// 선택된 브러쉬로 그리기
			SelectObject(Whdc, CurrentBrush);
			Rectangle(Whdc, x, y, x + MAZE_BOX_SIZE, y + MAZE_BOX_SIZE);
		}
	}

	// 원래 펜과 브러쉬로 복구
	SelectObject(Whdc, hOldPen);
	SelectObject(Whdc, hOldBrush);

	// 만든 오브젝트 삭제
	DeleteObject(WallBrush);
	DeleteObject(RordBrush);
	DeleteObject(ItemBrush);
	DeleteObject(DestBrush);
	DeleteObject(NullPen);
}