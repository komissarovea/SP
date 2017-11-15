#include <Windows.h>
#include <stdio.h>

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	TCHAR moduleName[100];
	GetModuleFileName(hModule, moduleName, 100);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		wprintf(TEXT("Загружается библиотека. Имя модуля: %s.\n"), moduleName);
		break;
	case DLL_THREAD_ATTACH:
		wprintf(TEXT("Подключается библиотека для нового потока.\n"));
		break;
	case DLL_THREAD_DETACH:
		wprintf(TEXT("Поток, вызвавший библиотеку завершается.\n"));
		break;
	case DLL_PROCESS_DETACH:
		wprintf(TEXT("Отключается библиотека. Имя модуля: %s.\n"), moduleName);
		break;
	}
	return TRUE;
}