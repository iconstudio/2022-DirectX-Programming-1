#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"

class PlayerBullet : public GameObject
{
public:
	PlayerBullet(GameScene& scene);
	PlayerBullet(GameScene& scene, const XMFLOAT3& pos);
	
	void SetDamage(const float value);
	void SetParent(Player* parent);

	virtual void Update(float elapsed_time);

	void OnCollisionWall(const XMFLOAT3& reflection);
	void OnCollision(GameObject* other);

	Player* myParent;

	float myDamage;

	float myLifetime;
	const float myDuration;
};
