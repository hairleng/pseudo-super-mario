#include "RushHour.h"

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{ 
	if (!playmusic)
	{
		PlaySound((LPCTSTR)IDR_BGM, GetModuleHandle(0), SND_RESOURCE | SND_ASYNC);
		playmusic = TRUE;
	}
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Mega Plane"),
			MB_OK);

		return 1;
	}

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create,~WS_THICKFRAME  fixed window size
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// WNDWIDTH, WNDHEIGHT: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, //���ô�����ʽ�����ɸı��С���������
		CW_USEDEFAULT, CW_USEDEFAULT,
		WNDWIDTH, WNDHEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Mega Plane"),
			MB_OK);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
// 
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		//��ʼ��
		Init(hWnd, wParam, lParam);
		break;
	case WM_PAINT:
		//����
		Render(hWnd);
		break;
	case WM_KEYDOWN:
		//���̰����¼�
		KeyDown(hWnd, wParam, lParam);
		break;
	case WM_KEYUP:
		//�����ɿ��¼�
		KeyUp(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		//������¼�
		LButtonDown(hWnd, wParam, lParam);
		break;
	case WM_TIMER:
		//��ʱ���¼�
		TimerUpdate(hWnd, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

VOID Init(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//���ر���λͼ
	m_hBackgroundBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BACKGROUND));
	//������������λͼ
	m_hDeadpicBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_DEADPIC));
	//����Buildingλͼ
	m_hBuildingBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_BUILDING));
	//����·��λͼ
	m_hLightBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_LIGHT));
	//�����̴�λͼ
	m_hChimneyBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_CHIMNEY));
	//����Ӣ��λͼ
	m_hHeroBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_HERO));
	//������Ϸ״̬λͼ
	m_hGameStatusBmp =  LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_GAME_STATUS));
	//����starλͼ
	m_hStarBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
		MAKEINTRESOURCE(IDB_STAR));
	//����Blockλͼ
	int k;
	for (k = 0; k < BLOCK_COLOR_NUM; ++k)
	{
		m_hBlockBmp[k] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
			MAKEINTRESOURCE(m_blockBmpNames[k]));
	}
	//�����ݶ�λͼ
	for (k = 0; k < ROOF_COLOR_NUM; ++k)
	{
		m_hRoofkBmp[k] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
			MAKEINTRESOURCE(m_roofBmpNames[k]));
	}
	

	//��������
	m_building = CreateBuilding(0, 70, 500, 250, m_hBuildingBmp);
	//�������Ρ�Ӣ��
	srand(time(NULL));
	for (k = 0; k < 5; ++k)
	{
		int building_height = 150;
		m_terrian[k] = CreateTerrian(k * TERRIAN_WIDTH, building_height, TERRIAN_WIDTH, 300, m_hBlockBmp[k % 4], m_hRoofkBmp[k % 4], 
			m_hLightBmp, m_hChimneyBmp, m_hStarBmp, 33, TERRIAN_WIDTH, 48, 32, 25);
	}
	m_hero = CreateHero(WNDWIDTH / 2, m_terrian[k].pos.y + 50, 30, 50, m_hHeroBmp, 0, 3);
	for (k = 5; k < MAX_TERRIAN_NUM; ++k)
	{
		//���ݸ߶��������
		int building_height = (rand() % 10) * 10 + 100;
		//��������
		if (rand() % 10 == 0)
		{
			building_height = WNDHEIGHT;
		}
		m_terrian[k] = CreateTerrian(k * TERRIAN_WIDTH, building_height, TERRIAN_WIDTH, 300, m_hBlockBmp[k % 4], m_hRoofkBmp[k % 4], 
			m_hLightBmp, m_hChimneyBmp, m_hStarBmp, 33, TERRIAN_WIDTH, 48, 32, 25);
	}

	//������Ϸ״̬
	m_gameStatus = CreateGameStatus(0, 0, 40, 30, m_hGameStatusBmp);

	//������ʱ��
	m_gameStatus.timer_id = SetTimer(hWnd, TIMER, 30, NULL);
}


VOID Render(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc;
	//��ʼ����
	hdc = BeginPaint(hWnd, &ps);

	HDC	hdcBmp, hdcBuffer;
	HBITMAP	cptBmp;
	cptBmp = CreateCompatibleBitmap(hdc, WNDWIDTH, WNDHEIGHT);
	hdcBmp = CreateCompatibleDC(hdc);
	hdcBuffer = CreateCompatibleDC(hdc);

	//����Ϸ��ʼ��״ֵ̬��Ϊ1
	if (m_gameStatus.state == 1)
	{
		SelectObject(hdcBuffer, cptBmp);
		SelectObject(hdcBmp, m_hBackgroundBmp);

		//���Ʊ���������
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);
		//���ƽ���������
		SelectObject(hdcBmp, m_building.hBmp);
		TransparentBlt(
			hdcBuffer, m_building.pos.x, m_building.pos.y,
			m_building.size.cx, m_building.size.cy,
			hdcBmp, 0, 0, m_building.size.cx, m_building.size.cy,
			RGB(255, 255, 255)
			);

		//���Ƶ���
		int k, i;
		for (k = 0; k < MAX_TERRIAN_NUM; ++k)
		{
			Terrian terrian = m_terrian[k];
			SelectObject(hdcBmp, terrian.hRoofBmp);
			TransparentBlt(
				hdcBuffer, terrian.pos.x, terrian.pos.y,
				terrian.roofWidth, terrian.roofHeight,
				hdcBmp, 0, 0, terrian.roofWidth, terrian.roofHeight,
				RGB(255, 255, 255)
				);
			SelectObject(hdcBmp, terrian.hBlockBmp);
			int t;
			for (t = 0; t < terrian.blockNum; ++t)
			{
				TransparentBlt(
					hdcBuffer, terrian.pos.x, terrian.pos.y + terrian.roofHeight + terrian.blockHeight * t,
					terrian.blockWidth, terrian.blockHeight,
					hdcBmp, 0, 0, terrian.blockWidth, terrian.blockHeight,
					RGB(255, 255, 255)
				);
			}
			SelectObject(hdcBmp, terrian.hLightBmp);
			for (i = 0; i < MAX_TERRIAN_NUM; i++)
			{
				Terrian terrian = m_terrian[i];
				if (terrian.pos.y != WNDHEIGHT && i % 3 == 0)
				TransparentBlt(
					hdcBuffer, terrian.pos.x + 28, terrian.pos.y - terrian.lightHeight,
					terrian.lightWidth , terrian.lightHeight,
					hdcBmp, 0, 0, terrian.lightWidth, terrian.lightHeight,
					RGB(255, 255, 255)
					);
			}
			SelectObject(hdcBmp, terrian.hChimneyBmp);
			for (i = 0; i < MAX_TERRIAN_NUM; i++)
			{
				Terrian terrian = m_terrian[i];
				if (terrian.pos.y != WNDHEIGHT && i % 3 == 1)
				TransparentBlt(
					hdcBuffer, terrian.pos.x, terrian.pos.y - terrian.chimneyHeight,
					terrian.chimneyWidth, terrian.chimneyHeight,
					hdcBmp, 0, 0, terrian.chimneyWidth, terrian.chimneyHeight,
					RGB(255, 255, 255)
					);
			}
			//����star������
			SelectObject(hdcBmp, terrian.hStarBmp);
			for (i = 0; i < MAX_TERRIAN_NUM; i++)
			{
				Terrian terrian = m_terrian[i];
				if (terrian.pos.y != WNDHEIGHT && i % 10 == 0 && terrian.isStar == false)
				{
					TransparentBlt(
						hdcBuffer, terrian.pos.x + 30, terrian.pos.y - terrian.chimneyHeight - terrian.starHeight,
						terrian.starWidth, terrian.starHeight,
						hdcBmp, 0, 0, terrian.starWidth, terrian.starHeight,
						RGB(255, 255, 255)
						);
				}
				terrian.isStar = false;
			} 
		}

		//����Hero������
		SelectObject(hdcBmp, m_hero.hBmp);
		TransparentBlt(
			hdcBuffer, m_hero.pos.x, m_hero.pos.y,
			m_hero.size.cx, m_hero.size.cy,
			hdcBmp, 0, m_hero.size.cy * m_hero.curFrameIndex, m_hero.size.cx, m_hero.size.cy,
			RGB(255, 255, 255)
			);

		//������Ϸ״̬
		//��ͣ�����λͼ
		SelectObject(hdcBmp, m_gameStatus.hBmp);
		TransparentBlt(hdcBuffer, m_gameStatus.pos.x, m_gameStatus.pos.y, m_gameStatus.size.cx, m_gameStatus.size.cy,
			hdcBmp, 0, m_gameStatus.size.cy * m_gameStatus.isPaused,
			m_gameStatus.size.cx, m_gameStatus.size.cy, RGB(255, 255, 255));
	
		//���Ʒ���
		TCHAR	szDist[13];
		SetTextColor(hdcBuffer, RGB(255, 251, 240));
		SetBkMode(hdcBuffer, TRANSPARENT);
		TextOut(hdcBuffer, WNDWIDTH - 100, 15, szDist, wsprintf(szDist, _T("����:%d"), m_gameStatus.totalDist));
		int level = m_gameStatus.totalDist / 2000 + 1;
		TextOut(hdcBuffer, WNDWIDTH - 100, 30, szDist, wsprintf(szDist, _T("��%d��"), level));
		TextOut(hdcBuffer, WNDWIDTH - 100, 45, szDist, wsprintf(szDist, _T("����ֵ:%d"), m_gameStatus.life));
	}
	//��Ϸδ��ʼ״̬��Ϊ0
	else if (m_gameStatus.state == 0)
	{
		SelectObject(hdcBuffer, cptBmp);
		SelectObject(hdcBmp, m_hBackgroundBmp);

		//���Ʊ���������
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);

		TCHAR	szDist[13];
		SetTextColor(hdcBuffer, RGB(255, 127, 80));
		SetBkMode(hdcBuffer, TRANSPARENT);
		TextOut(hdcBuffer, WNDWIDTH / 2 - 5, WNDHEIGHT / 2 - 15, szDist, wsprintf(szDist, _T("Begin������")));
	
	}
	//��Ϸ����״̬��Ϊ2
	else if (m_gameStatus.state == 2)
	{
		SelectObject(hdcBuffer, cptBmp);
		SelectObject(hdcBmp, m_hDeadpicBmp);

		//���Ʊ���������
		BitBlt(hdcBuffer, 0, 0, WNDWIDTH, WNDHEIGHT,
			hdcBmp, 0, 0, SRCCOPY);

		TCHAR	szDist[20];
		SetTextColor(hdcBuffer, RGB(255, 255, 0));
		SetBkMode(hdcBuffer, TRANSPARENT);
		TextOut(hdcBuffer, WNDWIDTH / 2 - 40, WNDHEIGHT / 2 + 10, szDist, wsprintf(szDist, _T("�ܾ���:%d"), m_gameStatus.totalDist));
		int level = m_gameStatus.totalDist / 2000 + 1;
		TextOut(hdcBuffer, WNDWIDTH / 2 - 40, WNDHEIGHT / 2 + 40, szDist, wsprintf(szDist, _T("�ܹؿ�:%d"), level));
		
		TextOut(hdcBuffer, WNDWIDTH / 2 - 40, WNDHEIGHT / 2 + 70, szDist, wsprintf(szDist, _T("�������⴦����")));
	}


	//������е���Ϣ���Ƶ���Ļ��
	BitBlt(hdc, 0, 0, WNDWIDTH, WNDHEIGHT, hdcBuffer, 0, 0, SRCCOPY);
		//������Դ��ռ���ڴ�
	DeleteObject(cptBmp);
	DeleteDC(hdcBuffer);
	DeleteDC(hdcBmp);
	
	//��������
	EndPaint(hWnd, &ps);
}

Hero CreateHero(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp, int curFrameIndex, int maxFrameSize)
{
	Hero hero;
	hero.hBmp = hBmp;
	hero.pos.x = posX;
	hero.pos.y = posY;
	hero.size.cx = sizeX;
	hero.size.cy = sizeY;
	hero.curFrameIndex = curFrameIndex;
	hero.maxFrameSize = maxFrameSize;
	return hero;
}

Building CreateBuilding(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp)
{
	Building building;
	building.hBmp = hBmp;
	building.pos.x = posX;
	building.pos.y = posY;
	building.size.cx = sizeX;
	building.size.cy = sizeY;
	return building;
}

GameStatus CreateGameStatus(LONG posX, LONG posY, LONG sizeX, LONG sizeY, HBITMAP hBmp)
{
	GameStatus gameStatus;
	gameStatus.pos.x = posX;
	gameStatus.pos.y = posY;
	gameStatus.size.cx = sizeX;
	gameStatus.size.cy = sizeY;
	gameStatus.hBmp = hBmp;
	gameStatus.totalDist = 0;
	gameStatus.isPaused = false;
	gameStatus.isRestarted = false;
	gameStatus.life = 3;
	gameStatus.flag =  0;
	gameStatus.run_flag = 0;
	gameStatus.state = 0;
	return gameStatus;
}

Terrian CreateTerrian(LONG posX, LONG posY, LONG sizeX, LONG sizeY, 
	HBITMAP hBlockBmp, HBITMAP hRoofBmp, HBITMAP hLightBmp, HBITMAP hChimneyBmp, HBITMAP hStarBmp, 
	int roofHeight, int blockHeight, int lightHeight, int chimneyHeight, int starHeight)
{
	Terrian terrian;
	terrian.isStar = false;
	terrian.pos.x = posX;
	terrian.pos.y = posY;
	terrian.size.cx = sizeX;
	terrian.size.cy = sizeY;
	terrian.hBlockBmp = hBlockBmp;
	terrian.hRoofBmp = hRoofBmp;
	terrian.hLightBmp = hLightBmp;
	terrian.hChimneyBmp = hChimneyBmp;
	terrian.hStarBmp = hStarBmp;
	terrian.roofWidth = sizeX;
	terrian.roofHeight = roofHeight;
	terrian.blockWidth = sizeX;
	terrian.blockHeight = blockHeight;
	terrian.blockNum = (int)ceil((sizeY - roofHeight) * 1.0 / blockHeight); 
	terrian.lightWidth = 40;
	terrian.lightHeight = 48;
	terrian.chimneyWidth = 32;
	terrian.chimneyHeight = 20;
	terrian.starWidth = 25;
	terrian.starHeight = 25;

	return terrian;
}

VOID TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if ((m_gameStatus.state == 1) && (m_gameStatus.life >0))
	{
		HeroUpdate();
		//TerrianUpdate();
		GameStatusUpdate();
	}
	if ((m_gameStatus.state == 1) && (m_gameStatus.life == 0))
	{
		m_gameStatus.state = 2;
		PlaySound((LPCTSTR)IDR_SCREAM, GetModuleHandle(0), SND_RESOURCE | SND_ASYNC);
		
	}
	InvalidateRect(hWnd, NULL, FALSE);
}

VOID HeroUpdate()
{
	//����λ��
	m_hero.pos.x = m_hero.pos.x >= WNDWIDTH ? 0 : m_hero.pos.x;
	TerrianUpdate();

	//���¶���
	++m_hero.curFrameIndex;
	m_hero.curFrameIndex = m_hero.curFrameIndex >= m_hero.maxFrameSize ? 0 : m_hero.curFrameIndex;
	m_hero.pos.y -= m_gameStatus.flag;
	
	int cur_wnd_index = 0;
	for (cur_wnd_index = 0; cur_wnd_index < MAX_TERRIAN_NUM; cur_wnd_index++)
	{
		if (m_terrian[cur_wnd_index].pos.x <= 0)
			break;
	}
	printf("cur_index:%d", cur_wnd_index);

	int cur_hero_left_pos_x = cur_wnd_index * TERRIAN_WIDTH - m_terrian[cur_wnd_index].pos.x + m_hero.pos.x;
	int cur_hero_right_pos_x = cur_hero_left_pos_x + m_hero.size.cx;
	int cur_left_terrian_index = (cur_hero_left_pos_x % (MAX_TERRIAN_NUM * TERRIAN_WIDTH)) / TERRIAN_WIDTH;
	int cur_right_terrian_index = (cur_hero_right_pos_x % (MAX_TERRIAN_NUM * TERRIAN_WIDTH)) / TERRIAN_WIDTH;

	int cur_terrian_index = m_terrian[cur_left_terrian_index].pos.y < m_terrian[cur_right_terrian_index].pos.y ? cur_left_terrian_index : cur_right_terrian_index;
	
	m_hero.left_terrian_index = cur_left_terrian_index;
	m_hero.right_terrian_index = cur_right_terrian_index;

	int lifeplus = 0;
	if(m_gameStatus.flag > 0)
	{	
		m_gameStatus.flag--;
	}
	else if (m_gameStatus.flag <= 0 && m_hero.pos.y + 50 < m_terrian[cur_terrian_index].pos.y)
	{
		m_hero.pos.y += 10;
		if (m_hero.pos.y > m_terrian[cur_terrian_index].pos.y - 50)
			m_hero.pos.y = m_terrian[cur_terrian_index].pos.y - 50;
	}
	if ((m_hero.pos.y + 50 <= m_terrian[cur_terrian_index].pos.y) && (cur_terrian_index % 10 == 0) && (m_hero.pos.x == m_terrian[cur_terrian_index].pos.x + TERRIAN_WIDTH / 2) &&
		(m_hero.pos.y + 50 > m_terrian[cur_terrian_index].pos.y - m_terrian[cur_terrian_index].starHeight))
	{
		m_gameStatus.life = m_gameStatus.life + 1;
		m_terrian[cur_terrian_index].isStar = true;
	}
/*	if ((m_hero.pos.y > m_terrian[k].pos.y - m_terrian[k].chimneyHeight - m_terrian[k].starHeight) &&
		(m_hero.pos.y + 50 < m_terrian[k].pos.y - m_terrian[k].chimneyHeight))
	{
		m_gameStatus.life++;
		m_terrian[k].isStar = TRUE;
	} */
}

VOID TerrianUpdate()
{
	int k;
	for (k = 0; k < MAX_TERRIAN_NUM; ++k)
	{
		m_terrian[k].pos.x -= 5 + m_gameStatus.run_flag + m_gameStatus.totalDist / 2000;
		if (m_terrian[k].pos.x + m_terrian[k].size.cx < 0)
		{
			m_terrian[k].pos.x += MAX_TERRIAN_NUM * m_terrian[k].size.cx;
		}
	}
	if (m_gameStatus.run_flag > 0)
		m_gameStatus.run_flag --;
	
}
//�����ж�
VOID GameStatusUpdate()
{
	if (m_hero.pos.y + 50 > m_terrian[m_hero.right_terrian_index].pos.y)
	{
		m_gameStatus.life--;
		playmusic = FALSE;
		PlaySound((LPCTSTR)IDR_SCREAM, GetModuleHandle(0), SND_RESOURCE | SND_ASYNC);
		playmusic = FALSE;
		//PlaySound((LPCTSTR)IDR_BGM, GetModuleHandle(0), SND_RESOURCE | SND_ASYNC | SND_LOOP);
		m_hero.pos.y = 0;
	}
	if (m_hero.pos.y + 50 >= WNDHEIGHT)
	{
		m_gameStatus.life--;
		playmusic = FALSE;
		//PlaySound((LPCTSTR)IDR_SCREAM, GetModuleHandle(0), SND_RESOURCE | SND_ASYNC);
		//playmusic = FALSE;
		PlaySound((LPCTSTR)IDR_BGM, GetModuleHandle(0), SND_RESOURCE | SND_ASYNC | SND_LOOP);
		m_hero.pos.y = 0;
	}
	if (!playmusic)
	{
		PlaySound((LPCTSTR)IDR_BGM, GetModuleHandle(0), SND_RESOURCE | SND_ASYNC);
		playmusic = TRUE;
	}
	++m_gameStatus.totalDist;

	
}
//��ͣ��Ϸ
BOOL Paused(POINT ptMouse)
{
	RECT rPause;

	rPause.left = m_gameStatus.pos.x;
	rPause.top = m_gameStatus.pos.y;
	rPause.right = m_gameStatus.size.cx;
	rPause.bottom = m_gameStatus.size.cy;

	return PtInRect(&rPause, ptMouse);
}

BOOL Restart(POINT ptMouse)
{
	RECT rRestart;

	rRestart.left = m_gameStatus.pos.x;
	rRestart.top = m_gameStatus.pos.y;
	rRestart.right = WNDWIDTH;
	rRestart.bottom = WNDHEIGHT;

	return PtInRect(&rRestart, ptMouse);
}

VOID KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_UP:
		m_gameStatus.flag = 10;
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case VK_DOWN:
		if (m_hero.pos.y < m_gameStatus.pos.y)
			m_hero.pos.y = m_gameStatus.pos.y;
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case VK_RIGHT:
		m_gameStatus.run_flag = 10;
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case VK_SPACE:
		if (m_gameStatus.state == 0)
			m_gameStatus.state = 1;
		if (m_gameStatus.state == 2)
		{
			//��������
			m_building = CreateBuilding(0, 70, 500, 250, m_hBuildingBmp);
			//��������
			srand(time(NULL));
			int k;
			for (k = 0; k < 5; ++k)
			{
				int building_height = 150;
				m_terrian[k] = CreateTerrian(k * TERRIAN_WIDTH, building_height, TERRIAN_WIDTH, 300, m_hBlockBmp[k % 4], m_hRoofkBmp[k % 4], 
					m_hLightBmp,  m_hChimneyBmp, m_hStarBmp, 33, TERRIAN_WIDTH, 48, 32, 25);
			}
			m_hero = CreateHero(WNDWIDTH / 2, m_terrian[k].pos.y + 50, 30, 50, m_hHeroBmp, 0, 3);
			for (k = 5; k < MAX_TERRIAN_NUM; ++k)
			{
				//���ݸ߶��������
				int building_height = (rand() % 5) * 20 + 100;
				//��������
				if (rand() % 10 == 0)
				{
					building_height = WNDHEIGHT;
				}
				m_terrian[k] = CreateTerrian(k * TERRIAN_WIDTH, building_height, TERRIAN_WIDTH, 300, m_hBlockBmp[k % 4], m_hRoofkBmp[k % 4], 
					m_hLightBmp, m_hChimneyBmp, m_hStarBmp, 33, TERRIAN_WIDTH, 48, 32, 25);
			}

			//������Ϸ״̬
			m_gameStatus = CreateGameStatus(0, 0, 40, 30, m_hGameStatusBmp);

			//������ʱ��
			m_gameStatus.timer_id = SetTimer(hWnd, TIMER, 30, NULL);

			m_gameStatus.life = 4;

		}
		InvalidateRect(hWnd, NULL, FALSE);
	default:
		break;
	}
}

VOID KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_UP:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case VK_DOWN:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	default:
		break;
	}
}

VOID LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POINT ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);
	
	
	//������������Ϸ
	if ((Restart(ptMouse)) && (m_gameStatus.state == 2))
	{
		if (!m_gameStatus.isRestarted)
		{
			//����Ӣ�ۡ�����
			m_hero = CreateHero(100, 100, 30, 50, m_hHeroBmp, 0, 3);
			m_building = CreateBuilding(0, 70, 500, 250, m_hBuildingBmp);
			//��������
			srand(time(NULL));
			int k;
			for (k = 0; k < 5; ++k)
			{
				int building_height = 150;
				m_terrian[k] = CreateTerrian(k * TERRIAN_WIDTH, building_height, TERRIAN_WIDTH, 300, m_hBlockBmp[k % 4], m_hRoofkBmp[k % 4], 
					m_hLightBmp, m_hChimneyBmp,m_hStarBmp, 33, TERRIAN_WIDTH, 48, 32, 25);
			}
			m_hero = CreateHero(WNDWIDTH / 2, m_terrian[k].pos.y + 50, 30, 50, m_hHeroBmp, 0, 3);
			for (k = 5; k < MAX_TERRIAN_NUM; ++k)
			{
				//���ݸ߶��������
				int building_height = (rand() % 10) * 10 + 100;
				//��������
				if (rand() % 10 == 0)
				{
					building_height = WNDHEIGHT;
				}
				m_terrian[k] = CreateTerrian(k * TERRIAN_WIDTH, building_height, TERRIAN_WIDTH, 300, m_hBlockBmp[k % 4], m_hRoofkBmp[k % 4], 
					m_hLightBmp, m_hChimneyBmp, m_hStarBmp, 33, TERRIAN_WIDTH, 48, 32, 25);
			}

			//������Ϸ״̬
			m_gameStatus = CreateGameStatus(0, 0, 40, 30, m_hGameStatusBmp);

			//������ʱ��
			m_gameStatus.timer_id = SetTimer(hWnd, TIMER, 30, NULL);
		
			m_gameStatus.life = 4;
		}
		else
		{
			m_gameStatus.isRestarted = FALSE;
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}

	//�������˼�������ͣͼ��
	if (Paused(ptMouse))
	{
		if (!m_gameStatus.isPaused)
		{
			KillTimer(hWnd, TIMER);
			m_gameStatus.isPaused = TRUE;
		}
		else
		{
			SetTimer(hWnd, TIMER, 50, NULL);
			m_gameStatus.isPaused = FALSE;
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
	else if (m_gameStatus.state == 0)
		m_gameStatus.state = 1;
	else if (m_gameStatus.state == 1)
		m_gameStatus.flag = 10;
}