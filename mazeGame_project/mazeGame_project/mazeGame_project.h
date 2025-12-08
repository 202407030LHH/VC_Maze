#pragma once

#include "resource.h"

/// g_me = 캐릭터, g_maze_TF = 벽인지 아닌지 여부 확인
RECT g_me;

/// x와 y좌표 전역변수화

int g_me_x, g_me_y;


/// 아이템의 좌표
int g_itemRow, g_itemCol;
int g_newItemRow, g_newItemCol;
int g_itemScore;


int g_maze[MAZE_ROWS][MAZE_COLS];

int g_destX, g_destY;
BOOL g_isDest;
BOOL g_destClear;
BOOL g_openDest;

HWND g_hWnd;
BOOL g_maze_clear;

BOOL g_helpButton;

int g_isGame;

WCHAR g_isGameText[100];


HANDLE g_hThread;
// 전역 변수
volatile int g_timerState = STOP; // 현재 타이머 상태 (volatile 필수)
volatile int g_playTime = 0;            // 게임 시간 (초 단위)

HANDLE g_hTimerThread;           // 타이머 쓰레드 핸들
HANDLE g_hStartButton;
HANDLE g_hResetButton;
HANDLE g_hPurseButton;

BOOL g_helpButton;

HWND g_hWnd;
