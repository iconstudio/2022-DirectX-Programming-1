#include "pch.hpp"
#include "stdafx.h"
#include "FinalProject.h"
#include "Timer.hpp"
#include "Framework.hpp"
#include "GraphicsCore.hpp"

constexpr int MAX_LOADSTRING = 100;
WCHAR captionTitle[MAX_LOADSTRING]{}; // 제목 표시줄 텍스트입니다.
WCHAR captionClass[MAX_LOADSTRING]{}; // 기본 창 클래스 이름입니다.
HINSTANCE gameClient; // 현재 인스턴스입니다.

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

Timer gameTimer{ 100.0f };
GraphicsCore gameRenderer{ FRAME_BUFFER_W, FRAME_BUFFER_H };
Framework gameFramework{ gameRenderer, FRAME_BUFFER_W, FRAME_BUFFER_H };

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, captionTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_FINALPROJECT, captionClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;
	while (true)
	{
		gameTimer.Tick();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		gameFramework.Update(gameTimer.GetDeltaTime());
		//gameFramework.Render();
	}

	return static_cast<int>(msg.wParam);
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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FINALPROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL; // MAKEINTRESOURCEW(IDC_FINALPROJECT);
	wcex.lpszClassName = captionClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int cmd_show)
{
	gameClient = hInstance;

	HWND window = CreateWindowW(captionClass, captionTitle
		, WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT, 0
		, CW_USEDEFAULT, 0
		, nullptr, nullptr, hInstance
		, nullptr);

	if (!window)
	{
		return FALSE;
	}

	ShowWindow(window, cmd_show);
	UpdateWindow(window);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps{};

	switch (msg)
	{
		case WM_CREATE:
		{
			gameRenderer.Awake();
			gameFramework.SetHWND(hwnd).SetHInstance(gameClient).Awake();
			gameRenderer.Start();
			gameFramework.Start();

			DialogBox(gameClient, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
		}
		break;

		case WM_PAINT:
		{
			HDC hdc = BeginPaint(hwnd, &ps);
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

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:
		return (INT_PTR)TRUE;

		case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
