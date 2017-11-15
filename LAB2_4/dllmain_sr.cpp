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
		wprintf(TEXT("����������� ����������. ��� ������: %s.\n"), moduleName);
		break;
	case DLL_THREAD_ATTACH:
		wprintf(TEXT("������������ ���������� ��� ������ ������.\n"));
		break;
	case DLL_THREAD_DETACH:
		wprintf(TEXT("�����, ��������� ���������� �����������.\n"));
		break;
	case DLL_PROCESS_DETACH:
		wprintf(TEXT("����������� ����������. ��� ������: %s.\n"), moduleName);
		break;
	}
	return TRUE;
}