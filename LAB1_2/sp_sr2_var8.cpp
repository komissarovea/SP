#include <Windows.h>
#include <tchar.h>

//-- Prototypes -------------------
LRESULT CALLBACK Sr_1_2_WndProc(HWND, UINT, WPARAM, LPARAM);

//-- Global Variables ------------
LPTSTR g_lpszClassName = TEXT("sp_sr2_class");
LPTSTR g_lpszAplicationTitle = TEXT("Разработчик: Комиссаров Евгений, 60322-2, СР 1-2, Вар. 8");
HINSTANCE g_hInst;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpszCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hWnd;
	MSG msg;

	g_hInst = hInstance;

	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.lpszClassName = g_lpszClassName;
	wc.lpfnWndProc = Sr_1_2_WndProc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	wc.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_SHIELD));
	wc.hCursor = LoadCursor(NULL, IDC_HAND);
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindowEx(NULL, g_lpszClassName,
		g_lpszAplicationTitle,
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		600,
		400,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL, TEXT("Окно не создано!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK Sr_1_2_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	static HWND hButtonExit;

	switch (msg)
	{
	case WM_CREATE:
	{
		hButtonExit = CreateWindowEx(WS_EX_LEFT, TEXT("Button"), TEXT("Выход"),
			WS_CHILD | WS_BORDER | WS_VISIBLE, 10, 10, 80, 24,
			hWnd, (HMENU)(IDCANCEL), g_hInst, NULL);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT paintStruct;
		int x = 10;
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		TCHAR buffer[100] = { 0 };
		wsprintf(buffer, TEXT("(%d, %d) (%d, %d)"),
			clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);
		HDC hdc = BeginPaint(hWnd, &paintStruct);
		clientRect.left = clientRect.top = x;
		//DrawText(hdc, TEXT("Координаты клиентской области:"), -1, &clientRect, 
		//	DT_LEFT | DT_TOP | DT_WORDBREAK);
		DrawText(hdc, buffer, -1, &clientRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hWnd, &paintStruct);
		break;
	}
	case WM_COMMAND:
	{
		wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDCANCEL:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}
	case WM_DESTROY:
		PostQuitMessage(10);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}