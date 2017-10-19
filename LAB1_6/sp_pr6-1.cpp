#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

// ���������� ����������
LPTSTR g_lpszClassName = TEXT("sp_pr6_class");
LPTSTR g_lpszAplicationTitle = TEXT("�����������: ���������� �������, 60322-2");
int nPosX = 10;
int nPosY = 10;
HDC g_hDC;

//========��������������� ���������� �������=========================
/* ��������� �������� ���� */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
/* ����������� ������ �������� ���� */
BOOL Register(HINSTANCE);
/* �������� �������� ���� */
HWND Create(HINSTANCE, int);

/* ����������� �������� */
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

// ����� ����� � ��������� ===========================================//
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpszCmdLine, int nCmdShow)
{
	BOOL isRegistered = Register(hInstance);
	if (!isRegistered) {
		MessageBox(NULL, TEXT("������ ����������� ������ ����!"),
			TEXT("������"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	HWND hwnd = Create(hInstance, nCmdShow);
	if (!hwnd)
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
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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
	g_hDC = GetDC(hwnd);
	return hwnd;
}

//== ������� ��������� �������� ���� =================================//
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

//=== ���������� ��������� WM_LBUTTONDOWN ==========//
void WndProc_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x,
	int y, UINT keyFlags)
{
	if (!fDoubleClick) //��������� ������
	{
		MoveToEx(g_hDC, x, y, NULL);
		SetCapture(hwnd);
	}
}

//=== ���������� ��������� WM_MOUSEMOVE ==================================//
void WndProc_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	TCHAR S[100]; //����� ��� ������������ ��������� ������
	HDC hDC = GetDC(hwnd); //�������� �������� ���������� ������������ ������

	wsprintf(S, TEXT("���������� ����: x = %d y = %d   "), x, y);
	if ((keyFlags & MK_LBUTTON) == MK_LBUTTON)
	{
		SetTextColor(hDC, RGB(0, 255, 0));
		LineTo(g_hDC, x, y);
	}

	//SetBkColor(DC, GetSysColor(COLOR_WINDOW)); //������������� ���� ����
	TextOut(hDC, nPosX, nPosY, S, lstrlen(S)); // ������� ������ ������

	ReleaseDC(hwnd, hDC);//����������� ��������
}

//=== ���������� ��������� WM_LBUTTONUP ==============================//
void WndProc_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	ReleaseCapture();
}

//=== ���������� ��������� WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hwnd)
{
	ReleaseDC(hwnd, g_hDC);
	PostQuitMessage(0);
}