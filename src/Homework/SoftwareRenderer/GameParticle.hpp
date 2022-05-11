#pragma once
#include "stdafx.hpp"
#include "GameEntity.hpp"

class GameParticle : public GameEntity
{
public:
	GameParticle(XMFLOAT3&& position, XMFLOAT3&& dir, float speed, float duration);
	virtual ~GameParticle();

	constexpr GOBJECT_TAGS GetTag() const override { return GOBJECT_TAGS::NONE; }

	virtual void Update(float elapsed_time);

	const XMFLOAT3 Direction;
	float Speed;
	float Friction;

	bool isKilled;
	float myLifetime;
	float myDuration;
};
