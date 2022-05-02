#include "stdafx.hpp"
#include "Enemy.hpp"
#include "GameScene.hpp"
#include "GameObject.hpp"
#include "CubeMesh.hpp"
#include "Player.hpp"

Enemy::Enemy(GameScene& scene)
	: GameObject(scene)
{}

Enemy::Enemy(GameScene & scene, const XMFLOAT3 & pos)
	: GameObject(scene, pos)
{}

void Enemy::InitHealth(float max_health)
{
	SetMaxHealth(max_health);
	SetHealth(max_health);
}

void Enemy::SetHealth(float value)
{
	myHealth = value;
}

void Enemy::AddHealth(float value)
{
	myHealth += value;
}

void Enemy::SetMaxHealth(float value)
{
	maxHealth = value;
}

float Enemy::GetDistanceFromPlayer() const
{
	return Vector3::Distance(Human->GetPosition(), GetPosition());
}
