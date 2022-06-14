#include "pch.hpp"
#include "GameEntity.hpp"
#include "Arithmetics.hpp"

GameEntity::GameEntity()
	: GameObject()
	, myDirection(XMFLOAT3()), mySpeed(0.0f), myGravity()
	, myFriction(0.0f), m_fMaxVelocityXZ(0.0f), m_fMaxVelocityY(0.0f)
	, m_xmf3RotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f)), m_fRotationSpeed(0.0f)
{}

GameEntity::~GameEntity()
{}

void GameEntity::Update(float delta_time)
{
	if (0.0f != m_fRotationSpeed)
	{
		Rotate(m_xmf3RotationAxis, m_fRotationSpeed * delta_time);
	}

	if (0.0f != mySpeed) 
	{
		Move(myDirection, mySpeed * delta_time);
	}

	if (0.0f != myFriction)
	{
		float deceleration = myFriction * delta_time;
		if (mySpeed <= deceleration)
		{
			mySpeed = 0.0f;
		}
		else
		{
			mySpeed -= deceleration;
		}
	}

	GameObject::Update(delta_time);
}

void GameEntity::SetVelocity(const XMFLOAT3& vector)
{
	SetVelocity(std::move(XMFLOAT3(vector)));
}

void GameEntity::SetVelocity(XMFLOAT3&& vector)
{
	const XMFLOAT3 vel = std::forward<XMFLOAT3>(vector);
	myDirection = Vector3::Normalize(vel);
	mySpeed = Vector3::Length(vel);
}

void GameEntity::SetDirection(const XMFLOAT3& direction)
{
	SetDirection(std::move(XMFLOAT3(direction)));
}

void GameEntity::SetDirection(XMFLOAT3&& direction)
{
	myDirection = Vector3::Normalize(std::forward<XMFLOAT3>(direction));
}

void GameEntity::SetSpeed(const float value)
{
	mySpeed = value;
}

void GameEntity::AddSpeed(const float value, const float max)
{
	auto speed = GetSpeed() + value;
	if (max < speed) speed = max;

	SetSpeed(speed);
}

void GameEntity::AddSpeed(const float value)
{
	SetSpeed(GetSpeed() + value);
}

void GameEntity::Accelerate(const XMFLOAT3& accel)
{
	const auto new_vel = Vector3::Add(accel, myDirection, mySpeed);
	myDirection = Vector3::Normalize(new_vel);
	mySpeed = Vector3::Length(new_vel);
}

void GameEntity::Accelerate(const XMFLOAT3& dir, float speed)
{
	const auto my_vel = GetVelocity();
	const auto add_vel = Vector3::ScalarProduct(dir, speed);

	const auto new_vel = Vector3::Add(my_vel, add_vel);
	
	myDirection = Vector3::Normalize(new_vel);
	mySpeed = Vector3::Length(new_vel);
}

float GameEntity::GetSpeed() const
{
	return mySpeed;
}

XMFLOAT3 GameEntity::GetVelocity() const
{
	return Vector3::ScalarProduct(myDirection, mySpeed);
}

const XMFLOAT3& GameEntity::GetGravity() const
{
	return myGravity;
}

XMFLOAT3& GameEntity::GetGravity()
{
	return myGravity;
}

void GameEntity::SetRotationAxis(const XMFLOAT3& axis)
{
	SetRotationAxis(std::move(XMFLOAT3(axis)));
}

void GameEntity::SetRotationAxis(XMFLOAT3&& axis)
{
	m_xmf3RotationAxis = Vector3::Normalize(std::forward<XMFLOAT3>(axis));
}

void GameEntity::SetRotationSpeed(float speed)
{
	m_fRotationSpeed = speed;
}

void GameEntity::SetFriction(float friction)
{
	myFriction = friction;
}

void GameEntity::SetGravity(const XMFLOAT3& gravity)
{
	myGravity = gravity;
}

void GameEntity::SetMaxXZVelocity(float value)
{
	m_fMaxVelocityXZ = value;
}

void GameEntity::SetMaxYVelocity(float value)
{
	m_fMaxVelocityY = value;
}
