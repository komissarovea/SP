#include <Windows.h>
#include <tchar.h>
#include "resource.h"

//-- Prototypes -------------------
LRESULT CALLBACK Pr3_WndProc(HWND, UINT, WPARAM, LPARAM);

//-- Global Variables ------------
LPTSTR g_lpszClassName = TEXT("sp_pr3_class");
LPTSTR g_lpszAplicationTitle = TEXT("Разработчик: Комиссаров Евгений, 60322-2");
LPTSTR g_lpszDestroyMessage = TEXT("Данный вывод выполнен в обработчике сообщения WM_DESTROY. Сообщение поступило от Windows в связи с разрушением окна.");
LPTSTR g_lpszLButtonDownMessage = TEXT("Обработка сообщения WM_LBUTTONDOWN, которое посылается в окно при щелчке левой кнопки мыши");
LPTSTR g_lpszPaintMessage = TEXT("Вывод текста при обработке сообщения WM_PAINT. Это соообщение окно получает после того, как оно было закрыто другим окном и затем открыто.");
LPTSTR g_lpszCreateMessage = TEXT("Выполняется обработка WM_CREATE");
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
	wc.lpfnWndProc = Pr3_WndProc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.hbrBackground = CreateSolidBrush(RGB(127, 127, 127));
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return 0;
	}

	//Размещение и размер окна приложения согласно варианту №10
	int N = 10;
	int x = (10 + N) * 10;
	int y = (5 + N) * 10;
	int width = (40 + N) * 10;
	int height = (40 - N) * 10;

	HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	hWnd = CreateWindowEx(NULL, g_lpszClassName,
		g_lpszAplicationTitle,
		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX,
		//WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX,
		x,
		y,
		width,
		height,
		NULL,
		hMenu,
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
		if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

BOOL SetMenuItem(HMENU hMenu, UINT ulns, UINT fState, BOOL flag)
{
	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE | MIIM_ID;
	mii.fState = fState;
	mii.wID = ulns;
	return SetMenuItemInfo(hMenu, ulns, flag, &mii);
}


LRESULT CALLBACK Pr3_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//HINSTANCE g_hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	int wmId, wmEvent;

	static HWND hButtonSave;
	static HWND hButtonAdd;
	static HWND hButtonExit;
	static HWND hEdit;
	static HWND hListBox;

#define IDC_BTN_SAVE 100
#define IDC_BTN_ADD 101
#define IDC_EDIT1 102
#define IDC_LISTBOX 103
#define IDM_CLOSEDOC 40019

	switch (msg)
	{
	case WM_CREATE:
	{
		hButtonSave = CreateWindowEx(WS_EX_STATICEDGE, TEXT("Button"), TEXT("В буффер"),
			WS_CHILD | WS_BORDER | WS_VISIBLE, 20, 220, 80, 24,
			hWnd, (HMENU)(IDC_BTN_SAVE), g_hInst, NULL);
		hButtonAdd = CreateWindowEx(WS_EX_RIGHT, TEXT("Button"), TEXT("В список"),
			WS_CHILD | WS_BORDER | WS_VISIBLE, 120, 220, 80, 24,
			hWnd, (HMENU)(IDC_BTN_ADD), g_hInst, NULL);
		hButtonExit = CreateWindowEx(WS_EX_WINDOWEDGE, TEXT("Button"), TEXT("Выход"),
			WS_CHILD | WS_VISIBLE, 220, 220, 80, 24,
			hWnd, (HMENU)(IDCANCEL), g_hInst, NULL);
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("Редактор"),
			WS_CHILD | WS_VISIBLE | WS_VSCROLL, 20, 50, 160, 80,
			hWnd, (HMENU)(IDC_EDIT1), g_hInst, NULL);
		hListBox = CreateWindowEx(WS_EX_DLGMODALFRAME, TEXT("ListBox"), TEXT("Список"),
			WS_CHILD | WS_BORDER | WS_VISIBLE, 200, 50, 160, 160,
			hWnd, (HMENU)(IDC_LISTBOX), g_hInst, NULL);

		if (!hButtonSave || !hButtonAdd || !hButtonExit || !hEdit || !hListBox)
			return -1;

		HMENU hMenu = GetMenu(hWnd);
		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		LPTSTR miText = TEXT("Закрыть документ");
		MENUITEMINFO miCloseDocument;
		miCloseDocument.cbSize = sizeof(miCloseDocument);
		miCloseDocument.fMask = MIIM_STRING | MIIM_ID;
		/*	miCloseDocument.fType = MFT_STRING;
		miCloseDocument.fState = MFS_ENABLED;
		miCloseDocument.hSubMenu = NULL;
		miCloseDocument.hbmpChecked = NULL;
		miCloseDocument.hbmpUnchecked = NULL;
		miCloseDocument.hbmpItem = NULL;
		miCloseDocument.dwItemData = 40020;	*/
		miCloseDocument.wID = IDM_CLOSEDOC;
		miCloseDocument.dwTypeData = miText;
		miCloseDocument.cch = lstrlen(miText);
		//bool result = InsertMenuItem(hmenu, IDM_EXIT, false, &miCloseDocument);
		bool result = InsertMenuItem(hSubMenu, 2, true, &miCloseDocument);
		if (result)
			DrawMenuBar(hWnd);

		//MessageBox(hWnd, g_lpszCreateMessage, TEXT("Сообщение"), MB_OK | MB_ICONINFORMATION);
		//return -1;
		break;
	}
	case WM_CONTEXTMENU:
	{
		HMENU hMenu = GetMenu(hWnd);
		UINT selectState = GetMenuState(hMenu, IDM_SELECT, MF_BYCOMMAND);
		UINT copyState = GetMenuState(hMenu, IDM_COPY, MF_BYCOMMAND);
		HMENU hPopupMenu = CreatePopupMenu();
		AppendMenu(hPopupMenu, MF_STRING | selectState, IDM_SELECT, TEXT("Выделить"));
		AppendMenu(hPopupMenu, MF_STRING | copyState, IDM_COPY, TEXT("Копировать"));
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		bool result = TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN, x, y, 0, hWnd, NULL);
		break;
	}
	case WM_COMMAND:
	{
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		static TCHAR pszTextBuff[500];
		switch (wmId)
		{
		case IDCANCEL:
			DestroyWindow(hWnd);
			break;
		case IDC_BTN_SAVE:
		{
			int cch = SendMessage(hEdit, WM_GETTEXT, 500, (LPARAM)pszTextBuff);
			if (cch == 0)
				MessageBox(hWnd, TEXT("Введите текст"), TEXT("Читаем Edit"), MB_OK);
			else {
				TCHAR Buff1[500] = { 0 };
				SYSTEMTIME st; GetSystemTime(&st);
				wsprintf(Buff1, TEXT("Время : %d ч %d мин %d сек\n"),
					st.wHour, st.wMinute, st.wSecond);
				lstrcat(Buff1, __TEXT("Текст в памяти: "));
				lstrcat(Buff1, pszTextBuff);
				MessageBox(hWnd, Buff1, TEXT("Содержимое буфера"), MB_OK);
			}
		}
		break;
		case IDC_BTN_ADD:
		{
			int ind = SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pszTextBuff);
			if (ind == LB_ERR)
				MessageBox(hWnd, TEXT("Ошибка в списке"), TEXT("Сообщение"), MB_OK);
		}
		break;
		case IDM_FCREATE:
		{
			MessageBox(hWnd, TEXT("Выбран пункт 'Создать'"), TEXT("Меню 'Файл'"), MB_OK);
			HMENU hMenu = GetMenu(hWnd);
			SetMenuItem(hMenu, IDM_SELECT, MFS_ENABLED, false);
			break;
		}
		case IDM_SELECT:
		{
			HMENU hMenu = GetMenu(hWnd);
			SetMenuItem(hMenu, IDM_COPY, MFS_ENABLED, false);
			/*HMENU hSubMenu = GetSubMenu(hMenu, 1);
			SetMenuItem(hSubMenu, 2, MFS_ENABLED, true);*/
			break;
		}
		case IDM_CLOSEDOC:
		{
			HMENU hMenu = GetMenu(hWnd);
			SetMenuItem(hMenu, IDM_SELECT, MFS_DISABLED, false);
			SetMenuItem(hMenu, IDM_CUT, MFS_DISABLED, false);
			SetMenuItem(hMenu, IDM_COPY, MFS_DISABLED, false);
			SetMenuItem(hMenu, IDM_PASTE, MFS_DISABLED, false);
			break;
		}
		case IDM_FOPEN:
			MessageBox(hWnd, TEXT("Выбран пункт 'Открыть'"), TEXT("Меню 'Файл'"), MB_OK);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_HELP:
			MessageBox(hWnd, TEXT("Выбран пункт 'Помощь'"), TEXT("Меню 'Справка'"), MB_OK);
			break;
		case IDM_ABOUT:
			MessageBox(hWnd, TEXT("Выбран пункт 'О программе...'"), TEXT("Меню 'Справка'"), MB_OK);
			break;
		default:
			if (wmId > 40000)
			{
				TCHAR buffer[100] = { 0 };
				wsprintf(buffer, TEXT("Команда  с идентификатором %d не реализована"), wmId);
				MessageBox(hWnd, buffer, TEXT("Сообщение"), MB_OK);
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;
	}
	case WM_MENUSELECT:
	{
		HDC hdc1;
		LPTSTR lpszMsgSpace = TEXT("____________________________");
		TCHAR buffer[300];
		HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		int size = LoadString(hInst, LOWORD(wParam), buffer, 300);
		hdc1 = GetDC(hWnd);
		RECT rc;
		GetClientRect(hWnd, &rc);
		TextOut(hdc1, rc.left + 10, rc.bottom - 40, lpszMsgSpace, lstrlen(lpszMsgSpace));
		TextOut(hdc1, rc.left + 10, rc.bottom - 40, buffer, lstrlen(buffer));
		ReleaseDC(hWnd, hdc1);
		break;
	}
	case WM_PAINT:
	{
		int x = 20; int y = 100;
		PAINTSTRUCT paintStruct;
		HDC hdc = BeginPaint(hWnd, &paintStruct);
		//TextOut(hdc, x, y, g_lpszPaintMessage, lstrlen(g_lpszPaintMessage));
		EndPaint(hWnd, &paintStruct);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		HDC hdc = GetDC(hWnd);
		//int x = 50; int y = 200;
		//TextOut(hdc, x, y, g_lpszLButtonDownMessage, lstrlen(g_lpszLButtonDownMessage));
		int x = LOWORD(lParam); // GET_X_LPARAM // #include <windowsx.h>
		int y = HIWORD(lParam); // GET_Y_LPARAM
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		RECT rect;
		rect.left = x;
		rect.top = y;
		rect.bottom = clientRect.bottom;
		rect.right = clientRect.right;
		DrawText(hdc, g_lpszLButtonDownMessage, -1, &rect, DT_LEFT | DT_TOP | DT_WORDBREAK);
		ReleaseDC(hWnd, hdc);
		break;
	}
	case WM_DESTROY:
		//MessageBox(hWnd, g_lpszDestroyMessage, TEXT("Сообщение"), MB_OK | MB_ICONINFORMATION);
		PostQuitMessage(10);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}