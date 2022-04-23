#pragma once
#include "stdafx.hpp"
#include "GameTimer.hpp"
#include "GameScene.hpp"

class GameFramework
{
public:
	GameFramework();
	~GameFramework();

	void Awake(HINSTANCE instance, HWND hwnd, RECT&& rect);
	void Start();
	void Update();
	void Render(HDC surface);

	LRESULT CALLBACK OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

private:
	void SetHwnd(HWND hwnd);

	void ClearFrameBuffer(COLORREF color);
	void PresentFrameBuffer(HDC surface);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	HINSTANCE process;
	HWND Window;

	RECT myFrame;
	HDC mySurface;
	HBITMAP myFrameBuffer;

	std::unique_ptr<GameTimer> myTimer;
	std::unique_ptr<GameScene> myScene;
	std::shared_ptr<GameCamera> myCamera;
};

