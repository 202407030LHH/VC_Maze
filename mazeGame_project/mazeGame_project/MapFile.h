#pragma once
#include "framework.h"
#include "Resource.h"

class MapFile
{
private:
	static int maze1[MAZE_ROWS][MAZE_COLS];
	static int maze2[MAZE_ROWS][MAZE_COLS];
	static int maze3[MAZE_ROWS][MAZE_COLS];

public:
	void MazeSelect(int level, HDC hMazeMap);
};

