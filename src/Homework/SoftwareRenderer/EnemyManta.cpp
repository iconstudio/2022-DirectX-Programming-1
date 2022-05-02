#include "stdafx.hpp"
#include "EnemyManta.hpp"

EnemyManta::EnemyManta(GameScene& scene)
	: EnemyManta(Scene, XMFLOAT3(0, 0, 0))
{}

EnemyManta::EnemyManta(GameScene& scene, const XMFLOAT3& pos)
	: Enemy(Scene, pos)
{
	InitHealth(20.0f);
}
