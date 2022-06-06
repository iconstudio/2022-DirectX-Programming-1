#include "pch.hpp"
#include "EnemyManta.hpp"

EnemyManta::EnemyManta()
	: EnemyManta(XMFLOAT3(0, 0, 0))
{}

EnemyManta::EnemyManta(const XMFLOAT3& pos)
	: Enemy(pos)
{
	InitHealth(10.0f);
}
