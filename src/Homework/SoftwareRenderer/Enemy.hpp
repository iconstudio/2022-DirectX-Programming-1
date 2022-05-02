#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"

class Enemy : public GameObject
{
public:
	Enemy();
	Enemy(const XMFLOAT3& pos);

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
