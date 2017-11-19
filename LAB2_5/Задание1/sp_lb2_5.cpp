// sp_lb2_5.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "resource.h"
#include <map>
using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
TCHAR lpszFileSpec[256]; //��� ������������ �����

// Forward declarations of functions included in this code module:
ATOM                RegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void WndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void WndProc_OnDestroy(HWND hwnd);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
OPENFILENAME InitializeOpenFileName(HWND hwnd, LPCTSTR lpcFilter);
void GetLastErrorDescription(TCHAR *description);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{
	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32TEMPLATE, szWindowClass, MAX_LOADSTRING);

	BOOL isRegistered = RegisterClass(hInstance);
	if (!isRegistered) {
		MessageBox(NULL, TEXT("������ ����������� ������ ����!"),
			TEXT("������"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		MessageBox(NULL, TEXT("���� �� �������!"),
			TEXT("������"), MB_OK | MB_ICONERROR);
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
		HANDLE_MSG(hWnd, WM_COMMAND, WndProc_OnCommand);
		HANDLE_MSG(hWnd, WM_DESTROY, WndProc_OnDestroy);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//=== ���������� ��������� WM_COMMAND ================================//
void WndProc_OnCommand(HWND hWnd, int wmId, HWND hwndCtl, UINT codeNotify)
{
	static DWORD  dwFileSize; // ������ �����
	static HANDLE hFile; // ���������� �����
	static HANDLE hFileMap; // ���������� ������������� �����
	static LPVOID lpvFile;      // ����� ������� � �������� ������������, ���� ����� ������������ ����
	// Parse the menu selections:
	switch (wmId)
	{
	case IDM_OPEN_FILE:
	{
		//������ ��������� ��� ������� �������� �����
		OPENFILENAME openTextFile = InitializeOpenFileName(hWnd, TEXT("��������� �����\0*.TXT\0"));
		// ����������� ����������� ���� 
		BOOL fRet = GetOpenFileName(&openTextFile);

		// ��������� ���� ��� ������ � ������. ��� ����� ������� ������
		// ���� "����". � ����������� �� ��������� ���������� ������� 
		// CreateFile ���� ��������� ������������ ����, ���� ������� �����.
		// ������ ��� ������� ����� �������������� ��� �������� �����,
		// ������������� � ������. ������� ���������� ���������� ���������
		// ������� ����, ��� ��� ������ INVALID_HANDLE_VALUE.
		hFile = CreateFile(
			lpszFileSpec,  // ��������� �� ������ � ������� �����
			GENERIC_WRITE | GENERIC_READ, // ��������� ����� �������
			0,   //  ����� ����������,0 - ������������� ������ � �����.       
			NULL,// LPSECURITY_ATTRIBUTES=0 - ������ �� �����������.
			OPEN_EXISTING,//���� ���� �� ����������, �� ���������� ������.
			FILE_ATTRIBUTE_NORMAL,//�������� �������� ��� ���� 
			NULL //�� ������ ��� ������� ���� "����"
		);
		if (hFile == INVALID_HANDLE_VALUE)//���� �� ������� ������� ����
		{
			MessageBox(hWnd, TEXT("�� ������� ������� ����"), TEXT("������"), MB_OK | MB_ICONERROR);
			break;
		}
		// ������ ������ �����. ������ �������� NULL, ��� ��� ��������������,
		// ��� ���� ������ 4 ��.
		if ((dwFileSize = GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE)//����� ������ �����. 
		{
			MessageBox(hWnd, TEXT("������ ��� ����������� ������� �����"), TEXT("������"), MB_OK | MB_ICONERROR);
			break;
		}
		EnableMenuItem(GetMenu(hWnd), IDM_CREATE_PROECTION_FILE, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_OPEN_FILE, MF_GRAYED);
		break;
	}
	case IDM_CREATE_PROECTION_FILE:
	{
		// ������� ������ "������������ ����". �� - �� 1 ���� ������, ���
		// ������ �����, ����� ����� ���� �������� � ����� ����� ������� 
		// ������  � �������� � ������ ��� � ����-��������������� �������.
		// ��������� ���� ��� ���������� �������� ���� ANSI - ��� Unicode
		// - �������, ����������� ���� �� �������� ������� ������� WCHAR
		hFileMap = CreateFileMapping(
			hFile,  // ���������� ������������ ������� "����" 
			NULL,   // 
			PAGE_READWRITE, // �������� ������ ������� 
			0,  // LPSECURITY_ATTRIBUTES
			dwFileSize + sizeof(WCHAR),   //������� 32 �������
			NULL  // � ������� 32 ������� ������� �����.
		);
		if (hFileMap == NULL) // ������� ������ "������������ ����" �� �������
		{
			//MessageBox(hWnd,TEXT("File map could not be opened."), NULL, MB_OK|MB_ICONERROR);
			TCHAR errorDescription[256];
			GetLastErrorDescription(errorDescription);
			MessageBox(hWnd, errorDescription, NULL, MB_OK | MB_ICONERROR);
			CloseHandle(hFile);// ����� ������� ��������� �������� �������
			break;
		}
		EnableMenuItem(GetMenu(hWnd), IDM_REFLECT_FILE, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_CREATE_PROECTION_FILE, MF_GRAYED);
		break;
	}
	case IDM_REFLECT_FILE:
	{
		// ��������� ����������� ����� �� ����������� �������� ������������ �
		// �������� �����, ������� � �������� ������������� ����� ����� � ������.
		lpvFile = MapViewOfFile(
			hFileMap, // ���������� ������� "������������ ����" 
			FILE_MAP_WRITE, // ����� �������
			0, // ������� 32 ������� �������� �� ������ �����, 
			0, // ������� 32 ������� �������� �� ������ �����
			0  // � ���������� ������������ ����. 0 - ���� ����.
		);
		if (lpvFile == NULL)
		{// ������������� ������� ������������� ����� �� �������
			MessageBox(hWnd, TEXT("Could   not map view of tile"), NULL, MB_OK | MB_ICONERROR);
			CloseHandle(hFileMap);// ����� ������� ��������� �������� �������
			CloseHandle(hFile);
			break;
		}
		EnableMenuItem(GetMenu(hWnd), IDM_MODIFY_TEXT, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_REFLECT_FILE, MF_GRAYED);
		break;
	}
	case IDM_MODIFY_TEXT:
	{
		LPSTR  lpchANSI;     // ��������� �� ANSI ������
							 // ���������� � ����� ����� ������� ������
		lpchANSI = (LPSTR)lpvFile;
		lpchANSI[dwFileSize] = 0;

		std::map<int, char*> dict;
		for (int i = 0; i < dwFileSize; i++)
		{
			if ((lpchANSI[i] == '�' || lpchANSI[i] == '�') && (i == 0 || lpchANSI[i - 1] == ' '))
			{
				int j = i;
				for (; j < dwFileSize; j++)
				{
					if (lpchANSI[j] == ' ' || lpchANSI[j] == '\r' || lpchANSI[j] == '\n')
						break;
				}
				int length = j - i;
				char* substr = new char[length + 1]; //(char*)malloc(length + 1);
				strncpy(substr, lpchANSI + i, length);
				substr[length] = '\0';
				dict.insert(pair<int, char*>(i, substr));
			}
		}

		char buff[100];
		std::string s = "\r\n";
		std::map<int, char*>::iterator it;
		for (it = dict.begin(); it != dict.end(); it++) {
			_itoa(it->first, buff, 10);
			s += "[";
			s += std::string(buff);
			s += "]";
			s += " - ";
			s += std::string(it->second);
			free(it->second);
			s += "\r\n";
		}
		strcat(lpchANSI, s.c_str());
		dwFileSize += s.size();

		EnableMenuItem(GetMenu(hWnd), IDM_CLOSE_PROECTION_FILE, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_MODIFY_TEXT, MF_GRAYED);
		break;
	}
	case IDM_CLOSE_PROECTION_FILE:
	{
		UnmapViewOfFile(lpvFile); // ��������� ������������� ����� � ���� ��������� ������������
		CloseHandle(hFileMap);// ��������� ������� ������ �� ������ ���� "������������ ����"
		EnableMenuItem(GetMenu(hWnd), IDM_CLOSE_FILE, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_CLOSE_PROECTION_FILE, MF_GRAYED);
		break;
	}
	case IDM_CLOSE_FILE:
	{
		// ������� ����������� ����� �������� ������� ����.��� �����
		// ���������� ��������� ����� � ����� �� ������� ����,
		// � ����� ���� ������� ���������� � ���� ����� ����� �����
		SetFilePointer(hFile, dwFileSize, NULL, FILE_BEGIN);
		SetEndOfFile(hFile);

		// SetEndOfFlle ����� �������� ����� �������� ����������� �������
		// ���� "������������ ����"
		CloseHandle(hFile);// ��������� ������� ������ �� ������ ���� "����"

		EnableMenuItem(GetMenu(hWnd), IDM_OPEN_FILE_NOTEPAD, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_CLOSE_FILE, MF_GRAYED);
		break;
	}
	case IDM_OPEN_FILE_NOTEPAD:
	{
		// ��������� NOTEPAD � ��������� � ���� ��������� ����,
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		memset(&si, 0, sizeof(STARTUPINFO));
		TCHAR openCommand[256];
		si.cb = sizeof(si);// ��������� ���� ������� ��������� si
		si.wShowWindow = SW_SHOW;// ������ ����� ������ ���� NOTEPAD
		si.dwFlags = STARTF_USESHOWWINDOW;// ������������� ���� - ���������
										  // �������� ���� wShowWindow
		wsprintf(openCommand, TEXT("notepad.exe %s"), lpszFileSpec);
		if (CreateProcess(NULL, openCommand,
			NULL, NULL, FALSE, 0,
			NULL, NULL, &si, &pi))
		{
			// ���� ������� ������, ����������� 
			// ����������� ������ � ��������	
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}

		EnableMenuItem(GetMenu(hWnd), IDM_OPEN_FILE, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_OPEN_FILE_NOTEPAD, MF_GRAYED);
		break;
	}
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
	default:
		FORWARD_WM_COMMAND(hWnd, wmId, hwndCtl, codeNotify, DefWindowProc);
		break;
	}
}

//=== ���������� ��������� WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hwnd)
{
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

//�������  ������������� ��������� OPENFILENAME
OPENFILENAME InitializeOpenFileName(HWND hwnd, LPCTSTR lpcFilter)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;// hwnd � ���������� ��������������
	ofn.lpstrFile = lpszFileSpec;//��� ������������ �����
	//ofn.lpstrFile[0] = '\0';//lpstrFile[0]='\0' ��������, ��� GetOpenFileName �� ����� ������������ �������� lpszFileSpec ��� ��������� ���������� �������� � ����� �����
	ofn.nMaxFile = sizeof(lpszFileSpec);
	// ������������ ������� ����� �������� �������
	ofn.lpstrFilter = lpcFilter;
	//ofn.nFilterIndex = 1; // ������ ��� �������� ������� �������
	//ofn.lpstrFileTitle = NULL; // ��� ���������
	//ofn.nMaxFileTitle = 0;
	//ofn.lpstrInitialDir = NULL; // � �������� ���������� ������� �������
	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	return ofn;
}

void GetLastErrorDescription(TCHAR *description)
{
	if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,//�������� ��������� �� ������� 
		NULL,
		GetLastError(),//������������� ���������. ��� ������� �� GetLastError
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),//����. ����� ����� ����������
		description,
		256, //���� FORMAT_MESSAGE_ALLOCATE_BUFFER �� ����������, �� ���� �������� ���������� ������ ������ ��������� ������, � TCHARs.
		NULL
	))
	{
		wsprintf(description, TEXT("�������� ������ %d �� �������"), GetLastError());
	}
}