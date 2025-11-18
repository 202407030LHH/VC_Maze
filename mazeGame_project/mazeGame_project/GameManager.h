#pragma once
#include "framework.h"

/// 이동 예정
#define MAZE_ROWS 20
#define MAZE_COLS 20
#define MAZE_BOX_SIZE 30

class GameManager
{
	/// 플레이어 생성
private:
	RECT playerIcon;
	RECT scoreItem;
	int playerRows, playerCols;
	/// 아이콘 크기 설정
	int iconsize;

public:
	GameManager();
	void MoveLeft();
};

