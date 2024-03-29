#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"

class Enemy : public GameObject
{
public:
	Enemy();
	Enemy(const XMFLOAT3& pos);
	virtual ~Enemy();

	constexpr GOBJECT_TAGS GetTag() const override { return GOBJECT_TAGS::ENEMY; }

	void InitHealth(float max_health);
	void SetHealth(float value);
	void AddHealth(float value);
	void SetMaxHealth(float value);
	void SetPlayer(Player* player);

	virtual void Update(float elapsed_time) override;

	float GetDistanceFromPlayer() const;

	virtual void OnCollisionEnter(GameObject* other) override;

	Player* Human;

	float myHealth;
	float maxHealth;
	bool isDead;

	// ���� ����
	XMFLOAT3 Waypoint;
	float patrolDistance;
	float isPatrolling;
};
