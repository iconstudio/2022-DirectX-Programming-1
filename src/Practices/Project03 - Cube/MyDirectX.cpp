#include "pch.h"
#include "MyDirectX.hpp"
#include "CGameFramework.hpp"

constexpr int MAX_LOADSTRING = 100;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
WCHAR m_pszFrameRate[MAX_LOADSTRING];

HINSTANCE hInst;
CGameFramework gameFramework{};
CGameTimer gameTimer{};

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PROJECT, szWindowClass, MAX_LOADSTRING);
	wcscpy_s(m_pszFrameRate, L"LapProject (");

	MyRegisterClass(hInstance);

	gameTimer.Reset();

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;
	while (true)
	{
		gameTimer.Tick(0.0f);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		gameFramework.FrameAdvance(gameTimer.GetTimeElapsed());
	}

	gameFramework.OnDestroy();

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CREATE:
		{
			SetTimer(hwnd, 0, 50, NULL);

			gameFramework.OnCreate(hInst, hwnd);
		}
		break;

		case WM_TIMER:
		{
			gameTimer.GetFrameRate(m_pszFrameRate + 12, 37);

			SetWindowText(hwnd, m_pszFrameRate);
		}
		break;

		case WM_ACTIVATE:
		{
			if (WA_INACTIVE == LOWORD(wParam))
				gameTimer.Stop();
			else
				gameTimer.Start();
		}
		break;

		case WM_SIZE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_KEYDOWN:
		case WM_KEYUP:
		gameFramework.OnProcessingWindowMessage(hwnd, msg, wParam, lParam);
		break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;

		default:
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}

	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	RECT rect = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rect, style, FALSE);

	HWND main_window = CreateWindow(szWindowClass, szTitle
		, style
		, CW_USEDEFAULT, CW_USEDEFAULT
		, rect.right - rect.left, rect.bottom - rect.top
		, NULL, NULL
		, hInstance
		, NULL);

	if (!main_window)
	{
		return FALSE;
	}

	ShowWindow(main_window, nCmdShow);
	UpdateWindow(main_window);

	return TRUE;
}
