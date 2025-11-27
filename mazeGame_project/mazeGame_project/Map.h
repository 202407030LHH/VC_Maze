#pragma once
#include "Windows.h"
#include <time.h>

/// 맵 생성을 위한 매크로 상수
#define MAZE_ROWS 20
#define MAZE_COLS 20
#define MAZE_BOX_SIZE 30

/// 게임 정보
/// 0 : 벽/검정색 , 1 : 길/흰색, 3 : 출발, 4 : 아이템, 5 : 도착지점

class Map
{
private:
	int maze_map[MAZE_ROWS][MAZE_COLS];
	
	/// 길이나 벽 표현을 위한 상수 정의
	const int WALL = 0;   // 벽 (검정)
	const int ROAD = 1;   // 길 (흰색)
	const int START = 3;  // 시작점
	const int ITEM = 4;   // 아이템
	const int DEST = 5;   // 도착점

	/// 맵의 크기를 받아오는 변수 설정
	int width;
	int height;

	/// 도작지에 대한 정보 저장
	int destX, destY;

	/// 아이템에 대한 정보 저장
	int itemX, itemY;
public:
	/// 초기화 및 맵 생성을 위한 생성자 추가
	Map();
	void SetMap();
	void GetMap(HDC Whdc);
};

