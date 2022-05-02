#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"

class Enemy : public GameObject
{
public:
	Enemy(GameScene& scene);
	Enemy(GameScene& scene, const XMFLOAT3& pos);

	void InitHealth(float max_health);
	void SetHealth(float value);
	void AddHealth(float value);
	void SetMaxHealth(float value);

	float GetDistanceFromPlayer() const;

	Player* Human;

	float myHealth;
	float maxHealth;
	bool isDead;

	// ���� ����
	XMFLOAT3 Waypoint;
	float patrolDistance;
	float isPatrolling;
};
