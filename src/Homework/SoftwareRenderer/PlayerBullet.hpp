#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"

class PlayerBullet : public GameObject
{
public:
	PlayerBullet();
	PlayerBullet(const XMFLOAT3& pos);

	constexpr GOBJECT_TAGS GetTag() const override { return GOBJECT_TAGS::PLAYER_BULLET; }

	void SetParent(Player* parent);
	void SetDamage(const float value);

	void Ready();
	void Update(float elapsed_time) override;

	void OnCollisionWall(const XMFLOAT3& reflection);
	void OnCollisionEnter(GameObject* other) override;

	Player* myParent;

	float myDamage;

	float myLifetime;
	const float myDuration;
};
