#pragma once
#include "stdafx.h"
#include "GameObject.h"

class CScene
{
public:
	CScene(CPlayer* pPlayer);
	virtual ~CScene();

	virtual void BuildObjects();
	virtual void ReleaseObjects();

	void CheckObjectByObjectCollisions();
	void CheckObjectByWallCollisions();
	void CheckPlayerByWallCollision();
	void CheckObjectByBulletCollisions();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);

private:
	CGameObject** m_ppObjects = NULL;
	CWallsObject* m_pWallsObject = NULL;
	CPlayer* m_pPlayer = NULL;
#ifdef _WITH_DRAW_AXIS
	CGameObject* m_pWorldAxis = NULL;
#endif
};

