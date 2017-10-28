#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <locale.h>
#include "resource.h"

#define MAX_BYTES  10000
#define IDC_EDIT_SAM 222

#pragma region ���������� ����������
LPTSTR g_lpszClassName = TEXT("sp_lb21_class");
LPTSTR g_lpszAplicationTitle = TEXT("SP-SR2-1, ���������� �.�., 60322-2");
HINSTANCE g_hInst;

HANDLE ProcHandle[4];
DWORD ProcId[4];
HANDLE ThreadHandle[4];
DWORD ThreadId[4];
LPTSTR ProcImage[4];
TCHAR CmdParam[4][260];
#pragma endregion

#pragma region ��������������� ���������� �������

/* ����������� ������ �������� ���� */
BOOL Register(HINSTANCE);
/* �������� �������� ���� */
HWND Create(HINSTANCE, int);
/*  */
BOOL GetPriorityText(DWORD priority, TCHAR buffer[256]);
/*  */
FILETIME SumFileTime(const FILETIME ft1, const FILETIME ft2, BOOL minus);

#pragma region WndProc
/* ��������� �������� ���� */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
/*--- WM_CREATE ------------------------------------------------------*/
BOOL WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
/*--- WM_COMMAND -----------------------------------------------------*/
void WndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
/*--- WM_DESTROY -----------------------------------------------------*/
void WndProc_OnDestroy(HWND hwnd);
#pragma endregion

#pragma region DlgProc
/* ��������� ���������� ����������� ���� */
LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
/*--- WM_INITDIALOG---------------------------------------------------*/
BOOL DlgProc_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
/*--- WM_COMMAND -----------------------------------------------------*/
void DlgProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
#pragma endregion

#pragma endregion

#pragma region ����� ����� � ���������
// ����� ����� � ��������� ===========================================//
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpszCmdLine, int nCmdShow)
{
	ProcImage[0] = TEXT("SP_LB2-1.exe");
	ProcImage[1] = TEXT("C:\\Windows\\System32\\notepad.exe");
	ProcImage[2] = TEXT("C:\\Windows\\System32\\notepad.exe");
	ProcImage[3] = TEXT("C:\\Windows\\System32\\calc.exe");

	TCHAR CmdParam0[] = TEXT("param0");
	TCHAR CmdParam1[] = TEXT("Notepad.exe");
	TCHAR CmdParam2[] = TEXT("Notepad.exe sp_lb2_1.cpp");
	TCHAR CmdParam3[] = TEXT("Calc.exe");
	_tcscpy_s(CmdParam[0], CmdParam0);
	_tcscpy_s(CmdParam[1], CmdParam1);
	_tcscpy_s(CmdParam[2], CmdParam2);
	_tcscpy_s(CmdParam[3], CmdParam3);

	//ProcHandle[0] = GetCurrentProcess();
	//ProcId[0] = GetCurrentProcessId();
	//ThreadHandle[0] = GetCurrentThread();
	//ThreadId[0] = GetCurrentThreadId();

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
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

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
#pragma endregion

#pragma region WndProc
//== ������� ��������� �������� ���� =================================//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, WndProc_OnCreate);
		HANDLE_MSG(hwnd, WM_COMMAND, WndProc_OnCommand);
		HANDLE_MSG(hwnd, WM_DESTROY, WndProc_OnDestroy);
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//=== ���������� ��������� WM_CREATE ================================//
BOOL WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	HWND hEdit = CreateWindowEx(0L, TEXT("Edit"), TEXT(""),
		WS_CHILD | WS_BORDER | WS_VISIBLE | ES_MULTILINE,
		10, 10, 560, 320, hWnd, (HMENU)(IDC_EDIT_SAM),
		g_hInst, NULL);

	DuplicateHandle
	(
		GetCurrentProcess(), // ��������� ��������, � �������� ��������� ���������������;
		GetCurrentProcess(), // ��������������� ��������;
		GetCurrentProcess(), // ��������� ��������, � �������� ��������� �����, ��������� ���������;
		&ProcHandle[0], // ���� �����, ��������� ���������, ���������������� �������;
		0, // ������������ ��-�� DUPLICATE_SAME_ACCESS;
		FALSE, // ����� ��������� �������� �������������;
		DUPLICATE_SAME_ACCESS
	); // ������ ��������� �������� ������������� �� �� �������� ������, ��� � ���������������

	ProcId[0] = GetCurrentProcessId();

	DuplicateHandle
	(
		GetCurrentProcess(), // ��������� ��������.� �������� ��������� ��������������� ������;
		GetCurrentThread(), // ��������������� ������������� ������;
		GetCurrentProcess(), // ��������� ��������, � �������� ��������� �����, ��������� ��������� ������;
		&ThreadHandle[0], // ���� �����, ��������� ���������, ���������������� ������������ �����;
		0, // ������������ ��-�� DUPLICATE_SAME_ACCESS;
		FALSE, // ����� ��������� ������ �������������;
		DUPLICATE_SAME_ACCESS
	); // ������ ��������� ������ ������������� �� �� �������� ������, ��� � ���������������
	ThreadId[0] = GetCurrentThreadId();

	return TRUE;
}

//=== ���������� ��������� WM_COMMAND ================================//
void WndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	PROCESS_INFORMATION processInfo;
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);

	switch (id)
	{
#pragma region IDM_Notepad
	case IDM_Notepad:
	{
		BOOL processCreated = CreateProcess(NULL, CmdParam[1], NULL,
			NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);
		if (!processCreated)
			MessageBox(hwnd, TEXT("������� �� ������"), TEXT("����"), MB_OK);
		ProcHandle[1] = processInfo.hProcess;
		ProcId[1] = processInfo.dwProcessId;
		ThreadHandle[1] = processInfo.hThread;
		ThreadId[1] = processInfo.dwThreadId;
		CloseHandle(ProcHandle[1]);
		CloseHandle(ThreadHandle[1]);
		break;
	}
#pragma endregion
#pragma region IDM_NotepadText
	case IDM_NotepadText:
	{
		OPENFILENAME ofn;   // ��������� ��� common dialog box
		TCHAR lpszFileSpec[260];   // ������ ��� ����� �����
		//������������ OPENFILENAME						  
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;  // hwnd � ���������� ��������������
		ofn.lpstrFile = lpszFileSpec;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(lpszFileSpec);
		// ������������ ������� ����� �������� �������
		ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
		ofn.nFilterIndex = 1; // ������ ��� �������� ������� �������
		ofn.lpstrFileTitle = NULL; // ��� ���������
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL; // � �������� ���������� ������� �������
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// ����������� ����������� ����
		BOOL fRet = GetOpenFileName(&ofn);
		if (fRet != FALSE)
		{
			wsprintf(CmdParam[2], TEXT("Notepad.exe %s"), ofn.lpstrFile);
			BOOL processCreated = CreateProcess(NULL, CmdParam[2], NULL,
				NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);
			if (!processCreated)
				MessageBox(hwnd, TEXT("������� �� ������"), TEXT("����"), MB_OK);
			else
			{
				ProcHandle[2] = processInfo.hProcess;
				ProcId[2] = processInfo.dwProcessId;
				ThreadHandle[2] = processInfo.hThread;
				ThreadId[2] = processInfo.dwThreadId;
			}
		}
		break;
	}
#pragma endregion
#pragma region IDM_Calculator
	case IDM_Calculator:
	{
		BOOL processCreated = CreateProcess(NULL, CmdParam[3], NULL,
			NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);
		if (!processCreated)
			MessageBox(hwnd, TEXT("������� �� ������"), TEXT("����"), MB_OK);
		ProcHandle[3] = processInfo.hProcess;
		ProcId[3] = processInfo.dwProcessId;
		ThreadHandle[3] = processInfo.hThread;
		ThreadId[3] = processInfo.dwThreadId;
		break;
	}
#pragma endregion
#pragma region IDM_CloseCalculator
	case IDM_CloseCalculator:
	{
		if (ProcHandle[3] != NULL)
		{
			TCHAR result[300];
			DWORD dwExit = 0;
			TerminateProcess(ProcHandle[3], 10);
			GetExitCodeProcess(ProcHandle[3], &dwExit);
			if (dwExit == STILL_ACTIVE)
				wsprintf(result, TEXT("������� ��� �� ����������"));
			else
				wsprintf(result, TEXT("��� ���������� �������� %d"), dwExit);
			MessageBox(hwnd, result, TEXT("����"), MB_OK);
		}
		else
			MessageBox(hwnd, TEXT("������� �� ������"), TEXT("������"), MB_OK);
		break;
	}
#pragma endregion
#pragma region IDM_TestProc
	case IDM_TestProc:
	{
		TCHAR cmdParam[] = TEXT("TestProc.exe");
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.dwXSize = 400;
		si.dwYSize = 300;
		si.dwX = 500;
		si.dwY = 450;
		si.lpTitle = TEXT("TestProc");
		si.wShowWindow = SW_NORMAL; // SW_MAXIMIZE
		si.dwFlags = STARTF_USESIZE | STARTF_USEPOSITION | STARTF_USESHOWWINDOW;

		BOOL processCreated = CreateProcess(NULL, cmdParam, NULL,
			NULL, FALSE, 0, NULL, NULL, &si, &processInfo);
		if (!processCreated)
			MessageBox(hwnd, TEXT("������� �� ������"), TEXT("����"), MB_OK);
		break;
	}
#pragma endregion
#pragma region IDM_NotepadCloseFile
	case IDM_NotepadCloseFile:
	{
		OPENFILENAME ofn;   // ��������� ��� common dialog box
		TCHAR lpszFileSpec[260];   // ������ ��� ����� �����
								   //������������ OPENFILENAME						  
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;  // hwnd � ���������� ��������������
		ofn.lpstrFile = lpszFileSpec;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(lpszFileSpec);
		// ������������ ������� ����� �������� �������
		ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
		ofn.nFilterIndex = 1; // ������ ��� �������� ������� �������
		ofn.lpstrFileTitle = NULL; // ��� ���������
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL; // � �������� ���������� ������� �������
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// ����������� ����������� ����
		BOOL fRet = GetOpenFileName(&ofn);
		if (fRet != FALSE)
		{
			wsprintf(CmdParam[2], TEXT("Notepad.exe %s"), ofn.lpstrFile);
			BOOL processCreated = CreateProcess(NULL, CmdParam[2], NULL,
				NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);
			if (!processCreated)
				MessageBox(hwnd, TEXT("������� �� ������"), TEXT("����"), MB_OK);
			else
			{
				WaitForSingleObject(processInfo.hProcess, INFINITE);
				HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_READ,
					0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
				//�������� ������������ �������� �����
				if (hFile != INVALID_HANDLE_VALUE)
				{
					DWORD dwNumbOfBytes = MAX_BYTES;
					CHAR mbBuffer[MAX_BYTES]; // ����� ��� ������

					ReadFile(hFile, mbBuffer, MAX_BYTES - 1, &dwNumbOfBytes, NULL);

					TCHAR wideBuffer[MAX_BYTES];
					// multibyte into wide characters
					_locale_t locale = _wcreate_locale(LC_ALL, TEXT("Russian"));
					_mbstowcs_s_l(0, wideBuffer, MAX_BYTES, mbBuffer, dwNumbOfBytes, locale);

					if (hFile)
						CloseHandle(hFile);
					SetDlgItemText(hwnd, IDC_EDIT_SAM, wideBuffer);
				}
				CloseHandle(processInfo.hProcess);
				CloseHandle(processInfo.hThread);
			}
		}
		break;
	}
#pragma endregion
	case IDM_CurrentProcess:
		DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)DlgProc, 0);
		break;
	case IDM_NotepadInfo:
		if (ProcHandle[1] != NULL)
			DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)DlgProc, 1);
		else
			MessageBox(hwnd, TEXT("������� �� ������"), TEXT("������"), MB_OK);
		break;
	case IDM_NotepadTextInfo:
		if (ProcHandle[2] != NULL)
			DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)DlgProc, 2);
		else
			MessageBox(hwnd, TEXT("������� �� ������"), TEXT("������"), MB_OK);
		break;
	case IDM_CalculatorInfo:
		if (ProcHandle[3] != NULL)
			DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)DlgProc, 3);
		else
			MessageBox(hwnd, TEXT("������� �� ������"), TEXT("������"), MB_OK);
		break;
	default:
		FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, DefWindowProc);
		break;
	}
}

//=== ���������� ��������� WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}
#pragma endregion

#pragma region DlgProc
LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_INITDIALOG, DlgProc_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, DlgProc_OnCommand);
	}
	return FALSE;
}

BOOL DlgProc_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	TCHAR buffer[256] = { 0 };

#pragma region ��������� �������������
	HWND hEdit1 = GetDlgItem(hwnd, IDC_EDIT1);
	lstrcpy(buffer, ProcImage[lParam]);
	SendMessage(hEdit1, WM_SETTEXT, 0, (LPARAM)buffer);

	HWND hEdit2 = GetDlgItem(hwnd, IDC_EDIT2);
	lstrcpy(buffer, CmdParam[lParam]);
	SendMessage(hEdit2, WM_SETTEXT, 0, (LPARAM)buffer);
#pragma endregion

#pragma region ��������� ��������
	HWND hEdit3 = GetDlgItem(hwnd, IDC_EDIT3);
	wsprintf(buffer, TEXT("%d"), ProcHandle[lParam]);
	SendMessage(hEdit3, WM_SETTEXT, 0, (LPARAM)buffer);

	HWND hEdit4 = GetDlgItem(hwnd, IDC_EDIT4);
	wsprintf(buffer, TEXT("%d"), ProcId[lParam]);
	SendMessage(hEdit4, WM_SETTEXT, 0, (LPARAM)buffer);

	HWND hEdit5 = GetDlgItem(hwnd, IDC_EDIT5);
	DWORD exitCodeProc = NULL;
	GetExitCodeProcess(ProcHandle[lParam], &exitCodeProc);
	if (exitCodeProc == STILL_ACTIVE)
		wsprintf(buffer, TEXT("��������� - �������"));
	else
		wsprintf(buffer, TEXT("%d"), exitCodeProc);
	SendMessage(hEdit5, WM_SETTEXT, 0, (LPARAM)buffer);
#pragma endregion

#pragma region ��������� ������
	HWND hEdit6 = GetDlgItem(hwnd, IDC_EDIT6);
	wsprintf(buffer, TEXT("%d"), ThreadHandle[lParam]);
	SendMessage(hEdit6, WM_SETTEXT, 0, (LPARAM)buffer);

	HWND hEdit7 = GetDlgItem(hwnd, IDC_EDIT7);
	wsprintf(buffer, TEXT("%d"), ThreadId[lParam]);
	SendMessage(hEdit7, WM_SETTEXT, 0, (LPARAM)buffer);

	DWORD exitCodeThread = NULL;
	HWND hEdit8 = GetDlgItem(hwnd, IDC_EDIT8);
	GetExitCodeThread(ThreadHandle[lParam], &exitCodeThread);
	if (exitCodeThread == STILL_ACTIVE)
		wsprintf(buffer, TEXT("��������� - �������"));
	else
		wsprintf(buffer, TEXT("%d"), exitCodeThread);
	SendMessage(hEdit8, WM_SETTEXT, 0, (LPARAM)buffer);
#pragma endregion

#pragma region  ������������� ������ ����������
	HWND hEdit9 = GetDlgItem(hwnd, IDC_EDIT9);
	DWORD prior = GetPriorityClass(ProcHandle[lParam]);
	GetPriorityText(prior, buffer);
	SendMessage(hEdit9, WM_SETTEXT, 0, (LPARAM)buffer);
#pragma endregion

#pragma region ��������� ��������������
	FILETIME ftCreation, ftExit, ftKernel, ftUser;
	if (GetProcessTimes(ProcHandle[lParam], &ftCreation, &ftExit, &ftKernel, &ftUser))
	{
		FILETIME ftWorking, ftWaiting;
		SYSTEMTIME stKernel, stUser, stWorking, stWaiting;

		// ����� ����� ��������
		if (exitCodeProc == STILL_ACTIVE)
		{
			SYSTEMTIME stCurrentTime;
			GetSystemTime(&stCurrentTime);
			FILETIME ftCurrentTime;
			SystemTimeToFileTime(&stCurrentTime, &ftCurrentTime);
			ftWorking = SumFileTime(ftCurrentTime, ftCreation, true);
		}
		else
			ftWorking = SumFileTime(ftExit, ftCreation, true);
		FileTimeToSystemTime(&ftWorking, &stWorking);
		wsprintf(buffer, TEXT("%d ���, %d �, %d ��"), stWorking.wMinute, stWorking.wSecond, stWorking.wMilliseconds);
		HWND hEdit10 = GetDlgItem(hwnd, IDC_EDIT10);
		SendMessage(hEdit10, WM_SETTEXT, 0, (LPARAM)buffer);

		//� ���������������� ������
		FileTimeToSystemTime(&ftUser, &stUser);
		wsprintf(buffer, TEXT("%d ���, %d �, %d ��"), stUser.wMinute, stUser.wSecond, stUser.wMilliseconds);
		HWND hEdit11 = GetDlgItem(hwnd, IDC_EDIT11);
		SendMessage(hEdit11, WM_SETTEXT, 0, (LPARAM)buffer);

		//� ������ ����
		FileTimeToSystemTime(&ftKernel, &stKernel);
		wsprintf(buffer, TEXT("%d ���, %d �, %d ��"), stKernel.wMinute, stKernel.wSecond, stKernel.wMilliseconds);
		HWND hEdit12 = GetDlgItem(hwnd, IDC_EDIT12);
		SendMessage(hEdit12, WM_SETTEXT, 0, (LPARAM)buffer);

		//����� �������
		FILETIME ftTemp = SumFileTime(ftKernel, ftUser, false);
		ftWaiting = SumFileTime(ftWorking, ftTemp, true);
		FileTimeToSystemTime(&ftWaiting, &stWaiting);
		wsprintf(buffer, TEXT("%d ���, %d �, %d ��"), stWaiting.wMinute, stWaiting.wSecond, stWaiting.wMilliseconds);
		HWND hEdit13 = GetDlgItem(hwnd, IDC_EDIT13);
		SendMessage(hEdit13, WM_SETTEXT, 0, (LPARAM)buffer);
#pragma endregion
	}
	return TRUE;
}

void DlgProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
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
#pragma endregion

BOOL GetPriorityText(DWORD priority, TCHAR buffer[256])
{
	BOOL result = TRUE;
	switch (priority)
	{
	case REALTIME_PRIORITY_CLASS:
		wsprintf(buffer, TEXT("Real-time (%d)"), priority);
		break;
	case HIGH_PRIORITY_CLASS:
		wsprintf(buffer, TEXT("High (%d)"), priority);
		break;
	case ABOVE_NORMAL_PRIORITY_CLASS:
		wsprintf(buffer, TEXT("Above normal (%d)"), priority);
		break;
	case NORMAL_PRIORITY_CLASS:
		wsprintf(buffer, TEXT("Normal (%d)"), priority);
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		wsprintf(buffer, TEXT("Below normal (%d)"), priority);
		break;
	case IDLE_PRIORITY_CLASS:
		wsprintf(buffer, TEXT("Idle (%d)"), priority);
		break;
	default:
		result = FALSE;
		break;
	}
	return result;
}

FILETIME SumFileTime(const FILETIME ft1, const FILETIME ft2, BOOL minus)
{
	FILETIME v_ftime;
	ULARGE_INTEGER v_ui;
	__int64 v_right, v_left, v_res;

	v_ui.LowPart = ft1.dwLowDateTime;
	v_ui.HighPart = ft1.dwHighDateTime;
	v_right = v_ui.QuadPart;

	v_ui.LowPart = ft2.dwLowDateTime;
	v_ui.HighPart = ft2.dwHighDateTime;
	v_left = v_ui.QuadPart;

	v_res = minus ? v_right - v_left : v_right + v_left;

	v_ui.QuadPart = v_res;
	v_ftime.dwLowDateTime = v_ui.LowPart;
	v_ftime.dwHighDateTime = v_ui.HighPart;
	return v_ftime;
}