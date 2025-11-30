#pragma once
#include "Windows.h"
#include "Object.h"
#include <time.h>

extern int maze_map[MAZE_ROWS][MAZE_COLS];
/// 게임 정보
/// 0 : 벽/검정색 , 1 : 길/흰색, 3 : 출발, 4 : 아이템, 5 : 도착지점

class Map
{
private:

	/// 맵의 크기를 받아오는 변수 설정
	int width;
	int height;

	/// 도작지에 대한 정보 저장
	int destX, destY;



public:
	/// 초기화 및 맵 생성을 위한 생성자 추가
	Map();
	void SetMap();
	void GetMap(HDC Whdc);
};

