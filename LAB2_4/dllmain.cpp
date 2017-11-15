#include <Windows.h>

//создаем cекцию для "расшаренных" переменных
#pragma data_seg(".ASHARE")
// счетчик загрузок библиотеки
__declspec(dllexport) int g_nDllCallsCount = 0;
#pragma data_seg()//конец секции

//указание компоновщику для создания раздела памяти с соответствующими правами доступа
#pragma comment(linker, "/SECTION:.ASHARE,RWS")

// счетчик вызова функций библиотеки в отдельном процессе
__declspec(dllexport) int g_nFnCallsCount;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	TCHAR moduleName[100];
	GetModuleFileName(hModule, moduleName, 100);
	TCHAR message[200];

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_nDllCallsCount++;
		wsprintf(message, TEXT("Загружается библиотека. Имя модуля: %s. Количество загрузок: %d."), moduleName, g_nDllCallsCount);
		MessageBox(NULL, message, TEXT("Проект KOMISSAROV_DLL"), MB_OK | MB_ICONINFORMATION);
		break;
	case DLL_THREAD_ATTACH:
		MessageBox(NULL, TEXT("Подключается библиотека для нового потока"), TEXT("Проект KOMISSAROV_DLL"), MB_OK | MB_ICONINFORMATION);
		break;
	case DLL_THREAD_DETACH:
		MessageBox(NULL, TEXT("Поток, вызвавший библиотеку завершается"), TEXT("Проект KOMISSAROV_DLL"), MB_OK | MB_ICONINFORMATION);
		break;
	case DLL_PROCESS_DETACH:
		g_nDllCallsCount--;
		wsprintf(message, TEXT("Отключается библиотека. Имя модуля: %s. Количество загрузок: %d."), moduleName, g_nDllCallsCount);
		MessageBox(NULL, message, TEXT("Проект KOMISSAROV_DLL"), MB_OK | MB_ICONINFORMATION);
		break;
	}
	return TRUE;
}

