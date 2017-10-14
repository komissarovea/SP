#include <Windows.h>
#include "sp_pr1.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
{
#pragma region 3.6
	LPCTSTR lpszHelloText = MESSAGE_TEXT;
	LPCTSTR lpszHelloWndTitle = MESSAGE_TITLE;
	// MessageBox(NULL, lpszHelloText, lpszHelloWndTitle, MB_OK);  
#pragma endregion

#pragma region 3.8
	TCHAR buffer[200];
	LPCTSTR lpszHello = TEXT("Привет из Win32 приложения с графическим интерфесом.\n");
	lstrcpy(buffer, lpszHello);
	lstrcat(buffer, TEXT("Автор: Комиссаров, 60322-2\n"));
	lstrcat(buffer, TEXT("Design time = 6.10.2017 12:00\n"));
	SYSTEMTIME localTime;
	GetLocalTime(&localTime);
	TCHAR timeText[100];
	wsprintf(timeText, TEXT("Run time = %d.%d.%d %d:%d"),
		localTime.wDay, localTime.wMonth, localTime.wYear,
		localTime.wHour, localTime.wMinute);
	lstrcat(buffer, timeText);
	// MessageBox(NULL, buffer, lpszHelloWndTitle, MB_OK);  
#pragma endregion

#pragma region 4.1
	/*MessageBox(NULL, buffer, lpszHelloWndTitle, MB_YESNO | MB_HELP | MB_ICONQUESTION | MB_DEFBUTTON3
		| MB_TOPMOST | MB_RIGHT);*/
#pragma endregion

#pragma region 4.3
	int iRetValue1, iRetValue2;
	LPCTSTR lpszMesBoxTitle = TEXT("Вариант 10");
	LPTSTR lpszResponce;

	do {
		iRetValue1 = MessageBox(NULL, TEXT("The message box contains three push buttons: Yes, No, and Cancel."),
			lpszMesBoxTitle, MB_YESNOCANCEL | MB_ICONASTERISK | MB_SETFOREGROUND);
		switch (iRetValue1)
		{
		case IDYES:
			lpszResponce = TEXT("(Yes) button was pressed.");
			break;
		case IDNO:
			lpszResponce = TEXT("(No) button was pressed.");
			break;
		case IDCANCEL:
			lpszResponce = TEXT("(Cancel) button was pressed.");
			break;
		default: lpszResponce = TEXT("The answer is not clear to me.");
		}

		TCHAR buf[200] = TEXT("");
		lstrcat(buf, lpszResponce);
		lstrcat(buf, TEXT(" Continue to examine return values?"));
		iRetValue2 = MessageBox(NULL, buf, lpszMesBoxTitle, MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND);
	} while (iRetValue2 != IDNO);
#pragma endregion

	return 0;
}