#pragma once
#include "Resource.h"
#include "windows.h"

class Object
{
private:
	RECT player;
	int playerSize;
	int player_row, player_col;

	RECT place;

	BOOL destInit;
	BOOL destOpen;

	/// 아이템의 좌표 저장
	int newItemRow;
	int newItemCol;
public:
	/// 도착지 관련 명령어
	Object();
	void DestInit(int itemCount);
	void DestIn(BOOL init);

	/// 아이템 생성
	void SetItem();
	void GetItem(HDC hdc);

	/// 아이템 생성 비교 함수

	void SetPlayer(WPARAM wParam, HWND hWnd);

	void DrawPlayer(HDC playerDc);
};

