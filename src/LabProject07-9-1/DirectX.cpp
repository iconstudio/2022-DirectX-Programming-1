#include "stdafx.h"
#include "DirectX.hpp"
#include "GameFramework.h"
#include "Timer.h"

HINSTANCE appInstance;

constexpr int MAX_LOADSTRING = 100;
WCHAR captionTitle[MAX_LOADSTRING];
WCHAR captionClass[MAX_LOADSTRING];
WCHAR captionFrame[MAX_LOADSTRING];

CGameTimer gameTimer{};
GameFramework gameFramework{ FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };

ATOM MyRegisterClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void SetTitle(HWND hwnd, const wchar_t* caption);

int APIENTRY _tWinMain(HINSTANCE hInstance
	, HINSTANCE hPrevInstance
	, LPTSTR lpCmdLine
	, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	ZeroMemory(captionTitle, sizeof(captionTitle));
	ZeroMemory(captionClass, sizeof(captionClass));

	LoadString(hInstance, IDS_APP_TITLE, captionTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_APP_CLASS, captionClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

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
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		gameFramework.Update(gameTimer.GetTimeElapsed());
		gameFramework.Render();
	}

	gameFramework.WaitForGpuComplete();

	return static_cast<int>(msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE instance)
{
	WNDCLASSEX wcex{};
	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_APP_ICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = captionClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE instance, int cmd_show)
{
	appInstance = instance;

	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, style, FALSE);

	HWND hMainWnd = CreateWindow(captionClass, captionTitle, style
		, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top
		, NULL, NULL
		, instance, NULL);

	if (!hMainWnd)
	{
		return FALSE;
	}

	ShowWindow(hMainWnd, cmd_show);
	UpdateWindow(hMainWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
		case WM_CREATE:
		{
			gameFramework.Awake(appInstance, hwnd);
			gameFramework.Start();
			gameTimer.Reset();

			SetTimer(hwnd, 0, 1, NULL);
		}
		break;

		case WM_ACTIVATE:
		{
			if (LOWORD(wp) == WA_INACTIVE)
				gameTimer.Stop();
			else
				gameTimer.Start();
		}
		case WM_SIZE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			gameFramework.OnWindowsEvent(hwnd, msg, wp, lp);
		}
		break;

		case WM_TIMER:
		{
			const auto framerate = gameTimer.GetFrameRate();
			ZeroMemory(captionFrame, sizeof(captionFrame));

			wsprintf(captionFrame, L"Racing Game (FPS: %u)", framerate);

			SetTitle(hwnd, captionFrame);
		}
		break;
		
		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
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
			return DefWindowProc(hwnd, msg, wp, lp);
		}
	}

	return 0;
}

void SetTitle(HWND hwnd, const wchar_t* caption)
{
	SetWindowText(hwnd, caption);
}
