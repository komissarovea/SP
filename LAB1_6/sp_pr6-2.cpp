#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "resource.h"

// ���������� ����������
LPTSTR g_lpszClassName = TEXT("sp_pr6_2_class");
LPTSTR g_lpszAplicationTitle = TEXT("�����������: ���������� �������, 60322-2");
HINSTANCE g_hInst;
HWND hFrameWnd = NULL;
HDC g_chDC;
BOOL fKeyDraw = 0;
int counter = 0;

// ������� 10
const int x0 = 100; // ���������� �������� ������ ���� ������������ ������
const int y0 = 200; // �������� ���� ���������� ������� ������������� ����
const int cx = 100; // ������
const int cy = 100; // ������ 
const char leftKey = 'A'; // ������� �����������
const char rightKey = 'S';
const char upKey = 'D';
const char downKey = 'F';
const int d = 3; // �������� ��������
const int n = 4; // ����� �������
const COLORREF cfColor = RGB(255, 255 / 2, 0); // 0,5���+��
const COLORREF blackColor = RGB(0, 0, 0);

//========��������������� ���������� �������=========================
/* ��������� �������� ���� */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
/* ��������� ����-����� */
LRESULT CALLBACK FrameDlgProc(HWND, UINT, WPARAM, LPARAM);
/* ����������� ������ �������� ���� */
BOOL Register(HINSTANCE);
/* �������� �������� ���� */
HWND Create(HINSTANCE, int);

/* ����������� �������� */
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

//== ������� ��������� �������� ���� =================================//
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

//=== ���������� ��������� WM_CREATE ==================================//
BOOL WndProc_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	hFrameWnd = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_FRAME_DIALOG),
		hwnd, (DLGPROC)FrameDlgProc);
	g_chDC = GetDC(hFrameWnd);
	SetDCPenColor(g_chDC, cfColor);
	MoveWindow(hFrameWnd, x0, y0, cx, cy, TRUE);
	return TRUE;
}

//=== ���������� ��������� WM_KEYDOWN ==================================//
void WndProc_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	int hd = 0; // �������������� ��������
	int vd = 0; // ������������ ��������
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
		// ���������� ������� ����� ����
		POINT ptClientUpL;
		// ���������� ������ ������ ���� 
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

//=== ���������� ��������� WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hWnd)
{
	ReleaseDC(hFrameWnd, g_chDC);
	PostQuitMessage(0);
}

#pragma endregion

#pragma region FrameDlgProc

//== ������� ��������� ����-����� =================================//
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

//=== ���������� ��������� WM_LBUTTONDOWN ==========//
void FrameDlgProc_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	if (fKeyDraw)
	{
		SetCapture(hwnd);
		MoveToEx(g_chDC, x, y, NULL);

		RECT rcClient;
		POINT ptClientUL;
		POINT ptClientLR;

		// ��������� ���������� ��������� ������� ��� ���������� �������,
		// � �������������� �� � ���������� ����������.. 
		GetClientRect(hwnd, &rcClient);
		ptClientUL.x = rcClient.left;
		ptClientUL.y = rcClient.top;

		// ��������� �� ������� � ������ � ������ ��������,
		// ������ ��� ����������, ���������� �����  GetClientRect 
		// �� �������� ����� ����� � ����� ������ �������.
		ptClientLR.x = rcClient.right + 1;
		ptClientLR.y = rcClient.bottom + 1;
		ClientToScreen(hwnd, &ptClientUL);
		ClientToScreen(hwnd, &ptClientLR);

		// �������� ��������� ���������� ��������� ������� � ��������� rcClient 
		// ���������  ��������� rcClient  ������� ClipCursor ��������� 
		// ����������� ������� ���� ���������� ��������.
		SetRect(&rcClient, ptClientUL.x, ptClientUL.y,
			ptClientLR.x, ptClientLR.y);
		ClipCursor(&rcClient);
	}
}

//=== ���������� ��������� WM_MOUSEMOVE ==================================//
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

//=== ���������� ��������� WM_LBUTTONUP ==============================//
void  FrameDlgProc_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	ClipCursor(NULL);
	ReleaseCapture();
}

#pragma endregion

