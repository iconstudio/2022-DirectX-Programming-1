#pragma once
#include "stdafx.hpp"
#include "Resource.h"
#include "Player.hpp"
#include "GameCamera.hpp"

constexpr int BULLETS = 50;

class AirplanePlayer : public Player
{
public:
	AirplanePlayer();
	virtual ~AirplanePlayer();

	void FireBullet(GameObject* pLockedObject);

	virtual void OnUpdateTransform();
	virtual void Update(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, GameCamera* pCamera);

	//CBulletObject* m_ppBullets[BULLETS];
	float m_fBulletEffectiveRange = 150.0f;
};
