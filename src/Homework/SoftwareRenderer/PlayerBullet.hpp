#pragma once
#include "stdafx.hpp"

class PlayerBullet : public GameObject
{
public:
	PlayerBullet();
	PlayerBullet(const XMFLOAT3& pos);
	
	void OnCollision(GameObject* other);

	float Damage;
};
