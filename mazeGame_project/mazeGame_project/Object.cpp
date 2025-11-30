#include "Object.h"
#include <time.h>
extern int maze_map[MAZE_ROWS][MAZE_COLS];

Object::Object()
{
	srand(time(NULL));
	destInit = FALSE;

	newItemRow = rand() % MAZE_ROWS;
	newItemCol = rand() % MAZE_COLS;

	player_row = 1;
	player_col = 1;
	
	player.left = player_col * MAZE_BOX_SIZE;
	player.top = player_row * MAZE_BOX_SIZE;
	player.right = player.left + MAZE_BOX_SIZE;
	player.bottom = player.top + MAZE_BOX_SIZE;
}

void Object::DestInit(int itemCount)
{
	if (itemCount >= TARGET_SCORE)
	{
		destInit = TRUE;
	}

	if (destInit)
	{
		DestIn(destInit);
	}
}

void Object::DestIn(BOOL init)
{
	if (init)
	{
		/// 여기에 도착지점 생성하는 로직을 넣어서 그것을 찾게 그려야함.

	}
}

void Object::DrawPlayer(HDC playerDc)
{
	HBRUSH myBrush = CreateSolidBrush(RGB(255, 255, 0));
	HBRUSH osBrush = (HBRUSH)SelectObject(playerDc, myBrush);
	Ellipse(playerDc, player.left, player.top, player.right, player.bottom);
	SelectObject(playerDc, osBrush);
	DeleteObject(myBrush);
}

void Object::SetPlayer(WPARAM wParam, HWND hWnd)
{
	int current_row = player_row;
	int current_col = player_col;
	
	HDC hdc = GetDC(hWnd);

	switch (wParam)
	{
	case VK_LEFT:   // 왼쪽: col 감소
	{
		if (maze_map[current_row][current_col - 1] == WALL) break;
		else
		{
			player.left -= MAZE_BOX_SIZE;
			player.right -= MAZE_BOX_SIZE;
			player_col--;
		}
	}
	break;
	case VK_RIGHT:  // 오른쪽: col 증가
	{
		if (maze_map[current_row][current_col + 1] == WALL) break;
		else
		{
			player.left += MAZE_BOX_SIZE;
			player.right += MAZE_BOX_SIZE;
			player_col++;
		}
	}
	break;
	case VK_UP:     // 위: row 감소
	{
		if (maze_map[current_row - 1][current_col] == WALL) break;
		else
		{
			player.top -= MAZE_BOX_SIZE;
			player.bottom -= MAZE_BOX_SIZE;
			player_row--;
		}
	}
	break;
	case VK_DOWN:   // 아래: row 증가
	{
		if (maze_map[current_row + 1][current_col] == WALL) break;
		else
		{
			player.top += MAZE_BOX_SIZE;
			player.bottom += MAZE_BOX_SIZE;
			player_row++;
		}
	}
	break;
	}
	
	if (player_row == newItemRow && player_col == newItemCol)
	{
		SetItem();
		GetItem(hdc);
	}
	InvalidateRect(hWnd, NULL, TRUE);
}

void Object::SetItem()
{
	maze_map[newItemRow][newItemCol] = ROAD;
	
	int nextItemRow = newItemRow;
	int nextItemCol = newItemCol;
	while (maze_map[nextItemRow][nextItemCol] == WALL)
	{
		nextItemRow = rand() % MAZE_ROWS;
		nextItemCol = rand() % MAZE_COLS;
	}
	
	maze_map[nextItemRow][nextItemCol] = ITEM;

	newItemRow = nextItemRow;
	newItemCol = nextItemCol;
}

void Object::GetItem(HDC hdc)
{
	HBRUSH ItemBrush = CreateSolidBrush(RGB(105, 115, 0));   // 아이템 색상
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, ItemBrush);

	for (int row = 0; row < MAZE_ROWS; row++)
	{
		for (int col = 0; col < MAZE_COLS; col++)
		{
			int x = col * MAZE_BOX_SIZE;
			int y = row * MAZE_BOX_SIZE;
			
			if (maze_map[row][col] == ITEM)
			{
				Rectangle(hdc, x, y, x + MAZE_BOX_SIZE, y + MAZE_BOX_SIZE);
				break;
			}
		}
	}
	SelectObject(hdc, hOldBrush);
	DeleteObject(ItemBrush);
}
