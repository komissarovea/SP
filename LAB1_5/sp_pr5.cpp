#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <string>
#include "resource.h"

//-- Prototypes -------------------
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK LoadDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GraphicDlgProc(HWND, UINT, WPARAM, LPARAM);

//-- Global Variables ------------
LPTSTR g_lpszClassName = TEXT("sp_pr5_class");
LPTSTR g_lpszAplicationTitle = TEXT("Разработчик: Комиссаров Евгений, 60322-2");

HWND hDlgModeless = NULL;

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
	wc.lpfnWndProc = WndProc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.hbrBackground = CreateSolidBrush(RGB(127, 127, 127));
	wc.hIcon = LoadIcon(NULL, IDI_SHIELD);
	wc.hCursor = LoadCursor(NULL, IDC_HAND);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return 0;
	}

	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	hWnd = CreateWindowEx(NULL, g_lpszClassName,
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
		if (hDlgModeless == NULL || !IsDialogMessage(hDlgModeless, &msg))
		{
			if (!TranslateAccelerator(hWnd, hAccel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	return msg.wParam;
}

#pragma region WndProc

BOOL WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	return TRUE;
}

void WndProc_OnPaint(HWND hwnd)
{
	FORWARD_WM_PAINT(hwnd, DefWindowProc);
}

void WndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDM_ABOUT:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)AboutDlgProc);
		break;
	case IDM_VIEW_TEXT:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_LOAD_TEXT_DIALOG), hwnd, (DLGPROC)LoadDlgProc);
		break;
	case IDM_VIEW_BMP:
		if (!hDlgModeless)
		{
			hDlgModeless = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_GRAPHIC_DIALOG),
				hwnd, (DLGPROC)GraphicDlgProc);
			ShowWindow(hDlgModeless, SW_SHOW);

			HDC hDC;
			HDC hMemDC;
			HANDLE hBmp;
			HANDLE hOldBmp;
			BITMAP Bmp;
			hBmp = LoadImage(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1),
				IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);

			hDC = GetDC(hDlgModeless);
			HDC hCompDC = CreateCompatibleDC(hDC);

			GetObject(hBmp, sizeof(BITMAP), &Bmp);
			hOldBmp = SelectObject(hCompDC, hBmp);
			RECT Rect;
			GetClientRect(hDlgModeless, &Rect);
			StretchBlt(hDC, 0, 0, Rect.right, Rect.bottom,
				hCompDC, 0, 0, Bmp.bmWidth, Bmp.bmHeight, SRCCOPY);
			SelectObject(hCompDC, hOldBmp);
			DeleteDC(hCompDC);
			DeleteObject(hBmp);
			ReleaseDC(hDlgModeless, hDC);
		}
		break;
	case IDM_FILE_EXIT:
		DestroyWindow(hwnd);
		break;
	default:
		if (id > 40000)
		{
			TCHAR buffer[200];
			TCHAR itemText[100];
			int size = LoadString(g_hInst, id, itemText, 100);
			wsprintf(buffer, TEXT("Выбрана команда \"%s\""), itemText);
			MessageBox(hwnd, buffer, TEXT("Сообщение"), MB_OK);
		}
		FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, DefWindowProc);
		break;
	}
}

void WndProc_OnDestroy(HWND hWnd)
{
	DestroyWindow(hDlgModeless);
	PostQuitMessage(0);
	FORWARD_WM_DESTROY(hWnd, DefWindowProc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, WndProc_OnCreate);
		HANDLE_MSG(hwnd, WM_COMMAND, WndProc_OnCommand);
		HANDLE_MSG(hwnd, WM_PAINT, WndProc_OnPaint);
		HANDLE_MSG(hwnd, WM_DESTROY, WndProc_OnDestroy);
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

#pragma endregion

#pragma region AboutDlgProc

BOOL AboutDlgProc_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	HWND hText3 = GetDlgItem(hwnd, IDC_TEXT3);
	SYSTEMTIME st;
	GetLocalTime(&st);
	TCHAR timeText[200];
	wsprintf(timeText, TEXT("Текущие дата и время: %d-%d-%d %d:%d"),
		st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
	SetWindowText(hText3, timeText);
	return TRUE;
}

void AboutDlgProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
		EndDialog(hwnd, IDOK);
		break;
	case IDCANCEL:
		EndDialog(hwnd, IDCANCEL);
		break;
	default:
		FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, DefWindowProc);
		break;
	}
}

LRESULT CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_INITDIALOG, AboutDlgProc_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, AboutDlgProc_OnCommand);
	}
	return FALSE;
}

#pragma endregion

#pragma region LoadDlgProc

BOOL LoadDlgProc_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	HWND hEditText = GetDlgItem(hwnd, IDC_EDIT_TEXT);
	SetWindowText(hEditText, TEXT("Шаг 1. Начало работы"));
	HWND hBtnLoad = GetDlgItem(hwnd, IDC_BTN_LOAD);
	SetFocus(hBtnLoad);
	//SendMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)btnLoad, TRUE);
	return FALSE;
}

void LoadDlgProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDC_BTN_LOAD:
	{
		OPENFILENAME ofn;   // структура для common dialog box
		TCHAR lpszFileSpec[260];   // массив для имени файла
		HANDLE hFile;       // дескриптор файла
#define MAX_BYTES  10000
		DWORD dwNumbOfBytes = MAX_BYTES;
		CHAR mbBuffer[MAX_BYTES]; // буфер для текста
		//Иницализация OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;  // hwnd – дескриптор окна–влвдельца
		ofn.lpstrFile = lpszFileSpec;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(lpszFileSpec);
		// Формирование массива строк шаблонов фильтра
		ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
		ofn.nFilterIndex = 1; // Индекс для текущего шаблона фильтра
		ofn.lpstrFileTitle = NULL; // Без заголовка
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL; // В качестве начального текущий каталог
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Отображение диалогового окна
		BOOL fRet = GetOpenFileName(&ofn);
		if (fRet == FALSE)
			MessageBox(hwnd, TEXT("Ошибка в диалоге"), NULL, MB_OK);
		else
		{
			hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

			//Проверка корректности открытия файла
			if (hFile == INVALID_HANDLE_VALUE)
				MessageBox(hwnd, TEXT("Ошибка при открытии файла"), NULL, MB_OK);
			else
			{
				//Чтение из файла в буфер
				ReadFile(hFile, mbBuffer, 100, &dwNumbOfBytes, NULL);
				TCHAR wideBuffer[MAX_BYTES];
				// multibyte into wide characters
				_locale_t locale = _wcreate_locale(LC_ALL, TEXT("Russian"));
				_mbstowcs_s_l(0, wideBuffer, MAX_BYTES, mbBuffer, dwNumbOfBytes, locale);
				HWND hEditText = GetDlgItem(hwnd, IDC_EDIT_TEXT);
				Edit_SetText(hEditText, wideBuffer);
				CloseHandle(hFile);
			}
		}
		break;
	}
	case IDOK:
		EndDialog(hwnd, IDOK);
		break;
	case IDCANCEL:
		EndDialog(hwnd, IDCANCEL);
		break;
	default:
		FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, DefWindowProc);
		break;
	}
}

LRESULT CALLBACK LoadDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_INITDIALOG, LoadDlgProc_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, LoadDlgProc_OnCommand);
	}
	return FALSE;
}

#pragma endregion

#pragma region GraphicDlgProc

BOOL GraphicDlgProc_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	return TRUE;
}

LRESULT CALLBACK GraphicDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_INITDIALOG, GraphicDlgProc_OnInitDialog);
	case WM_CLOSE:
		DestroyWindow(hDlgModeless);
		break;
	case WM_DESTROY:
		hDlgModeless = NULL;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

#pragma endregion
