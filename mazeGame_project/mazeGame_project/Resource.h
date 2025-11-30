//{{NO_DEPENDENCIES}}
// Microsoft Visual C++에서 생성한 포함 파일입니다.
// mazeGame_project.rc에서 사용되고 있습니다.
//
#define IDC_MYICON                      2
#define IDD_MAZEGAMEPROJECT_DIALOG      102
#define IDS_APP_TITLE                   103
#define IDD_ABOUTBOX                    103
#define IDM_ABOUT                       104
#define IDM_EXIT                        105
#define IDI_MAZEGAMEPROJECT             107
#define IDI_SMALL                       108
#define IDC_MAZEGAMEPROJECT             109
#define IDR_MAINFRAME                   128
/// 메뉴바 rc 파일 추가
#define IDM_GAMESTART                   32771
#define IDM_GAMEPAUSE                   32772
#define IDM_GAMESTOP                    32773
#define IDC_STATIC                      -1

/// 맵 생성을 위한 매크로 상수
#define MAZE_ROWS 28
#define MAZE_COLS 23
#define MAZE_BOX_SIZE 30


/// 게임 진행 시 스코어의 값을 변경하면 됨
#define TARGET_SCORE 1

/// 길이나 벽 표현을 위한 상수 정의
#define WALL  0   // 벽 (검정)
#define ROAD  1   // 길 (흰색)
#define START  3  // 시작점
#define ITEM  4   // 아이템
#define DEST  5   // 도착점

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NO_MFC                     1
#define _APS_NEXT_RESOURCE_VALUE        129
#define _APS_NEXT_COMMAND_VALUE         32774
#define _APS_NEXT_CONTROL_VALUE         1000
#define _APS_NEXT_SYMED_VALUE           110
#endif
#endif
