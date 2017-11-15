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
		HANDLE hFile = CreateFile(fileName, // имя файла
			GENERIC_READ, // право доступа
			0, // режим совместного доступа
			(LPSECURITY_ATTRIBUTES)NULL, // возвращенный дескриптор НЕ может быть унаследован дочерними процессами
			OPEN_EXISTING, // открывает файл. Функция завершается ошибкой, если файл не существует
			FILE_ATTRIBUTE_NORMAL, // у файла нет других установленных атрибутов
			(HANDLE)NULL); // дескр.шаблона файла
		if (hFile == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, TEXT("Ошибка открытия файла"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
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
				MessageBox(NULL, TEXT("При открытии файла в Notepad возникла ошбибка"), 
					TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		}
		else
			MessageBox(NULL, TEXT("Ошибка при записи в файл"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);

		FreeLibrary(hLibrary);
	}
	else {
		MessageBox(NULL, TEXT("Не удалось загрузить библиотеку KOMISSAROV_DLL_SR"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
	}
	getchar();
}
