#include <Windows.h>

//������� c����� ��� "�����������" ����������
#pragma data_seg(".ASHARE")
// ������� �������� ����������
__declspec(dllexport) int g_nDllCallsCount = 0;
#pragma data_seg()//����� ������

//�������� ������������ ��� �������� ������� ������ � ���������������� ������� �������
#pragma comment(linker, "/SECTION:.ASHARE,RWS")

// ������� ������ ������� ���������� � ��������� ��������
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
		wsprintf(message, TEXT("����������� ����������. ��� ������: %s. ���������� ��������: %d."), moduleName, g_nDllCallsCount);
		MessageBox(NULL, message, TEXT("������ KOMISSAROV_DLL"), MB_OK | MB_ICONINFORMATION);
		break;
	case DLL_THREAD_ATTACH:
		MessageBox(NULL, TEXT("������������ ���������� ��� ������ ������"), TEXT("������ KOMISSAROV_DLL"), MB_OK | MB_ICONINFORMATION);
		break;
	case DLL_THREAD_DETACH:
		MessageBox(NULL, TEXT("�����, ��������� ���������� �����������"), TEXT("������ KOMISSAROV_DLL"), MB_OK | MB_ICONINFORMATION);
		break;
	case DLL_PROCESS_DETACH:
		g_nDllCallsCount--;
		wsprintf(message, TEXT("����������� ����������. ��� ������: %s. ���������� ��������: %d."), moduleName, g_nDllCallsCount);
		MessageBox(NULL, message, TEXT("������ KOMISSAROV_DLL"), MB_OK | MB_ICONINFORMATION);
		break;
	}
	return TRUE;
}

