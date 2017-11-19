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
TCHAR lpszFileSpec[256]; //имя открываемого файла

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
		HANDLE_MSG(hWnd, WM_COMMAND, WndProc_OnCommand);
		HANDLE_MSG(hWnd, WM_DESTROY, WndProc_OnDestroy);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//=== Обработчик сообщения WM_COMMAND ================================//
void WndProc_OnCommand(HWND hWnd, int wmId, HWND hwndCtl, UINT codeNotify)
{
	static DWORD  dwFileSize; // размер файла
	static HANDLE hFile; // дескриптор файла
	static HANDLE hFileMap; // дескриптор проецируемого файла
	static LPVOID lpvFile;      // адрес региона в адресном пространстве, куда будет отображаться файл
	// Parse the menu selections:
	switch (wmId)
	{
	case IDM_OPEN_FILE:
	{
		//создаём структуру для диалога открытия файла
		OPENFILENAME openTextFile = InitializeOpenFileName(hWnd, TEXT("Текстовые файлы\0*.TXT\0"));
		// Отображение диалогового окна 
		BOOL fRet = GetOpenFileName(&openTextFile);

		// Открываем файл для чтения и записи. Для этого создаем объект
		// ядра "Файл". В зависимости от указанных параметров функция 
		// CreateFile либо открывает существующий файл, либо создает новый.
		// Только эта функция может использоваться для открытия файла,
		// проецируемого в память. Функция возвращает дескриптор открытого
		// объекта ядра, или код ошибки INVALID_HANDLE_VALUE.
		hFile = CreateFile(
			lpszFileSpec,  // Указатель на строку с имененм файла
			GENERIC_WRITE | GENERIC_READ, // Требуемый режим доступа
			0,   //  Режим разделения,0 - безраздельный доступ к файлу.       
			NULL,// LPSECURITY_ATTRIBUTES=0 - объект не наследуемый.
			OPEN_EXISTING,//Если файл не существует, то возвратить ошибку.
			FILE_ATTRIBUTE_NORMAL,//Оставить атрибуты как есть 
			NULL //Не давать имя объекту ядра "Файл"
		);
		if (hFile == INVALID_HANDLE_VALUE)//если не удалось открыть файл
		{
			MessageBox(hWnd, TEXT("Не удалось открыть файл"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
			break;
		}
		// Узнаем размер файла. Второй параметр NULL, так как предполагается,
		// что файл меньше 4 Гб.
		if ((dwFileSize = GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE)//узнаём размер файла. 
		{
			MessageBox(hWnd, TEXT("Ошибка при опередлении размера файла"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
			break;
		}
		EnableMenuItem(GetMenu(hWnd), IDM_CREATE_PROECTION_FILE, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_OPEN_FILE, MF_GRAYED);
		break;
	}
	case IDM_CREATE_PROECTION_FILE:
	{
		// Создаем объект "проецируемый файл". Он - на 1 байт больше, чем
		// размер файла, чтобы можно было записать в конец файла нулевой 
		// символ  и работать с файлом как с нуль-терминированной строкой.
		// Поскольку пока еще неизвестно содержит файл ANSI - или Unicode
		// - символы, увеличиваем файл на величину размера символа WCHAR
		hFileMap = CreateFileMapping(
			hFile,  // Дескриптор проецирумого объекта "Файл" 
			NULL,   // 
			PAGE_READWRITE, // Атрибуты защиты страниц 
			0,  // LPSECURITY_ATTRIBUTES
			dwFileSize + sizeof(WCHAR),   //Младшие 32 разряда
			NULL  // и старшие 32 разряда размера файла.
		);
		if (hFileMap == NULL) // Открыть объект "проецируемый файл" не удалось
		{
			//MessageBox(hWnd,TEXT("File map could not be opened."), NULL, MB_OK|MB_ICONERROR);
			TCHAR errorDescription[256];
			GetLastErrorDescription(errorDescription);
			MessageBox(hWnd, errorDescription, NULL, MB_OK | MB_ICONERROR);
			CloseHandle(hFile);// Перед выходом закрываем открытые объекты
			break;
		}
		EnableMenuItem(GetMenu(hWnd), IDM_REFLECT_FILE, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_CREATE_PROECTION_FILE, MF_GRAYED);
		break;
	}
	case IDM_REFLECT_FILE:
	{
		// Открываем отображение файла на виртуальное адресное пространство и
		// получаем адрес, начиная с которого располагается образ файла в памяти.
		lpvFile = MapViewOfFile(
			hFileMap, // Дескриптор объекта "Проецируемый файл" 
			FILE_MAP_WRITE, // Режим доступа
			0, // Старшие 32 разряда смещения от начала файла, 
			0, // младшие 32 разряда смещения от начала файла
			0  // и количество отображаемых байт. 0 - весь файл.
		);
		if (lpvFile == NULL)
		{// Спроецировать оконное представление файла не удалось
			MessageBox(hWnd, TEXT("Could   not map view of tile"), NULL, MB_OK | MB_ICONERROR);
			CloseHandle(hFileMap);// Перед выходом закрываем открытые объекты
			CloseHandle(hFile);
			break;
		}
		EnableMenuItem(GetMenu(hWnd), IDM_MODIFY_TEXT, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_REFLECT_FILE, MF_GRAYED);
		break;
	}
	case IDM_MODIFY_TEXT:
	{
		LPSTR  lpchANSI;     // Указатель на ANSI строку
							 // Записываем в конец файла нулевой символ
		lpchANSI = (LPSTR)lpvFile;
		lpchANSI[dwFileSize] = 0;

		std::map<int, char*> dict;
		for (int i = 0; i < dwFileSize; i++)
		{
			if ((lpchANSI[i] == 'П' || lpchANSI[i] == 'п') && (i == 0 || lpchANSI[i - 1] == ' '))
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
		UnmapViewOfFile(lpvFile); // Закрываем представление файла в окне адресного пространства
		CloseHandle(hFileMap);// Уменьшаем счетчик ссылок на объект ядра "Проецируемый файл"
		EnableMenuItem(GetMenu(hWnd), IDM_CLOSE_FILE, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_CLOSE_PROECTION_FILE, MF_GRAYED);
		break;
	}
	case IDM_CLOSE_FILE:
	{
		// Удаляем добавленный ранее концевой нулевой байт.Для этого
		// перемещаем указатель файла в конец на нулевой байт,
		// а затем даем команду установить в этом месте конец файла
		SetFilePointer(hFile, dwFileSize, NULL, FILE_BEGIN);
		SetEndOfFile(hFile);

		// SetEndOfFlle нужно вызывать после закрытия дескриптора объекта
		// ядра "Проецируемый файл"
		CloseHandle(hFile);// Уменьшаем счетчик ссылок на объект ядра "Файл"

		EnableMenuItem(GetMenu(hWnd), IDM_OPEN_FILE_NOTEPAD, MF_ENABLED);
		EnableMenuItem(GetMenu(hWnd), IDM_CLOSE_FILE, MF_GRAYED);
		break;
	}
	case IDM_OPEN_FILE_NOTEPAD:
	{
		// Запускаем NOTEPAD и загружаем в него созданный файл,
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		memset(&si, 0, sizeof(STARTUPINFO));
		TCHAR openCommand[256];
		si.cb = sizeof(si);// Заполняем поле размера структуры si
		si.wShowWindow = SW_SHOW;// Задаем режим показа окна NOTEPAD
		si.dwFlags = STARTF_USESHOWWINDOW;// Устанавливаем флаг - учитывать
										  // значение поля wShowWindow
		wsprintf(openCommand, TEXT("notepad.exe %s"), lpszFileSpec);
		if (CreateProcess(NULL, openCommand,
			NULL, NULL, FALSE, 0,
			NULL, NULL, &si, &pi))
		{
			// Если процесс создан, освобождаем 
			// дескрипторы потока и процесса	
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

//=== Обработчик сообщения WM_DESTROY ================================//
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

//функция  инициализации структуры OPENFILENAME
OPENFILENAME InitializeOpenFileName(HWND hwnd, LPCTSTR lpcFilter)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;// hwnd – дескриптор окна–владельца
	ofn.lpstrFile = lpszFileSpec;//имя открываемого файла
	//ofn.lpstrFile[0] = '\0';//lpstrFile[0]='\0' означает, что GetOpenFileName не будет использовать значение lpszFileSpec для установки начального каталога и имени файла
	ofn.nMaxFile = sizeof(lpszFileSpec);
	// Формирование массива строк шаблонов фильтра
	ofn.lpstrFilter = lpcFilter;
	//ofn.nFilterIndex = 1; // Индекс для текущего шаблона фильтра
	//ofn.lpstrFileTitle = NULL; // Без заголовка
	//ofn.nMaxFileTitle = 0;
	//ofn.lpstrInitialDir = NULL; // В качестве начального текущий каталог
	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	return ofn;
}

void GetLastErrorDescription(TCHAR *description)
{
	if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,//получаем сообщение от системы 
		NULL,
		GetLastError(),//идентификатор сообщения. его получит из GetLastError
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),//язык. Пусть будет английский
		description,
		256, //флаг FORMAT_MESSAGE_ALLOCATE_BUFFER не установлен, то этот параметр определяет размер буфера выводимых данных, в TCHARs.
		NULL
	))
	{
		wsprintf(description, TEXT("описание ошибки %d не найдено"), GetLastError());
	}
}