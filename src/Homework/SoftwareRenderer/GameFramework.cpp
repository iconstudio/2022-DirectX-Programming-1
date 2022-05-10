#include "stdafx.hpp"
#include "GameFramework.hpp"
#include "GameTimer.hpp"
#include "GameScene.hpp"
#include "GameCamera.hpp"
#include "Player.hpp"

GameFramework::GameFramework()
	: process(NULL), Window(NULL)
	, myFrame(), mySurface(NULL), myFrameBuffer(NULL)
	, myTimer(make_unique<GameTimer>()), myScene(nullptr)
	, myCamera(nullptr)
{}

GameFramework::~GameFramework()
{
	if (myFrameBuffer) DeleteObject(myFrameBuffer);
	if (mySurface) DeleteDC(mySurface);
}

void GameFramework::Awake(HINSTANCE instance, HWND hwnd, RECT&& rect)
{
	process = instance;
	SetHwnd(hwnd);
	myFrame = rect;

	HDC hDC = GetDC(hwnd);
	mySurface = CreateCompatibleDC(hDC);

	auto width = myFrame.right - myFrame.left;
	auto height = myFrame.bottom - myFrame.top;
	myFrameBuffer = CreateCompatibleBitmap(hDC, width, height);

	SelectObject(mySurface, myFrameBuffer);

	ReleaseDC(hwnd, hDC);
	SetBkMode(mySurface, TRANSPARENT);
}

void GameFramework::Start()
{
	myCamera = make_shared<GameCamera>();
	myCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	myCamera->GeneratePerspectiveProjectionMatrix(1.01f, 100.0f, 60.0f);
	myCamera->SetFOVAngle(60.0f);
	myCamera->GenerateOrthographicProjectionMatrix(1.01f, 50.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

	myScene = make_unique<GameScene>(*this, WORLD_H, WORLD_V, WORLD_U);
	myScene->SetHwnd(Window);
	myScene->SetCamera(myCamera);

	myScene->Start();
}

void GameFramework::Update()
{
	myTimer->Tick(FPS_LIMIT);
	auto delta_time = myTimer->GetTimeElapsed();

	if (myScene)
	{
		myScene->Update(delta_time);
	}
}

void GameFramework::PrepareRendering()
{
	if (myScene)
	{
		myScene->PrepareRendering();
	}
}

void GameFramework::Render(HDC surface)
{
	ClearFrameBuffer(FRAMEBUFFER_CLS_COLOR);

	if (myScene)
	{
		myScene->Render(mySurface);
	}

	PresentFrameBuffer(surface);
}

void GameFramework::SetHwnd(HWND hwnd)
{
	Window = hwnd;
}

void GameFramework::ClearFrameBuffer(COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, 0, color);
	HBRUSH hBrush = CreateSolidBrush(color);

	HPEN hOldPen = (HPEN)SelectObject(mySurface, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(mySurface, hBrush);

	Rectangle(mySurface, myFrame.left, myFrame.top, myFrame.right, myFrame.bottom);

	SelectObject(mySurface, hOldBrush);
	SelectObject(mySurface, hOldPen);

	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void GameFramework::PresentFrameBuffer(HDC surface)
{
	BitBlt(surface
		, myFrame.left, myFrame.top
		, myFrame.right - myFrame.left, myFrame.bottom - myFrame.top
		, mySurface
		, myFrame.left, myFrame.top, SRCCOPY);
}

void GameFramework::OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (myScene)
	{
		myScene->OnMouse(hwnd, msg, wp, lp);
	}
}

void GameFramework::OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (myScene)
	{
		myScene->OnKeyboard(hwnd, msg, wp, lp);
	}
}

void GameFramework::OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (myScene)
	{
		myScene->OnHWND(hwnd, msg, wp, lp);
	}

	switch (msg)
	{
		case WM_ACTIVATE:
		{
			const auto act = LOWORD(wp);

			if (WA_INACTIVE == act)
			{
				myTimer->Stop();
			}
			else
			{
				myTimer->Start();
			}
		}
		break;

		case WM_SIZE:
		{
		}
		break;
	}
}

LRESULT GameFramework::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{
			OnMouse(hwnd, msg, wp, lp);
		}
		break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			OnKeyboard(hwnd, msg, wp, lp);
		}
		break;

		case WM_ACTIVATE:
		case WM_SIZE:
		{
			OnHWND(hwnd, msg, wp, lp);
		}
		break;

		default:
		{
			return DefWindowProc(hwnd, msg, wp, lp);
		}
		break;
	}

	return 0;
}
