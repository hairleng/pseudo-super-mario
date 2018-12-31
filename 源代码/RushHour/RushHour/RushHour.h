#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <math.h>
#include "resource.h"
#include <stdio.h>
#include <time.h>
#include "mmsystem.h"//导入声音头文件
#pragma comment(lib,"winmm.lib")//导入声音头文件库

/*定义宏变量，WNDWIDTH、WNDHEIGHT为窗口的宽和高*/
#define TIMER                1
#define TIMER2               2
#define WNDWIDTH             500
#define WNDHEIGHT            360
#define BLOCK_COLOR_NUM      4
#define ROOF_COLOR_NUM       4
#define MAX_TERRIAN_NUM      100
#define TERRIAN_WIDTH        90
BOOL playmusic = FALSE;

/*英雄结构体，英雄位图、位置、大小*/
typedef struct
{
	HBITMAP	hBmp;
	POINT	pos;
	SIZE	size;
	int     curFrameIndex;
	int     maxFrameSize;
	int     left_terrian_index;
	int     right_terrian_index;
}Hero;

/*地形结构体，地形方块位图、屋顶位图、位置、大小、屋顶以及方块大小、方块个数*/
typedef struct
{
	HBITMAP hBlockBmp;
	HBITMAP hRoofBmp;
	HBITMAP hLightBmp;
	HBITMAP hChimneyBmp;
	HBITMAP hStarBmp;
	POINT	pos;
	SIZE    size;
	BOOL    isStar;
	int     roofWidth;
	int     roofHeight;
	int     blockWidth;
	int     blockHeight;
	int     blockNum;
	int     lightWidth;
	int     lightHeight;
	int     chimneyWidth;
	int     chimneyHeight; 
	int     starWidth;
	int     starHeight;
}Terrian;

/*背景建筑结构体，建筑位图、位置、大小、类型*/
typedef struct
{
	HBITMAP	hBmp;
	POINT	pos;
	SIZE	size;
}Building;

/*游戏状态结构体*/
typedef struct
{
	HBITMAP	hBmp;
	POINT   pos;
	SIZE	size;
	BOOL	isPaused;
	BOOL    isRestarted;
	int     life;
	int     totalDist;
	int     flag;
	int     run_flag;
	UINT    timer_id;
	UINT    timer_id2;
	int     state;

}GameStatus;

/*全局变量*/
static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("尖峰时刻");

/*声明位图句柄*/
HBITMAP m_hBackgroundBmp;
HBITMAP m_hStarticonBmp;
HBITMAP m_hDeadpicBmp;
HBITMAP m_hBuildingBmp;
HBITMAP m_hHeroBmp;
HBITMAP m_hGameStatusBmp;
HBITMAP	m_hBlockBmp[BLOCK_COLOR_NUM];
HBITMAP	m_hRoofkBmp[ROOF_COLOR_NUM];
HBITMAP m_hLightBmp;
HBITMAP m_hChimneyBmp;
HBITMAP m_hStarBmp;
/*定义方块颜色数组，与m_hBlockBmp[BLOCK_COLOR_NUM]个数对应，0表示蓝色方块，1表示绿色方块，2表示橙色方块，3表示粉色方块*/
int	m_blockBmpNames[] = { IDB_BLUE_BLOCK, IDB_GREEN_BLOCK, IDB_ORANGE_BLOCK, IDB_PINK_BLOCK };
/*定义屋顶颜色数组，与m_hRoofkBmp[ROOF_COLOR_NUM]个数对应，0表示黑色屋顶，1表示灰色屋顶*/
int	m_roofBmpNames[] = { IDB_BLACK_ROOF, IDB_GREY_ROOF, IDB_ORANGE_ROOF, IDB_BLUE_ROOF};

/*声明英雄、建筑、地形、游戏状态*/
Hero          m_hero;
Building      m_building;
Terrian       m_terrian[MAX_TERRIAN_NUM];
GameStatus     m_gameStatus;

/*全局函数*/
//窗体过程函数
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//初始化
VOID Init(HWND hWnd, WPARAM wParam, LPARAM lParam);

Hero CreateHero(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp, int curFrameIndex, int maxFrameSize);

/*************************************************
Function : 创建背景建筑
Parameter:
	posX、posY表示矩形左上角的坐标
	sizeX、sizeY表示矩形的size
	hBmp表示位图句柄
*************************************************/ 
Building CreateBuilding(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp);

GameStatus CreateGameStatus(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp);

/*************************************************
Fucntion : 创建英雄飞机
Parameter:
	posX、posY表示矩形左上角的坐标
	sizeX、sizeY表示矩形的size
	hBlockBmp表示方块位图句柄
	hRoofBmp表示屋顶位图句柄
	hLightBmp表示路灯位图句柄
	roofWidth、roofHeight屋顶宽度和高度
	blockWidth、blockHeight方块宽度和高度
	lightWidth、lightHeight路灯宽度和高度
*************************************************/ 
Terrian CreateTerrian(LONG posX, LONG posY, LONG sizeX, LONG sizeY, 
	HBITMAP hBlockBmp, HBITMAP hRoofBmp, HBITMAP hLightBmp, HBITMAP hChimneyBmp, HBITMAP hStarBmp, 
	int roofHeight, int blockHeight, int lightHeight, int chimneyHeight, int starHeight);


//双缓冲绘制
VOID Render(HWND hWnd);
//定时器事件
VOID TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam);
//hero更新
VOID HeroUpdate();
//地形更新
VOID TerrianUpdate();
//游戏状态更新
VOID GameStatusUpdate();
//判断是否点击暂停
BOOL Paused(POINT);
//判断是否点击新游戏
BOOL Restart(POINT);
//键盘按下事件处理
VOID KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
//键盘松开事件处理
VOID KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
//左鼠标点击事件
VOID LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);