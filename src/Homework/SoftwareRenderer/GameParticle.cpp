#include "stdafx.hpp"
#include "GameParticle.hpp"

GameParticle::GameParticle(XMFLOAT3&& position, XMFLOAT3&& dir, float speed, float duration)
	: GameEntity(std::forward<XMFLOAT3>(position))
	, Direction(dir), Speed(speed), Friction(0.0f)
	, isKilled(false), myLifetime(duration), myDuration(duration)
{}

GameParticle::~GameParticle()
{}

void GameParticle::Update(float elapsed_time)
{
	if (0 < myLifetime)
	{
		myLifetime -= elapsed_time;
	}
	else
	{
		isKilled = true;
	}
}
