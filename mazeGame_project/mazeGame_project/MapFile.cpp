#include "MapFile.h"

int maze1[MAZE_ROWS][MAZE_COLS]
	{
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Row 0 (End)
	{ 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 }, // Row 1
	{ 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 }, // Row 2
	{ 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0 }, // Row 3
	{ 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0 }, // Row 4
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 }, // Row 5
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 }, // Row 6
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0 }, // Row 7
	{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 }, // Row 8
	{ 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0 }, // Row 9
	{ 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0 }, // Row 10
	{ 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0 }, // Row 11
	{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0 }, // Row 12
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0 }, // Row 13
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0 }, // Row 14
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0 }, // Row 15
	{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Row 16
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // Row 17
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, // Row 18
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3 }  // Row 19 (Start)
	};

void MazeSelect(int level, HDC hMazeMap) 
{
	HBRUSH WallBrush = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH RordBrush = CreateSolidBrush(RGB(255, 255, 255));;
	HBRUSH CurrentBrush;

	switch (level)
	{
	case 1:
	{
		for (int i = 0; i < MAZE_ROWS; i++)
		{
			for (int j = 0; j < MAZE_COLS; j++)
			{
				/// 왜 이렇게 되는지는 다시 이해해야함.
				int x = j * MAZE_BOX_SIZE;
				int y = i * MAZE_BOX_SIZE;

				/// 브러쉬 선택 
				if (maze1[i][j] == 0) CurrentBrush = WallBrush;
				else CurrentBrush = RordBrush;

				SelectObject(hMazeMap, CurrentBrush);
				Rectangle(hMazeMap, x, y, x + MAZE_BOX_SIZE, y + MAZE_BOX_SIZE);

				/*if (maze1[i][j] == 3) {
					SelectObject(hMazeMap, RordBrush);
					Ellipse(hMazeMap, g_me.left + x, g_me.top + x, g_me.right + x, g_me.bottom + x);
				}*/
			}
		}
		InvalidateRect((HWND)hMazeMap, NULL, NULL);
	}

	DeleteObject(WallBrush);
	DeleteObject(RordBrush);
	DeleteObject(CurrentBrush);
	break;
	}
}