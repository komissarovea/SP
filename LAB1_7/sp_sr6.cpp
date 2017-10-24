#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <math.h>

// ���������� ����������
LPTSTR g_lpszClassName = TEXT("sp_sr6_class");
LPTSTR g_lpszAplicationTitle = TEXT("�����������: ���������� �������, 60322-2");
LPTSTR g_lpszEmfFile = TEXT("kea.emf");
HINSTANCE g_hInst;
bool firstPaint = true;

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
void WndProc_OnDestroy(HWND hwnd);
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
	wc.style = CS_VREDRAW | CS_HREDRAW;

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
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hwnd, &ps); //�������� ��������
	RECT rect;
	GetClientRect(hwnd, &rect); //������ ��������� �������

	if (firstPaint) {
#pragma region Text Logic
		LPTSTR lpszFio = TEXT("���������� ������� �������������");
		// ��������� ������-������� ����, �������� �������� �10
		SetTextColor(hDC, RGB(144, 238, 144));
		SetBkMode(hDC, TRANSPARENT); //� ��� �� ������� (����������)

									 // ���������� �������� �� ���� ���������� ���� �� ������ ������
		int deviceCaps = GetDeviceCaps(hDC, LOGPIXELSY);
		// ������ � ������� 24, �������� �������� �10	
		int heightPt = -MulDiv(24, deviceCaps, 72);
		HFONT hFont = CreateFont( //�������� �����
			heightPt,
			0, //������� ������ �������
			0, //���� �������
			0, //���� ���������� �������� �����
			0, //������� ������
			TRUE, //����� - ���������, �������� �������� �10
			0, //�������������
			0, //������������
			DEFAULT_CHARSET, //������������� ������ ��������
			0, //�������� ������
			0, //�������� ���������
			0, //�������� ������
			0, //��� ����� ��������� ������ � ���������
			L"Microsoft Sans Serif"); // ��� ������, �������� �������� �10
		SelectObject(hDC, hFont); // �������� ��������� �����

		DrawText(hDC, lpszFio, -1, &rect, DT_SINGLELINE | DT_CENTER);
		DeleteObject(hFont); // ������ �����
#pragma endregion

#pragma region Figure Logic
#define PI 3.14159265359
							 //------ ������, �������� �������� �10		
		double r = 100, angle = 270; //������ = 100, �������� �������� �10
									 // �������� �������� ���� �������� 3 �������� �����, �������� �������� �10
		HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
		//�������� ����� � �������������� ���������� �������� �����, �������� �������� �10
		HBRUSH hBrush = CreateHatchBrush(HS_HORIZONTAL, RGB(255, 0, 0));
		SelectObject(hDC, hPen); // �������� ����
		SelectObject(hDC, hBrush); // �������� �����
		SetPolyFillMode(hDC, WINDING); // �������� ������� ��������(��� �������� ������)

		POINT pt[5], final_pt[5];
		//� ����� ���� �� ������ � ��������� �� � ������ �����
		for (int i = 0; i < 5; i++)
		{
			pt[i].x = 150 + r / 2 * cos(angle*PI / 180); //���������� ��������� ���������� x
			pt[i].y = 150 + r / 2 * sin(angle*PI / 180); //���������� ��������� ���������� y
			angle += 360 / 5; //����������� ���� �� �������� ���� ����������� ��������������
		}
		//���������� � ����� ������ ����������� ���������
		final_pt[0] = pt[0];
		final_pt[1] = pt[2];
		final_pt[2] = pt[4];
		final_pt[3] = pt[1];
		final_pt[4] = pt[3];

		Polygon(hDC, final_pt, 5);//�������� ������������� ��������� ������ ��������� �����

		DeleteObject(hPen);  // ������ ����
		DeleteObject(hBrush); // ������ �����  
#pragma endregion

#pragma region Save MetaFile
		// �������� ��������
		HDC hdcMeta = CreateEnhMetaFile(NULL, g_lpszEmfFile, NULL, NULL);
		int cx = rect.right - rect.left;
		int cy = rect.bottom - rect.top;
		// ��������� �������� ���������
		BitBlt(hdcMeta, 0, 0, cx, cy, hDC, 0, 0, SRCCOPY);
		HENHMETAFILE hemf = CloseEnhMetaFile(hdcMeta);
		DeleteEnhMetaFile(hemf); //������ ��������
#pragma endregion

		firstPaint = false;
	}
	else
	{
		HENHMETAFILE hemf = GetEnhMetaFile(g_lpszEmfFile); //�������� ��������
		PlayEnhMetaFile(hDC, hemf, &rect); //������������� ������ ���������	
		DeleteEnhMetaFile(hemf); //������ ��������
	}

	EndPaint(hwnd, &ps); //��������� ��������
}

//=== ���������� ��������� WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}