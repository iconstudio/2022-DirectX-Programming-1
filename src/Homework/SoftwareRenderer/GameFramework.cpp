#include "stdafx.hpp"
#include "GameFramework.hpp"
#include "GameTimer.hpp"
#include "GameScene.hpp"
#include "GameCamera.hpp"
#include "Player.hpp"

GameFramework::GameFramework()
	: process(NULL), window(NULL)
	, myFrame(), mySurface(NULL), myFrameBuffer(NULL)
	, Cursor()
	, myTimer(std::make_unique<GameTimer>()), myScene(nullptr)
	, myCamera(nullptr)
	, myPlayer(nullptr)
{
}

GameFramework::~GameFramework()
{
	if (myFrameBuffer) DeleteObject(myFrameBuffer);
	if (mySurface) DeleteDC(mySurface);
}

void GameFramework::Awake(HINSTANCE instance, HWND hwnd, RECT&& rect)
{
	process = instance;
	window = hwnd;
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
	myCamera = std::make_shared<GameCamera>();

	myScene = std::make_unique<GameScene>(WORLD_H, WORLD_V, WORLD_U);
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

void GameFramework::Render(HDC surface)
{
	ClearFrameBuffer(FRAMEBUFFER_CLS_COLOR);

	if (myScene)
	{
		myScene->Render(mySurface);
	}

	PresentFrameBuffer(surface);
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
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{
			if (myPlayer)
			{
				myPlayer->OnMouse(hwnd, msg, wp, lp);
			}

			SetCursor(NULL);
			GetCursorPos(&Cursor);
		}
		break;
	}
}

void GameFramework::OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			if (myPlayer)
			{
				myPlayer->OnKeyboard(hwnd, msg, wp, lp);
			}
		}
		break;
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
				ReleaseCapture();
				myTimer->Stop();
			}
			else
			{
				SetCapture(hwnd);
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
