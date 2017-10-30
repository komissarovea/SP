//Т2.Ждущий таймер.
//Создать консольное приложение, которое через N = 7секунд после запуска и
//затем через каждые k = 3 секунд будет выводить на консоль фразу «С момента
//запуска прошло <X> секунд», где Х = n + i * k(i = 0, 1, 2, …, 20), где i – номер
//срабатывания таймера.

#include <windows.h>
#include <stdio.h>
#include <locale.h>

void main(void)
{
	const int nTimerUnitsPerSecond = 10000000;
	int N = 7;
	int k = 3;
	int i = 0;

	setlocale(LC_ALL, "Russian");

	SYSTEMTIME st;
	GetLocalTime(&st);
	printf("Начальное время: \t\t%.2d:%.2d:%.2d\n", st.wHour, st.wMinute, st.wSecond);

	printf("\nКконсольное приложение, которое через N = 7 секунд после запуска");
	printf("\nи затем через каждые k = 3 секунд будет выводить на консоль фразу \n\n");

	LARGE_INTEGER li;
	li.QuadPart = -(N * nTimerUnitsPerSecond);
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (SetWaitableTimer(hTimer, &li, k * 1000, NULL, NULL, FALSE)) {
		while (TRUE) {
			WaitForSingleObject(hTimer, INFINITE);
			int x = N + i * k;
			printf("С момента запуска прошло %d секунд\n", x);
			i++;
		}
	}
}