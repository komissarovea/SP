#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

// ���������� ����������
LPTSTR g_lpszClassName = TEXT("sp_pr6_2_class");
LPTSTR g_lpszAplicationTitle = TEXT("�����������: ���������� �������, 60322-2");
HINSTANCE g_hInst;

//========��������������� ���������� �������=========================
/* ��������� �������� ���� */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
/* ����������� ������ �������� ���� */
BOOL Register(HINSTANCE);
/* �������� �������� ���� */
HWND Create(HINSTANCE, int);

/* ����������� �������� */
/*--- WM_PAINT -------------------------------------------------------*/
void WndProc_OnPaint(HWND hwnd);
/*--- WM_DESTROY -----------------------------------------------------*/
void WndProc_OnDestroy(HWND hWnd);
/*--------------------------------------------------------------------*/
//====================================================================//

// ����� ����� � ��������� ===========================================//
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpszCmdLine, int nCmdShow)
{
	g_hInst = hInstance;

	BOOL isRegistered = Register(hInstance);
	if (!isRegistered) {
		MessageBox(NULL, TEXT("������ ����������� ������ ����!"),
			TEXT("������"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	HWND hWnd = Create(hInstance, nCmdShow);
	if (!hWnd)
	{
		MessageBox(NULL, TEXT("���� �� �������!"),
			TEXT("������"), MB_OK | MB_ICONERROR);
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

//== ������� ��� ����������� ������ ���� =============================//
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

//== ������� �������� ���� ===========================================//
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

//== ������� ��������� �������� ���� =================================//
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

//=== ���������� ��������� WM_PAINT ==================================//
void WndProc_OnPaint(HWND hwnd)
{
	FORWARD_WM_PAINT(hwnd, DefWindowProc);
}

//=== ���������� ��������� WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hWnd)
{
	PostQuitMessage(0);
}