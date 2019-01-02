#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <math.h>
#include "resource.h"
#include <stdio.h>
#include <time.h>
#include "mmsystem.h"//��������ͷ�ļ�
#pragma comment(lib,"winmm.lib")//��������ͷ�ļ���

/*����������WNDWIDTH��WNDHEIGHTΪ���ڵĿ�͸�*/
#define TIMER                1
#define TIMER2               2
#define WNDWIDTH             500
#define WNDHEIGHT            360
#define BLOCK_COLOR_NUM      4
#define ROOF_COLOR_NUM       4
#define MAX_TERRIAN_NUM      100
#define TERRIAN_WIDTH        90
BOOL playmusic = FALSE;

/*Ӣ�۽ṹ�壬Ӣ��λͼ��λ�á���С*/
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

/*���νṹ�壬���η���λͼ���ݶ�λͼ��λ�á���С���ݶ��Լ������С���������*/
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

/*���������ṹ�壬����λͼ��λ�á���С������*/
typedef struct
{
	HBITMAP	hBmp;
	POINT	pos;
	SIZE	size;
}Building;

/*��Ϸ״̬�ṹ��*/
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

/*ȫ�ֱ���*/
static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("���ʱ��");

/*����λͼ���*/
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
/*���巽����ɫ���飬��m_hBlockBmp[BLOCK_COLOR_NUM]������Ӧ��0��ʾ��ɫ���飬1��ʾ��ɫ���飬2��ʾ��ɫ���飬3��ʾ��ɫ����*/
int	m_blockBmpNames[] = { IDB_BLUE_BLOCK, IDB_GREEN_BLOCK, IDB_ORANGE_BLOCK, IDB_PINK_BLOCK };
/*�����ݶ���ɫ���飬��m_hRoofkBmp[ROOF_COLOR_NUM]������Ӧ��0��ʾ��ɫ�ݶ���1��ʾ��ɫ�ݶ�*/
int	m_roofBmpNames[] = { IDB_BLACK_ROOF, IDB_GREY_ROOF, IDB_ORANGE_ROOF, IDB_BLUE_ROOF};

/*����Ӣ�ۡ����������Ρ���Ϸ״̬*/
Hero          m_hero;
Building      m_building;
Terrian       m_terrian[MAX_TERRIAN_NUM];
GameStatus     m_gameStatus;

/*ȫ�ֺ���*/
//������̺���
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//��ʼ��
VOID Init(HWND hWnd, WPARAM wParam, LPARAM lParam);

Hero CreateHero(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp, int curFrameIndex, int maxFrameSize);

/*************************************************
Function : ������������
Parameter:
	posX��posY��ʾ�������Ͻǵ�����
	sizeX��sizeY��ʾ���ε�size
	hBmp��ʾλͼ���
*************************************************/ 
Building CreateBuilding(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp);

GameStatus CreateGameStatus(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp);

/*************************************************
Fucntion : ����Ӣ�۷ɻ�
Parameter:
	posX��posY��ʾ�������Ͻǵ�����
	sizeX��sizeY��ʾ���ε�size
	hBlockBmp��ʾ����λͼ���
	hRoofBmp��ʾ�ݶ�λͼ���
	hLightBmp��ʾ·��λͼ���
	roofWidth��roofHeight�ݶ���Ⱥ͸߶�
	blockWidth��blockHeight�����Ⱥ͸߶�
	lightWidth��lightHeight·�ƿ�Ⱥ͸߶�
*************************************************/ 
Terrian CreateTerrian(LONG posX, LONG posY, LONG sizeX, LONG sizeY, 
	HBITMAP hBlockBmp, HBITMAP hRoofBmp, HBITMAP hLightBmp, HBITMAP hChimneyBmp, HBITMAP hStarBmp, 
	int roofHeight, int blockHeight, int lightHeight, int chimneyHeight, int starHeight);


//˫�������
VOID Render(HWND hWnd);
//��ʱ���¼�
VOID TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam);
//hero����
VOID HeroUpdate();
//���θ���
VOID TerrianUpdate();
//��Ϸ״̬����
VOID GameStatusUpdate();
//�ж��Ƿ�����ͣ
BOOL Paused(POINT);
//�ж��Ƿ�������Ϸ
BOOL Restart(POINT);
//���̰����¼�����
VOID KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
//�����ɿ��¼�����
VOID KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
//��������¼�
VOID LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);