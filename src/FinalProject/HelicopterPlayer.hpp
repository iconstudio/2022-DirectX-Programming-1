#pragma once
#include "Player.h"

class HellicopterPlayer : public CPlayer
{
public:
	HellicopterPlayer();
	virtual ~HellicopterPlayer();

	void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
	void Update(float delta_time) override;

private:
	GameObject* m_pMainRotorFrame = NULL;
	GameObject* m_pTailRotorFrame = NULL;
};
