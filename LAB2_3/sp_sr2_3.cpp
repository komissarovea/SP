//�2.������ ������.
//������� ���������� ����������, ������� ����� N = 7������ ����� ������� �
//����� ����� ������ k = 3 ������ ����� �������� �� ������� ����� �� �������
//������� ������ <X> ������, ��� � = n + i * k(i = 0, 1, 2, �, 20), ��� i � �����
//������������ �������.

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
	printf("��������� �����: \t\t%.2d:%.2d:%.2d\n", st.wHour, st.wMinute, st.wSecond);

	printf("\n����������� ����������, ������� ����� N = 7 ������ ����� �������");
	printf("\n� ����� ����� ������ k = 3 ������ ����� �������� �� ������� ����� \n\n");

	LARGE_INTEGER li;
	li.QuadPart = -(N * nTimerUnitsPerSecond);
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (SetWaitableTimer(hTimer, &li, k * 1000, NULL, NULL, FALSE)) {
		while (TRUE) {
			WaitForSingleObject(hTimer, INFINITE);
			int x = N + i * k;
			printf("� ������� ������� ������ %d ������\n", x);
			i++;
		}
	}
}