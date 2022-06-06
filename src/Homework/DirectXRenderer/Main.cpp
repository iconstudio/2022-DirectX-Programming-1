#include "pch.hpp"
#include "Main.hpp"
#include "GameFramework.hpp"
#include "GameTimer.hpp"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst; // 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING]; // 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING]; // 기본 창 클래스 이름입니다.

Timer gameTimer{};
GameFramework gameFramework{};

ATOM				MyRegisterClass(HINSTANCE instance);
BOOL				InitInstance(HINSTANCE instance, int show);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_APP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg{};
	while (true)
	{
		gameTimer.Tick(0.0f);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		gameFramework.Update();
		//gameFramework.Update(gameTimer.GetTimeElapsed());
		//gameFramework.Render();
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	const UINT timer_id = 0;

	switch (msg)
	{
		case WM_CREATE:
		{
			SetTimer(hwnd, timer_id, UINT(1000.0f / FPS_LIMIT), NULL);
		}
		break;

		case WM_TIMER:
		{
			gameFramework.PrepareRendering();

			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;

		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);

			gameFramework.Render(hdc);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;

		default:
		{
			return gameFramework.OnWindows(hwnd, msg, wparam, lparam);
		}
	}

	return 0;
}

ATOM MyRegisterClass(HINSTANCE instance)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE instance, int nCmdShow)
{
	hInst = instance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT rect = { 0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT };
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rect, style, FALSE);

	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, style
		, CW_USEDEFAULT, CW_USEDEFAULT
		, rect.right - rect.left, rect.bottom - rect.top
		, NULL, NULL, instance, NULL);

	if (!hMainWnd)
	{
		return(FALSE);
	}

	gameFramework.Awake(instance, hMainWnd, std::move(rect));
	gameFramework.Start();

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

	return TRUE;
}
