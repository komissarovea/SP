#include <Windows.h>
#include <locale.h>
#include <map>

typedef std::map<int, char*>(*pIndexCount) (HANDLE);
typedef bool(*pWriteInfoToFile) (CHAR*, CHAR*);
typedef bool(*pOpenNotepad) (TCHAR*);

void main(void) {
	setlocale(LC_ALL, "Russian");

	HMODULE hLibrary = LoadLibrary(TEXT("KOMISSAROV_DLL_SR"));
	if (hLibrary) {
		pIndexCount IndexCount = (pIndexCount)GetProcAddress(hLibrary, "IndexCount");
		pWriteInfoToFile WriteInfoToFile = (pWriteInfoToFile)GetProcAddress(hLibrary, "WriteInfoToFile");
		pOpenNotepad OpenNotepad = (pOpenNotepad)GetProcAddress(hLibrary, "OpenNotepad");

		TCHAR* fileName = TEXT("test.txt");
		HANDLE hFile = CreateFile(fileName, // ��� �����
			GENERIC_READ, // ����� �������
			0, // ����� ����������� �������
			(LPSECURITY_ATTRIBUTES)NULL, // ������������ ���������� �� ����� ���� ����������� ��������� ����������
			OPEN_EXISTING, // ��������� ����. ������� ����������� �������, ���� ���� �� ����������
			FILE_ATTRIBUTE_NORMAL, // � ����� ��� ������ ������������� ���������
			(HANDLE)NULL); // �����.������� �����
		if (hFile == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, TEXT("������ �������� �����"), TEXT("������"), MB_OK | MB_ICONERROR);
			return;
		}

		std::map<int, char*> dict = IndexCount(hFile);
		CloseHandle(hFile);

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

		if (WriteInfoToFile("test.txt", (char*)s.c_str()))
		{
			if (!OpenNotepad(fileName))
				MessageBox(NULL, TEXT("��� �������� ����� � Notepad �������� �������"), 
					TEXT("������"), MB_OK | MB_ICONERROR);
		}
		else
			MessageBox(NULL, TEXT("������ ��� ������ � ����"), TEXT("������"), MB_OK | MB_ICONERROR);

		FreeLibrary(hLibrary);
	}
	else {
		MessageBox(NULL, TEXT("�� ������� ��������� ���������� KOMISSAROV_DLL_SR"),
			TEXT("������"), MB_OK | MB_ICONERROR);
	}
	getchar();
}
