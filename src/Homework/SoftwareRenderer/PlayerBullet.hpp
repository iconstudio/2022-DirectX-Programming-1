#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"

class PlayerBullet : public GameObject
{
public:
	PlayerBullet();
	PlayerBullet(const XMFLOAT3& pos);
	
	void OnCollisionWall(const XMFLOAT3& reflection);
	void OnCollision(GameObject* other);

	float Damage;
};
