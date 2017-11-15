#include <Windows.h>
#include <stdio.h>

int *gp_nDllCallsCount;
int *gp_nFnCallsCount;

typedef int (WINAPI *pFun101) (int, int);
typedef float(*pFun102) (int, int, int);
typedef void(*pFun103) (double in, double *out);

void main(void) {

	HMODULE hLibrary = LoadLibrary(TEXT("KOMISSAROV_DLL"));
	if (hLibrary) {
		/*(FARPROC&)gp_nDllCallsCount = GetProcAddress(hLibrary, "g_nDllCallsCount");
		(FARPROC&)gp_nFnCallsCount = GetProcAddress(hLibrary, "g_nFnCallsCount");
		pFun101 Fun101 = (pFun101)GetProcAddress(hLibrary, "Fun101");
		pFun102 Fun102 = (pFun102)GetProcAddress(hLibrary, "Fun102");
		pFun103 Fun103 = (pFun103)GetProcAddress(hLibrary, "Fun103");*/

		(FARPROC&)gp_nDllCallsCount = GetProcAddress(hLibrary, MAKEINTRESOURCEA(1));
		(FARPROC&)gp_nFnCallsCount = GetProcAddress(hLibrary, MAKEINTRESOURCEA(2));
		pFun101 Fun101 = (pFun101)GetProcAddress(hLibrary, MAKEINTRESOURCEA(3));
		pFun102 Fun102 = (pFun102)GetProcAddress(hLibrary, MAKEINTRESOURCEA(4));
		pFun103 Fun103 = (pFun103)GetProcAddress(hLibrary, MAKEINTRESOURCEA(5));

		int f1 = Fun101(2, 3);
		printf("explicit Fun101(2, 3) = 2 + 3 = %d\n", f1);
		*gp_nFnCallsCount += 1;
		printf("g_nDllCallsCount: %d, g_nFnCallsCount: %d\n\n", *gp_nDllCallsCount, *gp_nFnCallsCount);

		float f2 = Fun102(2, 3, 4);
		printf("explicit Fun102(2, 3, 4) = 2*3 / 4 = %f\n", f2);
		*gp_nFnCallsCount += 1;
		printf("g_nDllCallsCount: %d, g_nFnCallsCount: %d\n\n", *gp_nDllCallsCount, *gp_nFnCallsCount);

		double f3;
		Fun103(2, &f3);
		printf("explicit Fun103(2) = 2 + 2 = %lf\n", f3);
		*gp_nFnCallsCount += 1;
		printf("g_nDllCallsCount: %d, g_nFnCallsCount: %d\n\n", *gp_nDllCallsCount, *gp_nFnCallsCount);

		FreeLibrary(hLibrary);
	}
	else {
		MessageBox(NULL, TEXT("Не удалось загрузить библиотеку KOMISSAROV_DLL"), TEXT("Проект KOMISSAROV_DLL_E"), MB_OK | MB_ICONERROR);
	}

	getchar();
}