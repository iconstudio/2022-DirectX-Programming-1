#include "pch.hpp"
#include "Main.hpp"
#include "GameFramework.hpp"

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
TCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.

GameFramework gGameFramework;

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

	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			gGameFramework.Update();
		}
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
			gGameFramework.PrepareRendering();

			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;

		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);

			gGameFramework.Render(hdc);

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
			return gGameFramework.OnWindows(hwnd, msg, wparam, lparam);
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
	hInst = instance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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

	gGameFramework.Awake(instance, hMainWnd, std::move(rect));
	gGameFramework.Start();

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

	return TRUE;
}
