#pragma once

#include "GameObject.h"
#include "Camera.h"

class CScene
{
public:
	CScene();
	virtual ~CScene();

private:
	int							m_nObjects = 0;
	CGameObject					**m_ppObjects = NULL;

public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

