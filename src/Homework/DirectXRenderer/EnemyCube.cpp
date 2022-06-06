#include "pch.hpp"
#include "EnemyCube.hpp"

EnemyCube::EnemyCube()
	: EnemyCube(XMFLOAT3(0, 0, 0))
{}

EnemyCube::EnemyCube(const XMFLOAT3& pos)
	: Enemy(pos)
{
	InitHealth(6.0f);
}
