#include "stdafx.hpp"
#include "GameFramework.hpp"
#include "GameTimer.hpp"
#include "GameScene.hpp"
#include "GameCamera.hpp"
#include "Player.hpp"

// 프레임 버퍼 색상
constexpr COLORREF FRAMEBUFFER_CLS_COLOR = C_SMOG;

GameFramework::GameFramework()
	: process(NULL), Window(NULL)
	, isPaused(false), autoPaused(false)
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

	POINT center = {
		myFrame.left + (myFrame.right - myFrame.left) / 2
		, myFrame.top + (myFrame.bottom - myFrame.top) / 2
	};
	ClientToScreen(hwnd, &center);
	SetCursorPos(center.x, center.y);

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

	myScene = make_unique<GameScene>(*this);
	myScene->SetHwnd(Window);
	myScene->SetCamera(myCamera);

	myScene->Start();
}

void GameFramework::Update()
{
	myTimer->Tick(FPS_LIMIT);
	auto delta_time = myTimer->GetTimeElapsed();

	if (!isPaused && myScene)
	{
		myScene->Update(delta_time);
		SetCursor(NULL);
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
	ClearFrameBuffer(RGB(230, 230, 230)); // FRAMEBUFFER_CLS_COLOR

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

bool GameFramework::TryPause()
{
	if (isPaused)
	{
		return false;
	}
	else
	{ 
		isPaused = true;
		return true;
	}
}

void GameFramework::Resume()
{
	isPaused = false;
	autoPaused = false;
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
	if (!isPaused && myScene)
	{
		myScene->OnMouse(hwnd, msg, wp, lp);
	}
}

void GameFramework::OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			switch (wp)
			{
				case VK_ESCAPE:
				{
					if (!TryPause())
					{
						Resume();
					}
				}
				break;
			}
		}
		break;

		case WM_KEYUP:
		{
		}
		break;
	}

	if (!isPaused && myScene)
	{
		myScene->OnKeyboard(hwnd, msg, wp, lp);
	}
}

void GameFramework::OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_ACTIVATE:
		{
			const auto act = LOWORD(wp);

			if (WA_INACTIVE == act)
			{
				myTimer->Stop();

				if (TryPause())
				{
					autoPaused = true;
				}
			}
			else
			{
				myTimer->Start();

				if (autoPaused)
				{
					Resume();
				}
			}
		}
		break;

		case WM_SIZE:
		{
		}
		break;
	}

	if (!isPaused && myScene)
	{
		myScene->OnHWND(hwnd, msg, wp, lp);
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
