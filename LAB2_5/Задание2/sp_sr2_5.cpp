// sp_sr2_5.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "resource.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
DWORD  dwFileSize; // размер файла
HANDLE hFile; // дескриптор файла
HANDLE hFileMap; // дескриптор проецируемого файла
LPVOID lpvFile; // адрес региона в адресном пространстве, куда будет отображаться файл
HWND hwndEdit, hwndBtnRead, hwndBtnWrite;
LPCTSTR lpszMapFileName = TEXT("MAP_FILE");

#define IDC_EDIT1 100
#define IDC_BTN_COPY 101
#define IDC_BTN_PASTE 102

// Forward declarations of functions included in this code module:
ATOM                RegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void WndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void WndProc_OnDestroy(HWND hwnd);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{
	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32TEMPLATE, szWindowClass, MAX_LOADSTRING);

	BOOL isRegistered = RegisterClass(hInstance);
	if (!isRegistered) {
		MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		MessageBox(NULL, TEXT("Окно не создано!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32TEMPLATE));
	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32TEMPLATE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32TEMPLATE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 600, 400, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_CREATE, WndProc_OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, WndProc_OnCommand);
		HANDLE_MSG(hWnd, WM_DESTROY, WndProc_OnDestroy);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//=== Обработчик сообщения WM_CREATE ================================//
BOOL WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,
		NULL, PAGE_READWRITE, 0, 4 * 1024, lpszMapFileName);
	if (hFileMap != NULL)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			hFileMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
				FALSE, lpszMapFileName);
		}
		if (hFileMap != NULL)
		{
			// Создание проецируемого файла завершилось успешно
			// Проецируем оконное представление файла
			// на адресное пространство
			lpvFile = MapViewOfFile(hFileMap,
				FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 4 * 1024);
			if ((BYTE *)lpvFile == NULL)
				MessageBox(hWnd, TEXT("Can't map view of file. "), NULL, MB_OK);
		}
	}
	else
		MessageBox(hWnd, TEXT("Can't create file mapping."), NULL, MB_OK);
	hwndEdit = CreateWindow(
		TEXT("edit"), TEXT("Введите текст"),
		WS_CHILD | WS_VISIBLE |
		WS_BORDER | ES_LEFT | ES_MULTILINE |
		ES_AUTOHSCROLL | ES_AUTOVSCROLL,
		5, 5, 575, 280,
		hWnd,/* Parent is this window. */
		(HMENU)IDC_EDIT1,
		lpCreateStruct->hInstance,
		NULL
	);
	hwndBtnRead = CreateWindow(
		TEXT("button"), TEXT("Запись"),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		370, 300, 100, 30,
		hWnd,/* Parent is this window. */
		(HMENU)IDC_BTN_COPY,
		lpCreateStruct->hInstance,
		NULL
	);
	hwndBtnWrite = CreateWindow(
		TEXT("button"), TEXT("Чтение"),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		480, 300, 100, 30,
		hWnd,/* Parent is this window. */
		(HMENU)IDC_BTN_PASTE,
		lpCreateStruct->hInstance,
		NULL
	);
	return TRUE;
}

//=== Обработчик сообщения WM_COMMAND ================================//
void WndProc_OnCommand(HWND hWnd, int wmId, HWND hwndCtl, UINT codeNotify)
{
	// Parse the menu selections:
	switch (wmId)
	{
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
	case IDC_BTN_COPY:
		TCHAR buff[1024];
		GetWindowText(hwndEdit, (LPTSTR)lpvFile, 
			GetWindowTextLength(hwndEdit) + 1);
		break;
	case IDC_BTN_PASTE:
		SetWindowText(hwndEdit, (LPTSTR)lpvFile);
		break;
	default:
		FORWARD_WM_COMMAND(hWnd, wmId, hwndCtl, codeNotify, DefWindowProc);
		break;
	}
}

//=== Обработчик сообщения WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hwnd)
{
	UnmapViewOfFile((LPVOID)lpvFile);
	CloseHandle(hFileMap);
	PostQuitMessage(0);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
