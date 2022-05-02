#include "stdafx.hpp"
#include "Enemy.hpp"
#include "GameScene.hpp"
#include "GameObject.hpp"
#include "CubeMesh.hpp"
#include "Player.hpp"

Enemy::Enemy(GameScene& scene)
	: Enemy(scene, XMFLOAT3(0, 0, 0))
{}

Enemy::Enemy(GameScene & scene, const XMFLOAT3& pos)
	: GameObject(scene, pos)
	, Human(nullptr)
	, myHealth(1), maxHealth(1), isDead(false)
	, Waypoint(pos), patrolDistance(200.0f), isPatrolling(false)
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

void Enemy::SetPlayer(Player* player)
{
	Human = player;
}

void Enemy::Update(float elapsed_time)
{
	GameObject::Update(elapsed_time);
}

float Enemy::GetDistanceFromPlayer() const
{
	return Vector3::Distance(Human->GetPosition(), GetPosition());
}
