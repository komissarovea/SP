#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

// Глобальные переменные
LPTSTR g_lpszClassName = TEXT("sp_pr6_class");
LPTSTR g_lpszAplicationTitle = TEXT("Разработчик: Комиссаров Евгений, 60322-2");
int nPosX = 10;
int nPosY = 10;
HDC g_hDC;

//========Предварительное объявление функций=========================
/* Процедура главного окна */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
/* Регистрация класса главного окна */
BOOL Register(HINSTANCE);
/* Создание главного окна */
HWND Create(HINSTANCE, int);

/* Обработчики соощений */
/*--- WM_MOUSEMOVE -------------------------------------------------------*/
void WndProc_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
/*--- WM_LBUTTONDOWN -------------------------------*/
void  WndProc_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
/*--- WM_LBUTTONUP ---------------------------------------------------*/
void  WndProc_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
/*--- WM_DESTROY -----------------------------------------------------*/
void WndProc_OnDestroy(HWND hWnd);
/*--------------------------------------------------------------------*/
//====================================================================//

// Точка входа в программу ===========================================//
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpszCmdLine, int nCmdShow)
{
	BOOL isRegistered = Register(hInstance);
	if (!isRegistered) {
		MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	HWND hwnd = Create(hInstance, nCmdShow);
	if (!hwnd)
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
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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
		400,
		400,
		600,
		400,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (hwnd == NULL)
		return hwnd;
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	g_hDC = GetDC(hwnd);
	return hwnd;
}

//== Оконная процедура главного окна =================================//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, WndProc_OnLButtonDown);
		HANDLE_MSG(hwnd, WM_MOUSEMOVE, WndProc_OnMouseMove);
		HANDLE_MSG(hwnd, WM_LBUTTONUP, WndProc_OnLButtonUp);
		HANDLE_MSG(hwnd, WM_DESTROY, WndProc_OnDestroy);
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//=== Обработчик сообщения WM_LBUTTONDOWN ==========//
void WndProc_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x,
	int y, UINT keyFlags)
{
	if (!fDoubleClick) //Одиночный щелчек
	{
		MoveToEx(g_hDC, x, y, NULL);
		SetCapture(hwnd);
	}
}

//=== Обработчик сообщения WM_MOUSEMOVE ==================================//
void WndProc_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	TCHAR S[100]; //Буфер для формирования выводимой строки
	HDC hDC = GetDC(hwnd); //Получаем контекст устройства графического вывода

	wsprintf(S, TEXT("Координаты мыши: x = %d y = %d   "), x, y);
	if ((keyFlags & MK_LBUTTON) == MK_LBUTTON)
	{
		SetTextColor(hDC, RGB(0, 255, 0));
		LineTo(g_hDC, x, y);
	}

	//SetBkColor(DC, GetSysColor(COLOR_WINDOW)); //Устанавливаем цвет фона
	TextOut(hDC, nPosX, nPosY, S, lstrlen(S)); // Выводим строку текста

	ReleaseDC(hwnd, hDC);//Освобождаем контекст
}

//=== Обработчик сообщения WM_LBUTTONUP ==============================//
void WndProc_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	ReleaseCapture();
}

//=== Обработчик сообщения WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hwnd)
{
	ReleaseDC(hwnd, g_hDC);
	PostQuitMessage(0);
}