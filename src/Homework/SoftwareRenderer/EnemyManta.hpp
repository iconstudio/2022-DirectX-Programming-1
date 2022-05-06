#pragma once
#include "stdafx.hpp"
#include "Enemy.hpp"

class EnemyManta : public Enemy
{
public:
	EnemyManta(GameScene& scene);
	EnemyManta(GameScene& scene, const XMFLOAT3& pos);


};
