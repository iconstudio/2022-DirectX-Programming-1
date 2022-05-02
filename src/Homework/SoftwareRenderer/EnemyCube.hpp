#pragma once
#include "stdafx.hpp"
#include "Enemy.hpp"

class EnemyCube : public Enemy
{
public:
	EnemyCube(GameScene& scene);
	EnemyCube(GameScene& scene, const XMFLOAT3& pos);
};
