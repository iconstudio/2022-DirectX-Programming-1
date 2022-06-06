#pragma once
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
	void PrepareRendering();
	void Render(HDC surface);

	LRESULT CALLBACK OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

private:
	void SetHwnd(HWND hwnd);
	bool TryPause();
	void Resume();

	void ClearFrameBuffer(COLORREF color);
	void PresentFrameBuffer(HDC surface);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	HINSTANCE process;
	HWND Window;
	bool isPaused, autoPaused;

	RECT myFrame;
	HDC mySurface;
	HBITMAP myFrameBuffer;

	unique_ptr<Timer> myTimer;
	unique_ptr<GameScene> myScene;
	shared_ptr<GameCamera> myCamera;
};
