#include <windows.h>
#include <tchar.h>

//-- Prototypes -------------------
LRESULT CALLBACK SimWndProc(HWND, UINT, WPARAM, LPARAM);

//-- Global Variables ------------

//  Стартовая функция
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpszCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	MSG msg;
	HWND hWnd;

	//memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpszClassName = TEXT("SimpleClassName");
	wc.lpfnWndProc = SimWndProc;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.hInstance = hInstance;
	//wc.hIcon = wc.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hIcon = wc.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ASTERISK));
	//wc.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	wc.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));
	//wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wc.hbrBackground = CreateSolidBrush(RGB(260, 110, 50));
	wc.hbrBackground = (HBRUSH)(COLOR_ACTIVECAPTION + 1);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hWnd = CreateWindowEx(NULL, TEXT("SimpleClassName"),
		TEXT("Simple Application with Message handling"),
		WS_OVERLAPPEDWINDOW,
		700,
		400,
		500, //CW_USEDEFAULT,
		300, //CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL, TEXT("Окно не создано!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

// Оконная процедура
LRESULT CALLBACK SimWndProc(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	LPCTSTR text = TEXT("Комиссаров, 60322-2");

	switch (msg)
	{
	case WM_PAINT:    // Вывод при обновлении окна
		PAINTSTRUCT ps;
		hDC = BeginPaint(hWnd, &ps); // Получение контекста для
									 // обновления окна 
		TextOut(hDC, 20, 20, text, lstrlen(text)); // Вывод в контекст
		EndPaint(hWnd, &ps); // Завершение обновления окна
		break;
	case WM_DESTROY:  // Завершение работы приложения
		PostQuitMessage(0); // Посылка WM_QUIT приложению
		break;
	default: // Вызов "Обработчика по умолчанию"
		return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return FALSE;// Для ветвей с "break"
}