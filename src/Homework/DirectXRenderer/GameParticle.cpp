#include "pch.hpp"
#include "GameParticle.hpp"

GameParticle::GameParticle(float duration)
	: GameObject()
	, Direction(), Speed(), Friction(0.0f)
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

	Transform.Move(Direction, Speed * elapsed_time);
	UpdateBoundingBox();

	float deceleration = Friction * elapsed_time;
	if (std::abs(Speed) < deceleration)
	{
		Speed = 0.0f;
	}
	else
	{
		if (0 < Speed)
		{
			Speed -= deceleration;
		}
		else
		{
			Speed += deceleration;
		}
	}
}
