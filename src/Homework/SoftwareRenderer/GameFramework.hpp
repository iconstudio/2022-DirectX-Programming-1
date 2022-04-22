#pragma once
#include "stdafx.hpp"
#include "GameScene.hpp"
#include "GameCamera.hpp"

class GameFramework
{
public:
	GameFramework();
	~GameFramework();

	void OnCreate(HINSTANCE instance, HWND hwnd);

	void Start();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE process;
	HWND window;

	std::unique_ptr<GameScene> myScene;
	std::shared_ptr<GameCamera> myCamera;
};

