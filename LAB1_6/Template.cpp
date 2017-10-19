#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

// Глобальные переменные
LPTSTR g_lpszClassName = TEXT("sp_pr6_2_class");
LPTSTR g_lpszAplicationTitle = TEXT("Разработчик: Комиссаров Евгений, 60322-2");
HINSTANCE g_hInst;

//========Предварительное объявление функций=========================
/* Процедура главного окна */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
/* Регистрация класса главного окна */
BOOL Register(HINSTANCE);
/* Создание главного окна */
HWND Create(HINSTANCE, int);

/* Обработчики соощений */
/*--- WM_PAINT -------------------------------------------------------*/
void WndProc_OnPaint(HWND hwnd);
/*--- WM_DESTROY -----------------------------------------------------*/
void WndProc_OnDestroy(HWND hWnd);
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
	return hwnd;
}

//== Оконная процедура главного окна =================================//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_PAINT, WndProc_OnPaint);
		HANDLE_MSG(hwnd, WM_DESTROY, WndProc_OnDestroy);
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//=== Обработчик сообщения WM_PAINT ==================================//
void WndProc_OnPaint(HWND hwnd)
{
	FORWARD_WM_PAINT(hwnd, DefWindowProc);
}

//=== Обработчик сообщения WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hWnd)
{
	PostQuitMessage(0);
}