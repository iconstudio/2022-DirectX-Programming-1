#include "pch.hpp"
#include "GameEntity.hpp"
#include "Arithmetics.hpp"

GameEntity::GameEntity()
	: GameObject()
	, myVelocity(), myGravity()
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

	auto xz_vel = Vector3::CrossProduct(myVelocity, { 0, 1, 0 }, false);
	auto xz_speed = sqrtf(xz_vel.x * xz_vel.x + xz_vel.z + xz_vel.z);
	if (m_fMaxVelocityXZ < xz_speed)
	{
		myVelocity.x *= (m_fMaxVelocityXZ / xz_speed);
		myVelocity.z *= (m_fMaxVelocityXZ / xz_speed);
	}

	auto& y_vel = myVelocity.y;
	auto y_speed = abs(myVelocity.y);
	if (m_fMaxVelocityY < y_speed)
	{
		myVelocity.y *= (m_fMaxVelocityY / y_speed);
	}

	const auto movement = Vector3::ScalarProduct(myVelocity, delta_time, false);
	Translate(movement);

	const auto speed = Vector3::Length(myVelocity);
	if (0.0f != speed && 0.0f != myFriction)
	{
		const auto deceleration = myFriction * delta_time;

		if (speed <= deceleration)
		{
			myVelocity = XMFLOAT3();
		}
		else
		{
			const auto dir = Vector3::Normalize(myVelocity);
			const auto deaccel = speed - deceleration;

			myVelocity = Vector3::ScalarProduct(dir, deaccel);
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
	myVelocity = std::forward<XMFLOAT3>(vector);
}

void GameEntity::SetDirection(const XMFLOAT3& direction)
{
	SetDirection(std::move(XMFLOAT3(direction)));
}

void GameEntity::SetDirection(XMFLOAT3&& direction)
{
	auto speed = Vector3::Length(myVelocity);

	myVelocity = Vector3::ScalarProduct(std::forward<XMFLOAT3>(direction), speed);
}

void GameEntity::SetSpeed(const float value)
{
	myVelocity = Vector3::ScalarProduct(myVelocity, value);
}

void GameEntity::AddSpeed(const float value, const float max)
{
	auto speed = GetSpeed() + value;
	if (max < speed)
	{
		speed = max;
	}

	SetSpeed(speed);
}

void GameEntity::AddSpeed(const float value)
{
	SetSpeed(GetSpeed() + value);
}

void GameEntity::Accelerate(const XMFLOAT3& accel)
{
	myVelocity = Vector3::Add(myVelocity, accel);
}

void GameEntity::Accelerate(const XMFLOAT3& dir, float speed)
{
	myVelocity = Vector3::Add(myVelocity, dir, speed);
}

float GameEntity::GetSpeed() const
{
	return Vector3::Length(myVelocity);
}

const XMFLOAT3& GameEntity::GetVelocity() const
{
	return myVelocity;
}

XMFLOAT3& GameEntity::GetVelocity()
{
	return myVelocity;
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
