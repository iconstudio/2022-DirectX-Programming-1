#pragma once

#include "Mesh.h"

class CGameObject
{
public:
	CGameObject() { }
	~CGameObject();

private:
	float					m_fxPosition = 0.0f;
	float					m_fyPosition = 0.0f;
	float					m_fzPosition = 0.0f;

	float					m_fxRotation = 0.0f;
	float					m_fyRotation = 0.0f;
	float					m_fzRotation = 0.0f;

	float					m_fxScale = 1.0f;
	float					m_fyScale = 1.0f;
	float					m_fzScale = 1.0f;

    CMesh					*m_pMesh = NULL;        
	DWORD					m_dwColor = RGB(255, 0, 0);

	float					m_fxRotationSpeed = 0.0f;
	float					m_fyRotationSpeed = 0.0f;
	float					m_fzRotationSpeed = 0.0f;

public:
	void SetMesh(CMesh *pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }

	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }

	void SetPosition(float x, float y, float z) { m_fxPosition = x; m_fyPosition = y; m_fzPosition = z; }
	void SetRotation(float x, float y, float z) { m_fxRotation = x; m_fyRotation = y; m_fzRotation = z; }
	void SetRotationSpeed(float x, float y, float z) { m_fxRotationSpeed = x; m_fyRotationSpeed = y; m_fzRotationSpeed = z; }

	void Move(float x, float y, float z) { m_fxPosition += x; m_fyPosition += y; m_fyPosition += z; }
	void Rotate(float x, float y, float z) { m_fxRotation += x; m_fyRotation += y; m_fzRotation += z; }

public:
	CPoint3D WorldTransform(CPoint3D& f3Model);

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer);
};

