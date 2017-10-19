#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "resource.h"

// Глобальные переменные
LPTSTR g_lpszClassName = TEXT("sp_pr6_2_class");
LPTSTR g_lpszAplicationTitle = TEXT("Разработчик: Комиссаров Евгений, 60322-2");
HINSTANCE g_hInst;
HWND hFrameWnd = NULL;
HDC g_chDC;
BOOL fKeyDraw = 0;
int counter = 0;

// Вариант 10
const int x0 = 100; // координаты верхнего левого угла относительно левого
const int y0 = 200; // верхнего угла клиентской области родительского окна
const int cx = 100; // ширина
const int cy = 100; // высота 
const char leftKey = 'A'; // клавишы перемещения
const char rightKey = 'S';
const char upKey = 'D';
const char downKey = 'F';
const int d = 3; // величина смещения
const int n = 4; // номер позиции
const COLORREF cfColor = RGB(255, 255 / 2, 0); // 0,5зел+кр
const COLORREF blackColor = RGB(0, 0, 0);

//========Предварительное объявление функций=========================
/* Процедура главного окна */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
/* Процедура окна-рамки */
LRESULT CALLBACK FrameDlgProc(HWND, UINT, WPARAM, LPARAM);
/* Регистрация класса главного окна */
BOOL Register(HINSTANCE);
/* Создание главного окна */
HWND Create(HINSTANCE, int);

/* Обработчики соощений */
/*--- WM_CREATE -------------------------------------------------------*/
BOOL WndProc_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
/*--- WM_KEYDOWN --------------------------------------------*/
void WndProc_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
/*--- WM_DESTROY -----------------------------------------------------*/
void WndProc_OnDestroy(HWND hWnd);

/*--- WM_LBUTTONDOWN -------------------------------*/
void  FrameDlgProc_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
/*--- WM_MOUSEMOVE -------------------------------------------------------*/
void FrameDlgProc_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
/*--- WM_LBUTTONUP ---------------------------------------------------*/
void  FrameDlgProc_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
/*--------------------------------------------------------------------*/
//====================================================================//

// Точка входа в программу ===========================================//
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpszCmdLine, int nCmdShow)
{
	g_hInst = hInstance;

	BOOL isRegistered = Register(hInstance);
	if (!isRegistered) {
		MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	HWND hWnd = Create(hInstance, nCmdShow);
	if (!hWnd)
	{
		MessageBox(NULL, TEXT("Окно не создано!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//== Функция для регистрации класса окна =============================//
BOOL Register(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.lpszClassName = g_lpszClassName;
	wc.lpfnWndProc = WndProc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	wc.hIcon = LoadIcon(NULL, IDI_SHIELD);
	wc.hCursor = LoadCursor(NULL, IDC_HAND);

	return RegisterClassEx(&wc);
}

//== Функция создания окна ===========================================//
HWND Create(HINSTANCE hInstance, int nCmdShow)
{
	HWND hwnd = CreateWindowEx(NULL, g_lpszClassName,
		g_lpszAplicationTitle,
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		800,
		600,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (hwnd == NULL)
		return hwnd;
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return hwnd;
}

#pragma region WndProc

//== Оконная процедура главного окна =================================//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, WndProc_OnCreate);
		HANDLE_MSG(hwnd, WM_KEYDOWN, WndProc_OnKey);
		HANDLE_MSG(hwnd, WM_DESTROY, WndProc_OnDestroy);
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//=== Обработчик сообщения WM_CREATE ==================================//
BOOL WndProc_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	hFrameWnd = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_FRAME_DIALOG),
		hwnd, (DLGPROC)FrameDlgProc);
	g_chDC = GetDC(hFrameWnd);
	SetDCPenColor(g_chDC, cfColor);
	MoveWindow(hFrameWnd, x0, y0, cx, cy, TRUE);
	return TRUE;
}

//=== Обработчик сообщения WM_KEYDOWN ==================================//
void WndProc_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	int hd = 0; // горизонтальное смещение
	int vd = 0; // вертикальное смещение
	switch (vk)
	{
	case leftKey:
		hd = -d;
		break;
	case rightKey:
		hd = d;
		break;
	case upKey:
		vd = -d;
		break;
	case downKey:
		vd = d;
		break;
	case VK_F2:
		//MessageBox(hwnd, TEXT("F2"), NULL, MB_OK);
		fKeyDraw = TRUE;
		break;
	case VK_F3:
		fKeyDraw = FALSE;
		break;
	}

	if (hd != 0 || vd != 0)
	{
		RECT wi;
		// Клиентский верхний левый угол
		POINT ptClientUpL;
		// Клиентский правый нижний угол 
		POINT ptClientLowR;
		GetWindowRect(hFrameWnd, &wi);
		ptClientUpL.x = wi.left;
		ptClientUpL.y = wi.top;
		ptClientLowR.x = wi.right;
		ptClientLowR.y = wi.bottom;
		ScreenToClient(hwnd, &ptClientUpL);
		ScreenToClient(hwnd, &ptClientLowR);
		int x1 = ptClientUpL.x + hd;
		int y1 = ptClientUpL.y + vd;
		MoveWindow(hFrameWnd, x1, y1, cx, cy, TRUE);

		HDC hdc = GetDC(hwnd);
		TCHAR buffer[50];
		wsprintf(buffer, TEXT("(%d, %d)     "), x1, y1);
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		SetTextColor(hdc, cfColor);
		DrawText(hdc, buffer, -1, &clientRect, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
		ReleaseDC(hwnd, hdc);
		//TextOut(hdc, 10, 300, buffer, lstrlen(buffer));
	}

}

//=== Обработчик сообщения WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hWnd)
{
	ReleaseDC(hFrameWnd, g_chDC);
	PostQuitMessage(0);
}

#pragma endregion

#pragma region FrameDlgProc

//== Оконная процедура окна-рамки =================================//
LRESULT CALLBACK FrameDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, FrameDlgProc_OnLButtonDown);
		HANDLE_MSG(hwnd, WM_MOUSEMOVE, FrameDlgProc_OnMouseMove);
		HANDLE_MSG(hwnd, WM_LBUTTONUP, FrameDlgProc_OnLButtonUp);
	}
	return FALSE;
}

//=== Обработчик сообщения WM_LBUTTONDOWN ==========//
void FrameDlgProc_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	if (fKeyDraw)
	{
		SetCapture(hwnd);
		MoveToEx(g_chDC, x, y, NULL);

		RECT rcClient;
		POINT ptClientUL;
		POINT ptClientLR;

		// Получение аппаратных координат дисплея для клиентской области,
		// и преобразование их в клиентские координаты.. 
		GetClientRect(hwnd, &rcClient);
		ptClientUL.x = rcClient.left;
		ptClientUL.y = rcClient.top;

		// Добавляем по единице к правой и нижней сторонам,
		// потому что координаты, полученные через  GetClientRect 
		// не включают самый левый и самый нижний пикселы.
		ptClientLR.x = rcClient.right + 1;
		ptClientLR.y = rcClient.bottom + 1;
		ClientToScreen(hwnd, &ptClientUL);
		ClientToScreen(hwnd, &ptClientLR);

		// Копируем клиентные координаты клиентной области в структуру rcClient 
		// Передавая  структуру rcClient  функции ClipCursor ограничим 
		// перемещения курсора мыши клиентской областью.
		SetRect(&rcClient, ptClientUL.x, ptClientUL.y,
			ptClientLR.x, ptClientLR.y);
		ClipCursor(&rcClient);
	}
}

//=== Обработчик сообщения WM_MOUSEMOVE ==================================//
void FrameDlgProc_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	if (keyFlags & MK_LBUTTON && fKeyDraw)
	{
		/*POINT ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(hwnd, &ptCursor);*/

		SelectObject(g_chDC, GetStockObject(BLACK_PEN));
		LineTo(g_chDC, x, y);

		counter++;
		if (counter > n)
		{
			SelectObject(g_chDC, GetStockObject(DC_PEN));
			Ellipse(g_chDC, x - 4, y - 4, x + 4, y + 4);
			counter = 0;
		}
	}
}

//=== Обработчик сообщения WM_LBUTTONUP ==============================//
void  FrameDlgProc_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	ClipCursor(NULL);
	ReleaseCapture();
}

#pragma endregion

