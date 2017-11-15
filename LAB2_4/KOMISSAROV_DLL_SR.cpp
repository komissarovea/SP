#include <Windows.h>
#include <map>

using namespace std;

__declspec(dllexport) std::map<int, char*> IndexCount(HANDLE hFile)
{
	std::map<int, char*> dict;
	DWORD dwNumbOfBytes;
	CHAR tmpBuff[10000];

	ReadFile(hFile, // дескриптор файла
		tmpBuff, // буфер, который принимает прочитанные данные из файла
		10000, // число байтов, которые читаются из файла
		&dwNumbOfBytes, // переменная, которая получает число прочитанных байтов
		NULL); // асинхронный буфер

	for (int i = 0; i < dwNumbOfBytes; i++)
	{
		if ((tmpBuff[i] == 'П' || tmpBuff[i] == 'п') && (i == 0 || tmpBuff[i - 1] == ' '))
		{
			int j = i;
			for (; j < dwNumbOfBytes; j++)
			{
				if (tmpBuff[j] == ' ' || tmpBuff[j] == '\r' || tmpBuff[j] == '\n')
					break;
			}
			int length = j - i;
			char* substr = new char[length + 1]; //(char*)malloc(length + 1);
			strncpy(substr, tmpBuff + i, length);
			substr[length] = '\0';
			printf("[%d] - %s\n", i, substr);
			dict.insert(pair<int, char*>(i, substr));
		}
	}
	return dict;
}

__declspec(dllexport) bool WriteInfoToFile(CHAR *fileName, CHAR *info)
{
	FILE *file;
	if (!(file = fopen(fileName, "a")))
		return false;

	fprintf(file, "\n%s", info);
	fclose(file);
	return true;
}


__declspec(dllexport) bool OpenNotepad(TCHAR *fileName)
{
	STARTUPINFO sInfo;
	memset(&sInfo, 0, sizeof(STARTUPINFO));
	PROCESS_INFORMATION pInfo;
	TCHAR runCommand[256];
	wsprintf(runCommand, TEXT("notepad.exe %s"), fileName);
	if (CreateProcess(NULL, runCommand, NULL, NULL, FALSE, NULL, NULL, NULL, &sInfo, &pInfo))
	{
		CloseHandle(pInfo.hProcess);
		return true;
	}
	return false;
}