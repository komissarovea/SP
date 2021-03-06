// Win32Template.cpp : Defines the entry point for the application.
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

UINT g_uThCount = 0; // счетчик потоков
HANDLE g_hSecThread[4] = { 0,0,0,0 }; // массив дескрипторов потоков
DWORD g_dwSecThreadId[4] = { 0,0,0,0 }; // массив идентификаторов потоков
bool g_dwThreadSuspendStatus[4] = { false }; // флаги состояний процессов
LPTSTR szThName[4] = { L"Первичный поток:", L"Поток 1:", L"Поток 2:", L"Поток самостоятельной работы:" };
UINT g_uXPos = 50, g_uYPos = 20, g_uYPos2 = 50;

struct THREAD_PARAM // структура с параметрами для функций потоков
{
	int Num;       // Номер потока
	UINT XPos; // Позиция X области вывода
	UINT YPos; // Позиция Y области вывода
	HWND hWnd; // Дескиптор окна вывода
};

// заполним параметры
THREAD_PARAM ThrParam1 = { 1, g_uXPos, g_uYPos, 0 };
THREAD_PARAM ThrParam2 = { 2, g_uXPos, g_uYPos2, 0 };
THREAD_PARAM ThrParam3 = { 3, g_uXPos, g_uYPos, 0 };

// Forward declarations of functions included in this code module:
ATOM RegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void WndProc_OnPaint(HWND hwnd);
void WndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void WndProc_OnDestroy(HWND hwnd);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK InfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

DWORD WINAPI ThreadFunc(PVOID pvParam); // прототип функции потока
DWORD WINAPI SamostThreadFunc(PVOID pvParam); // прототип функции для самостоятельной работы
void ErrorView(HWND hWnd);

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

	ThrParam1.hWnd = hWnd;
	ThrParam2.hWnd = hWnd;
	ThrParam3.hWnd = hWnd;

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
		HANDLE_MSG(hWnd, WM_PAINT, WndProc_OnPaint);
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
	g_hSecThread[0] = GetCurrentThread(); //запишем дескриптор текущего потока
	g_dwSecThreadId[0] = GetCurrentThreadId(); //запишем идентификатор текущего потока
	return TRUE;
}

//=== Обработчик сообщения WM_PAINT ==================================//
void WndProc_OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	// TODO: Add any drawing code that uses hdc here...
	EndPaint(hWnd, &ps);
}

//=== Обработчик сообщения WM_COMMAND ================================//
void WndProc_OnCommand(HWND hWnd, int wmId, HWND hwndCtl, UINT codeNotify)
{
	HMENU hMenu = GetMenu(hWnd);
	// Parse the menu selections:
	switch (wmId)
	{
#pragma region IDM_CreateThread1
	case IDM_CreateThread1:
	{
		//создаем поток
		g_hSecThread[1] = CreateThread(NULL, NULL,
			ThreadFunc, &ThrParam1, 0, &g_dwSecThreadId[1]);
		if (g_hSecThread[1])
		{
			g_uThCount++;
			//ограничиваем и/или активируем некоторые пункты меню
			EnableMenuItem(hMenu, IDM_CreateThread1, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_CreateWaitingThread1, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_ResumeThread1, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_SuspendThread1, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_TerminateThread1, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_HighThread1, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_LowThread1, MF_ENABLED);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_CreateThread2
	case IDM_CreateThread2:
	{
		g_hSecThread[2] = CreateThread(NULL, NULL,
			ThreadFunc, &ThrParam2, 0, &g_dwSecThreadId[2]);
		if (g_hSecThread[2])
		{
			g_uThCount++;
			EnableMenuItem(hMenu, IDM_CreateThread2, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_CreateWaitingThread2, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_ResumeThread2, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_SuspendThread2, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_TerminateThread2, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_HighThread2, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_LowThread2, MF_ENABLED);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_CreateWaitingThread1
	case IDM_CreateWaitingThread1:
	{
		g_hSecThread[1] = CreateThread(NULL, NULL,
			ThreadFunc, &ThrParam1, CREATE_SUSPENDED, // для ожидания потока
			&g_dwSecThreadId[1]);
		if (g_hSecThread[1])
		{
			g_uThCount++;
			g_dwThreadSuspendStatus[1] = true;
			EnableMenuItem(hMenu, IDM_CreateThread1, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_CreateWaitingThread1, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_ResumeThread1, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_SuspendThread1, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_TerminateThread1, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_HighThread1, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_LowThread1, MF_ENABLED);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_CreateWaitingThread2
	case IDM_CreateWaitingThread2:
	{
		g_hSecThread[2] = CreateThread(NULL, NULL,
			ThreadFunc, &ThrParam2, CREATE_SUSPENDED, // для ожидания потока
			&g_dwSecThreadId[2]);
		if (g_hSecThread[2])
		{
			g_uThCount++;
			g_dwThreadSuspendStatus[2] = true;
			EnableMenuItem(hMenu, IDM_CreateThread2, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_CreateWaitingThread2, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_ResumeThread2, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_SuspendThread2, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_TerminateThread2, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_HighThread2, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_LowThread2, MF_ENABLED);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_SuspendThread1
	case IDM_SuspendThread1:
	{
		if (SuspendThread(g_hSecThread[1]) != 0xFFFFFFFF)
		{
			g_dwThreadSuspendStatus[1] = true;
			EnableMenuItem(hMenu, IDM_SuspendThread1, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_ResumeThread1, MF_ENABLED);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_SuspendThread2
	case IDM_SuspendThread2:
	{
		if (SuspendThread(g_hSecThread[2]) != 0xFFFFFFFF)
		{
			g_dwThreadSuspendStatus[1] = true;
			EnableMenuItem(hMenu, IDM_SuspendThread2, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_ResumeThread2, MF_ENABLED);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_ResumeThread1
	case IDM_ResumeThread1:
	{
		if (ResumeThread(g_hSecThread[1]) != 0xFFFFFFFF)
		{
			g_dwThreadSuspendStatus[1] = false;
			EnableMenuItem(hMenu, IDM_SuspendThread1, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_ResumeThread1, MF_GRAYED);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_ResumeThread2
	case IDM_ResumeThread2:
	{
		if (ResumeThread(g_hSecThread[2]) != 0xFFFFFFFF)
		{
			g_dwThreadSuspendStatus[2] = false;
			EnableMenuItem(hMenu, IDM_SuspendThread2, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_ResumeThread2, MF_GRAYED);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_TerminateThread1
	case IDM_TerminateThread1:
	{
		if (TerminateThread(g_hSecThread[1], 10))
		{
			g_uThCount--;
			g_dwThreadSuspendStatus[1] = false;
			RECT rc;
			GetClientRect(hWnd, &rc);
			rc.top = ThrParam1.YPos;
			rc.left = ThrParam1.XPos;
			InvalidateRect(hWnd, &rc, TRUE);
			EnableMenuItem(hMenu, IDM_CreateThread1, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_CreateWaitingThread1, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_ResumeThread1, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_SuspendThread1, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_TerminateThread1, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_HighThread1, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_LowThread1, MF_GRAYED);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_TerminateThread2
	case IDM_TerminateThread2:
	{
		if (TerminateThread(g_hSecThread[2], 10))
		{
			g_uThCount--;
			g_dwThreadSuspendStatus[2] = false;
			RECT rc;
			GetClientRect(hWnd, &rc);
			rc.top = ThrParam2.YPos;
			rc.left = ThrParam2.XPos;
			InvalidateRect(hWnd, &rc, TRUE);
			EnableMenuItem(hMenu, IDM_CreateThread2, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_CreateWaitingThread2, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_ResumeThread2, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_SuspendThread2, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_TerminateThread2, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_HighThread2, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_LowThread2, MF_GRAYED);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_HighThread1
	case IDM_HighThread1:
	{
		int priority = GetThreadPriority(g_hSecThread[1]);
		if (priority != THREAD_PRIORITY_ERROR_RETURN)
		{
			if (priority < THREAD_PRIORITY_HIGHEST)
				SetThreadPriority(g_hSecThread[1], priority + 1);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_HighThread2
	case IDM_HighThread2:
	{
		int priority = GetThreadPriority(g_hSecThread[2]);
		if (priority != THREAD_PRIORITY_ERROR_RETURN)
		{
			if (priority < THREAD_PRIORITY_HIGHEST)
				SetThreadPriority(g_hSecThread[2], priority + 1);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_LowThread1
	case IDM_LowThread1:
	{
		int priority = GetThreadPriority(g_hSecThread[1]);
		if (priority != THREAD_PRIORITY_ERROR_RETURN)
		{
			if (priority > THREAD_PRIORITY_LOWEST)
				SetThreadPriority(g_hSecThread[1], priority - 1);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
#pragma region IDM_LowThread2
	case IDM_LowThread2:
	{
		int priority = GetThreadPriority(g_hSecThread[2]);
		if (priority != THREAD_PRIORITY_ERROR_RETURN)
		{
			if (priority > THREAD_PRIORITY_LOWEST)
				SetThreadPriority(g_hSecThread[2], priority - 1);
		}
		else
			ErrorView(hWnd);
		break;
	}
#pragma endregion
	case IDM_Samost_Thread:
		g_hSecThread[3] = CreateThread(
			NULL, NULL,	SamostThreadFunc,
			(PVOID)&ThrParam3, 0, &g_dwSecThreadId[3]);
		g_uThCount++;
		break;
	case IDM_PrimaryThreadInfo:
	{
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_THREAD_INFO), hWnd, InfoDlgProc, 0);
		break;
	}
	case IDM_Thread1Info:
	{
		DialogBoxParam(hInst,
			MAKEINTRESOURCE(IDD_THREAD_INFO), hWnd, InfoDlgProc, 1);
		break;
	}
	case IDM_Thread2Info:
	{
		DialogBoxParam(hInst,
			MAKEINTRESOURCE(IDD_THREAD_INFO), hWnd, InfoDlgProc, 2);
		break;
	}
	case IDM_Samost_Info:
	{
		DialogBoxParam(hInst,
			MAKEINTRESOURCE(IDD_THREAD_INFO), hWnd, InfoDlgProc, 3);
		break;
	}
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDlgProc);
		break;
	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
	default:
		FORWARD_WM_COMMAND(hWnd, wmId, hwndCtl, codeNotify, DefWindowProc);
		break;
	}
}



//=== Обработчик сообщения WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}

INT_PTR CALLBACK InfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		TCHAR buf[100];
		DWORD dwExitCode;

		FILETIME ftCreationTime; // время создания
		FILETIME ftExitTime; // время окончания
		FILETIME ftKernelTime; // время ядра
		FILETIME ftUserTime; // время пользователя
		FILETIME ftLifeTime; // время жизни
		FILETIME ftCurrTime; // текущее время		
		SYSTEMTIME stLifeTime; // время жизни(системное)
		SYSTEMTIME currSysTime; // текущее время (системное)
		SYSTEMTIME stTmpTime; // временная структура
							  // для работы и операций с FILETIME
		ULARGE_INTEGER ulCreationTime,
			ulExitTime,
			ulCurrentSystemTime,
			ulLifeTime;

		if (!GetExitCodeThread(g_hSecThread[lParam], &dwExitCode))
		{
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)L"Поток еще не создан");
		}
		else
		{
			wsprintf(buf, L"Вторичных потоков: %d", g_uThCount);
			SetDlgItemText(hDlg, IDC_STATIC_TXT2, buf);

			wsprintf(buf, L"%s", szThName[lParam]);
			SetDlgItemText(hDlg, IDC_STATIC_TXT, buf);

			wsprintf(buf, L"Идентификатор потока - %d", g_dwSecThreadId[lParam]);
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)buf);

			wsprintf(buf, L"Дескриптор потока - 0x%08X", g_hSecThread[lParam]);
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)buf);

			if (dwExitCode == STILL_ACTIVE && g_dwThreadSuspendStatus[lParam] && lParam != 0)
			{
				wsprintf(buf, L"%s", L"Состояние потока - Ожидает");
				SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)buf);
			}
			else if ((dwExitCode == STILL_ACTIVE))
			{
				wsprintf(buf, L"%s", L"Состояние потока - Активен");
				SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)buf);
			}
			else
			{
				wsprintf(buf, L"Поток завершен. Код завершения %d", dwExitCode);
				SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)buf);
			}

			wsprintf(buf, L"Относительный приоритет: %d", GetThreadPriority(g_hSecThread[lParam]));
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)buf);

			if (GetThreadTimes(
				g_hSecThread[lParam],
				&ftCreationTime,
				&ftExitTime,
				&ftKernelTime,
				&ftUserTime))
			{
				ulCreationTime.LowPart = ftCreationTime.dwLowDateTime;
				ulCreationTime.HighPart = ftCreationTime.dwHighDateTime;
				// при активном процессе продолжительность работы будем считать до текущего момента
				if (dwExitCode == STILL_ACTIVE)
				{
					GetSystemTime(&currSysTime); // текущие показатели времени
					SystemTimeToFileTime(&currSysTime, &ftCurrTime); // конвертируем
																	 // из Systemtime в ularge_integer
					ulCurrentSystemTime.LowPart = ftCurrTime.dwLowDateTime;
					ulCurrentSystemTime.HighPart = ftCurrTime.dwHighDateTime;
					//рассчитаем время жизни
					ulLifeTime.QuadPart = ulCurrentSystemTime.QuadPart - ulCreationTime.QuadPart;
				}
				//если процесс завершен
				else
				{
					ulExitTime.LowPart = ftExitTime.dwLowDateTime;
					ulExitTime.HighPart = ftExitTime.dwHighDateTime;
					ulLifeTime.QuadPart = ulExitTime.QuadPart - ulCreationTime.QuadPart;
				}
				//делаем обратные преобразования
				ftLifeTime.dwLowDateTime = ulLifeTime.LowPart;
				ftLifeTime.dwHighDateTime = ulLifeTime.HighPart;
				FileTimeToSystemTime(&ftLifeTime, &stLifeTime);
				//формируем строку времени для времени жизни процесса
				wsprintf(buf, L"Время работы потока: %d м %d с %d мс", stLifeTime.wMinute, stLifeTime.wSecond, stLifeTime.wMilliseconds);
				SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)buf);
			}
		}
		return TRUE;
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		}
	}
	return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

//функция потока
DWORD WINAPI ThreadFunc(PVOID pvParam)
{
	// Локальная переменная для хранения переданного параметра
	THREAD_PARAM * thrParam = (THREAD_PARAM *)pvParam;

#define MAXCHAR 270
	TCHAR creepingLine[MAXCHAR] = { 0 }; // Буфер для символов бегущей строки
	TCHAR buf[MAXCHAR] = { 0 }, bufCount[66] = { 0 }; // Рабочий буфер для циклического сдвига строки
	int   iBeginningIndex; // Индекс начала выводимой последовательности символов
	int   StringLength = 0; // Длина строки
	RECT  rc, rcCount;
	int cRun = 0; // Счетчик “пробегов”
	int N = 6; //  Количество “пробегов” в серии
	int thN = 0; //[] = { 0,0 }; //запоминаем, сколько "набегает" поток
				 // Формирование текста бегущей строки
	wsprintf(creepingLine, L"  Вторичный поток %d(%d) создал Комиссаров Е.А.   ",
		thrParam->Num, g_dwSecThreadId[thrParam->Num]);
	//  Длинна строки
	StringLength = iBeginningIndex = lstrlen(creepingLine);// -1;
	lstrcpy(buf, creepingLine);
	//  Задание прямоугольной области вывода
	GetClientRect(thrParam->hWnd, &rc);
	rc.top = thrParam->YPos;
	rc.left = thrParam->XPos;
	rcCount.top = thrParam->YPos + 70;
	rcCount.left = thrParam->XPos;
	//  Получение контекста для вывода строки		
	HDC hDC = GetDC(thrParam->hWnd);
	//  Бесконечный цикл вывода строк сериями по N строк 
	while (TRUE)
	{
		cRun = 0;
		while (cRun < N)// Цикл вывода серии из N строк
		{
			// Цикл однократного продвижения строки от последнего 
			// символа до первого (перемещение слева направо в области вывода)
			for (int j = 0; j < StringLength; j++)
			{
				if (iBeginningIndex == 0)
				{
					iBeginningIndex = StringLength;
					cRun++; // Подсчет количества полных пробегов строки
					thN++;
					wsprintf(bufCount, L"Поток %d, Полных строк %d", thrParam->Num, thN);
					DrawText(hDC, bufCount, -1, &rcCount, DT_LEFT | DT_SINGLELINE);
				}
				// Cдвиг символов в рабочем буфере на одну позицию
				TCHAR c = buf[StringLength];
				for (int i = StringLength; i > 0; i--)// Цикл сдвига
					buf[i] = buf[i - 1];
				buf[0] = c;
				// Вывод строки
				DrawText(hDC, buf, 100, &rc, DT_LEFT | DT_SINGLELINE);
				iBeginningIndex--;
				Sleep(50); // приостановка потока на 400 мсек - замедление цикла
			} // Конец цикла полного однократного “пробега” строки
		}// Конец цикла вывода серии строк 
	}
	return 0;
}

//функция для самостоятельной работы
DWORD WINAPI SamostThreadFunc(PVOID pvParam)
{
	THREAD_PARAM * ThrParam; // Локальная переменная для хранения переданного параметра
	ThrParam = (THREAD_PARAM*)pvParam;
	const int count = 5;
	HBITMAP hBmp[count];
	hBmp[0] = LoadBitmap((HINSTANCE)GetWindowLong(ThrParam->hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP1));
	hBmp[1] = LoadBitmap((HINSTANCE)GetWindowLong(ThrParam->hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP2));
	hBmp[2] = LoadBitmap((HINSTANCE)GetWindowLong(ThrParam->hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP9));
	hBmp[3] = LoadBitmap((HINSTANCE)GetWindowLong(ThrParam->hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP10));
	hBmp[4] = LoadBitmap((HINSTANCE)GetWindowLong(ThrParam->hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP11));
	//получим контекст
	HDC hDc = GetDC(ThrParam->hWnd);
	//Устанавливаем прозрачный фон
	SetBkMode(hDc, TRANSPARENT);
	//Создаем контекст в памяти, совместимый с контекстом 
	//устройства - в данном случае окна приложение
	HDC hMemDc = CreateCompatibleDC(hDc);

	while (true)
	{
		for (int index = 0; index < count; index++)
		{
			//выбираем картинку
			SelectObject(hMemDc, hBmp[index]);
			//Отрисовка изображения
			BitBlt(hDc, 0, 150, 370, 207, hMemDc, 0, 0, SRCCOPY);
			//приостановим для смены кадров
			Sleep(120);
		}
	}
	//освободим ненужное
	DeleteDC(hMemDc);
	for (int i = 0; i <= 5; i++)
		DeleteObject(hBmp[i]);
	DeleteDC(hDc);
	return 0;
}

// функция для обработки ошибок
void ErrorView(HWND hWnd)
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR FullMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), sysMsg, 256, NULL);
	p = sysMsg;
	while ((*p > 31) || (*p == 9))
		++p;
	do {
		*p-- = 0;
	} while ((p >= sysMsg) && ((*p == '.') || (*p < 33)));
	wsprintf(FullMsg, L"\nError %d (%s)", eNum, sysMsg);
	MessageBox(NULL, FullMsg, L"Error", MB_OK | MB_ICONERROR);
}