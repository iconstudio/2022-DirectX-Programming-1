#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"

class PlayerBullet : public GameObject
{
public:
	PlayerBullet(GameScene& scene);
	PlayerBullet(GameScene& scene, const XMFLOAT3& pos);
	
	void SetDamage(const float value);

	void OnCollisionWall(const XMFLOAT3& reflection);
	void OnCollision(GameObject* other);

	float Damage;
};
