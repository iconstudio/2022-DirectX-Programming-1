#include "stdafx.hpp"
#include "Enemy.hpp"
#include "Player.hpp"
#include "PlayerBullet.hpp"

Enemy::Enemy()
	: Enemy(XMFLOAT3(0, 0, 0))
{}

Enemy::Enemy(const XMFLOAT3& pos)
	: GameObject(pos)
	, Human(nullptr)
	, myHealth(1), maxHealth(1), isDead(false)
	, Waypoint(pos), patrolDistance(200.0f), isPatrolling(false)
{}

Enemy::~Enemy()
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

void Enemy::OnCollisionEnter(GameObject* other)
{
	const auto tag = other->GetTag();
	switch (tag)
	{
		case GOBJECT_TAGS::PLAYER:
		{
			isKilled = true;
		}
		break;

		case GOBJECT_TAGS::PLAYER_BULLET:
		{
			AddHealth(-3.0f);

			if (myHealth <= 0.0f)
			{
				isKilled = true;
			}
		}
		break;

		default:
		{
		}
		break;
	}
}
