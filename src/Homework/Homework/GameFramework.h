#pragma once

#include "Player.h"
#include "Scene.h"
#include "Timer.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	void SetActive(bool bActive);
	void OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void BuildObjects();
	void ReleaseObjects();

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
	bool						m_bActive = true;

	HINSTANCE					m_hInstance = NULL;
	HWND						m_hWnd = NULL;

	RECT						m_rcClient;
	HDC							m_hDCFrameBuffer = NULL;
	HBITMAP						m_hBitmapFrameBuffer = NULL;
	HBITMAP						m_hBitmapSelect = NULL;

	CScene* m_pScene = NULL;
	CGameTimer m_GameTimer;
	CPlayer* m_pPlayer = NULL;
	CGameObject* m_pLockedObject = NULL;

	POINT m_ptOldCursorPos;

	WCHAR m_pszFrameRate[50];

};

