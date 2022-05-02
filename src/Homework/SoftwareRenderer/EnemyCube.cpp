#include "stdafx.hpp"
#include "EnemyCube.hpp"

EnemyCube::EnemyCube(GameScene& scene)
	: EnemyCube(Scene, XMFLOAT3(0, 0, 0))
{}

EnemyCube::EnemyCube(GameScene & scene, const XMFLOAT3& pos)
	: Enemy(Scene, pos)
{
	InitHealth(10.0f);
}
