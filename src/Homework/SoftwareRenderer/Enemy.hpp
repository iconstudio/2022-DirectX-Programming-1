#pragma once
#include "stdafx.hpp"

class Enemy : public GameObject
{
public:
	Enemy();
	Enemy(const XMFLOAT3& pos);

	float myHealth;
	float maxHealth;
	bool isDead;

	// 시작 지점
	XMFLOAT3 Waypoint;
	float patrolDistance;
	float isPatrolling;
};
