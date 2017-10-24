#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <math.h>

// Глобальные переменные
LPTSTR g_lpszClassName = TEXT("sp_sr6_class");
LPTSTR g_lpszAplicationTitle = TEXT("Разработчик: Комиссаров Евгений, 60322-2");
LPTSTR g_lpszEmfFile = TEXT("kea.emf");
HINSTANCE g_hInst;
bool firstPaint = true;

//========Предварительное объявление функций=========================
/* Процедура главного окна */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
/* Регистрация класса главного окна */
BOOL Register(HINSTANCE);
/* Создание главного окна */
HWND Create(HINSTANCE, int);

/* Обработчики соощений */
/*--- WM_PAINT -------------------------------------------------------*/
void WndProc_OnPaint(HWND hwnd);
/*--- WM_DESTROY -----------------------------------------------------*/
void WndProc_OnDestroy(HWND hwnd);
/*--------------------------------------------------------------------*/
//====================================================================//

// Точка входа в программу ===========================================//
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpszCmdLine, int nCmdShow)
{
	g_hInst = hInstance;

	BOOL isRegistered = Register(hInstance);
	if (!isRegistered) {
		MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	HWND hWnd = Create(hInstance, nCmdShow);
	if (!hWnd)
	{
		MessageBox(NULL, TEXT("Окно не создано!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//== Функция для регистрации класса окна =============================//
BOOL Register(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.lpszClassName = g_lpszClassName;
	wc.lpfnWndProc = WndProc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	wc.hIcon = LoadIcon(NULL, IDI_SHIELD);
	wc.hCursor = LoadCursor(NULL, IDC_HAND);
	wc.style = CS_VREDRAW | CS_HREDRAW;

	return RegisterClassEx(&wc);
}

//== Функция создания окна ===========================================//
HWND Create(HINSTANCE hInstance, int nCmdShow)
{
	HWND hwnd = CreateWindowEx(NULL, g_lpszClassName,
		g_lpszAplicationTitle,
		WS_OVERLAPPEDWINDOW,
		400,
		400,
		600,
		400,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (hwnd == NULL)
		return hwnd;
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return hwnd;
}

//== Оконная процедура главного окна =================================//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_PAINT, WndProc_OnPaint);
		HANDLE_MSG(hwnd, WM_DESTROY, WndProc_OnDestroy);
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//=== Обработчик сообщения WM_PAINT ==================================//
void WndProc_OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hwnd, &ps); //захватим контекст
	RECT rect;
	GetClientRect(hwnd, &rect); //узнаем клинтскую область

	if (firstPaint) {
#pragma region Text Logic
		LPTSTR lpszFio = TEXT("Комиссаров Евгений Александрович");
		// установим светло-зеленый цвет, согласно варианту №10
		SetTextColor(hDC, RGB(144, 238, 144));
		SetBkMode(hDC, TRANSPARENT); //и фон за текстом (прозрачный)

									 // Количество пикселей на один логический дюйм по высоте экрана
		int deviceCaps = GetDeviceCaps(hDC, LOGPIXELSY);
		// высота в пунктах 24, согласно варианту №10	
		int heightPt = -MulDiv(24, deviceCaps, 72);
		HFONT hFont = CreateFont( //создадим шрифт
			heightPt,
			0, //средняя ширина символа
			0, //угол наклона
			0, //угол ориентации базисной линии
			0, //толщина шрифта
			TRUE, //шрифт - курсивный, согласно варианту №10
			0, //подчеркивание
			0, //зачеркивание
			DEFAULT_CHARSET, //идентификатор набора символов
			0, //точность вывода
			0, //точность отсечения
			0, //качество вывода
			0, //шаг между символами шрифта и семейство
			L"Microsoft Sans Serif"); // имя шрифта, согласно варианту №10
		SelectObject(hDC, hFont); // выбираем созданный шрифт

		DrawText(hDC, lpszFio, -1, &rect, DT_SINGLELINE | DT_CENTER);
		DeleteObject(hFont); // удалим шрифт
#pragma endregion

#pragma region Figure Logic
#define PI 3.14159265359
							 //------ Звезда, согласно варианту №10		
		double r = 100, angle = 270; //радиус = 100, согласно варианту №10
									 // создадим сплошное перо толщиной 3 зеленого цвета, согласно варианту №10
		HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
		//создадим кисть с горизонтальной штриховкой красного цвета, согласно варианту №10
		HBRUSH hBrush = CreateHatchBrush(HS_HORIZONTAL, RGB(255, 0, 0));
		SelectObject(hDC, hPen); // выбираем перо
		SelectObject(hDC, hBrush); // выбираем кисть
		SetPolyFillMode(hDC, WINDING); // выбираем вариант закраски(всю будующую фигуру)

		POINT pt[5], final_pt[5];
		//в цикле идем по точкам и сохраняем их в массив точек
		for (int i = 0; i < 5; i++)
		{
			pt[i].x = 150 + r / 2 * cos(angle*PI / 180); //Запоминаем следующую координату x
			pt[i].y = 150 + r / 2 * sin(angle*PI / 180); //Запоминаем следующую координату y
			angle += 360 / 5; //Увеличиваем угол на величину угла правильного многоугольника
		}
		//перестроим в новый массив очередность рисования
		final_pt[0] = pt[0];
		final_pt[1] = pt[2];
		final_pt[2] = pt[4];
		final_pt[3] = pt[1];
		final_pt[4] = pt[3];

		Polygon(hDC, final_pt, 5);//нарисуем многоугольник используя массив найденных точек

		DeleteObject(hPen);  // удалим перо
		DeleteObject(hBrush); // удалим кисть  
#pragma endregion

#pragma region Save MetaFile
		// создадим метафайл
		HDC hdcMeta = CreateEnhMetaFile(NULL, g_lpszEmfFile, NULL, NULL);
		int cx = rect.right - rect.left;
		int cy = rect.bottom - rect.top;
		// скопируем контекст рисования
		BitBlt(hdcMeta, 0, 0, cx, cy, hDC, 0, 0, SRCCOPY);
		HENHMETAFILE hemf = CloseEnhMetaFile(hdcMeta);
		DeleteEnhMetaFile(hemf); //удалим метафайл
#pragma endregion

		firstPaint = false;
	}
	else
	{
		HENHMETAFILE hemf = GetEnhMetaFile(g_lpszEmfFile); //запросим метафайл
		PlayEnhMetaFile(hDC, hemf, &rect); //воспроизведем данные метафайла	
		DeleteEnhMetaFile(hemf); //удалим метафайл
	}

	EndPaint(hwnd, &ps); //освободим контекст
}

//=== Обработчик сообщения WM_DESTROY ================================//
void WndProc_OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}