#include <Windows.h>

__declspec(dllexport) int WINAPI Fun101(int a, int b)
{
	return a + b;
}

__declspec(dllexport) float Fun102(int a, int b, int c)
{
	return (float)a*b / c;
}

__declspec(dllexport) void Fun103(double in, double *out)
{
	*out = in + in;
}
