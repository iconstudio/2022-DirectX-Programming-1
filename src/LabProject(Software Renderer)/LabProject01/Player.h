#pragma once

#include "GameObject.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

private:
	CCamera*					m_pCamera = NULL;

public:
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void Move(float x, float y, float z);
	void Rotate(float x, float y, float z);

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	CCamera* GetCamera() { return(m_pCamera); }
};

