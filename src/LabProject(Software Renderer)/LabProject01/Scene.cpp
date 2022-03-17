#include "stdafx.h"
#include "Scene.h"
#include "GraphicsPipeline.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::BuildObjects()
{
	CCubeMesh *pCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);

	m_nObjects = 5;
	m_ppObjects = new CGameObject*[m_nObjects];

	m_ppObjects[0] = new CGameObject();
	m_ppObjects[0]->SetMesh(pCubeMesh);
	m_ppObjects[0]->SetPosition(0.0f, 0.0f, +14.0f);
	m_ppObjects[0]->SetRotation(0.0f, 0.0f, 0.0f);
	m_ppObjects[0]->SetRotationSpeed(90.0, 0.0f, 0.0f);
	m_ppObjects[0]->SetColor(RGB(255, 255, 0));

	m_ppObjects[1] = new CGameObject();
	m_ppObjects[1]->SetMesh(pCubeMesh);
	m_ppObjects[1]->SetPosition(-10.0f, 0.0f, -14.0f);
	m_ppObjects[1]->SetRotation(0.0f, 0.0f, 0.0f);
	m_ppObjects[1]->SetRotationSpeed(0.0f, 90.0f, 0.0f);
	m_ppObjects[1]->SetColor(RGB(0, 0, 255));

	m_ppObjects[2] = new CGameObject();
	m_ppObjects[2]->SetMesh(pCubeMesh);
	m_ppObjects[2]->SetPosition(+8.0f, +5.0f, -14.0f);
	m_ppObjects[2]->SetRotation(0.0f, 0.0f, 0.0f);
	m_ppObjects[2]->SetRotationSpeed(0.0f, 90.0f, 180.0f);
	m_ppObjects[2]->SetColor(RGB(0, 255, 255));

	m_ppObjects[3] = new CGameObject();
	m_ppObjects[3]->SetMesh(pCubeMesh);
	m_ppObjects[3]->SetPosition(-10.0f, +10.0f, -20.0f);
	m_ppObjects[3]->SetRotation(0.0f, 0.0f, 0.0f);
	m_ppObjects[3]->SetRotationSpeed(30.0f, 180.0f, 90.0f);
	m_ppObjects[3]->SetColor(RGB(255, 0, 0));

	m_ppObjects[4] = new CGameObject();
	m_ppObjects[4]->SetMesh(pCubeMesh);
	m_ppObjects[4]->SetPosition(+16.0f, -10.0f, -5.0f);
	m_ppObjects[4]->SetRotation(0.0f, 0.0f, 0.0f);
	m_ppObjects[4]->SetRotationSpeed(9.0f, 135.0f, 0.0f);
	m_ppObjects[4]->SetColor(RGB(255, 0, 255));
}

void CScene::ReleaseObjects()
{
	for (int i = 0; i < m_nObjects; i++) if (m_ppObjects[i]) delete m_ppObjects[i];
	if (m_ppObjects) delete[] m_ppObjects;
}

void CScene::Animate(float fElapsedTime)
{
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Animate(fElapsedTime);
}

void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	if (pCamera) CGraphicsPipeline::SetCamera(pCamera);

	for (int i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i]->Render(hDCFrameBuffer);
	}
}
