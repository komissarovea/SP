#include <Windows.h>
#include <stdio.h>

__declspec(dllimport) int g_nDllCallsCount;
__declspec(dllimport) int g_nFnCallsCount;

__declspec(dllimport) int WINAPI Fun101(int a, int b);
__declspec(dllimport) float Fun102(int a, int b, int c);
__declspec(dllimport) void Fun103(double in, double *out);

void main(void) {

	int f1 = Fun101(2, 3);
	printf("Fun101(2, 3) = 2 + 3 = %d\n", f1);
	g_nFnCallsCount++;
	printf("g_nDllCallsCount: %d, g_nFnCallsCount: %d\n\n", g_nDllCallsCount, g_nFnCallsCount);

	float f2 = Fun102(2, 3, 4);
	printf("Fun102(2, 3, 4) = 2*3 / 4 = %f\n", f2);
	g_nFnCallsCount++;
	printf("g_nDllCallsCount: %d, g_nFnCallsCount: %d\n\n", g_nDllCallsCount, g_nFnCallsCount);

	double f3;
	Fun103(2, &f3);
	printf("Fun103(2) = 2 + 2 = %lf\n", f3);
	g_nFnCallsCount++;
	printf("g_nDllCallsCount: %d, g_nFnCallsCount: %d\n\n", g_nDllCallsCount, g_nFnCallsCount);

	getchar();
}