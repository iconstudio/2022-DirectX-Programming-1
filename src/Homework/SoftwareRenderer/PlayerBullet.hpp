#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"

class PlayerBullet : public GameObject
{
public:
	PlayerBullet();
	PlayerBullet(const XMFLOAT3& pos);

	virtual constexpr GOBJECT_TAGS GetTag() const { return GOBJECT_TAGS::PLAYER_BULLET; }

	void SetParent(Player* parent);
	void SetDamage(const float value);

	void Ready();

	virtual void Update(float elapsed_time);

	void OnCollisionWall(const XMFLOAT3& reflection);
	void OnCollision(GameObject* other);

	Player* myParent;

	float myDamage;

	float myLifetime;
	const float myDuration;
};
