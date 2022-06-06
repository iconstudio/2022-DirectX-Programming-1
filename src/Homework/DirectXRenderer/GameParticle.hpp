#pragma once
#include "GameObject.hpp"

class GameParticle : public GameObject
{
public:
	GameParticle(float duration);
	virtual ~GameParticle();

	constexpr GOBJECT_TAGS GetTag() const override { return GOBJECT_TAGS::NONE; }

	constexpr bool IsStatic() const override { return false; }

	virtual void Update(float elapsed_time);

	XMFLOAT3 Direction;
	float Speed;
	float Friction;

	bool isKilled;
	float myLifetime;
	const float myDuration;
};
