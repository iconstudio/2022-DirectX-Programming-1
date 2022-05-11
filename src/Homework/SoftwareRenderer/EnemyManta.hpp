#pragma once
#include "stdafx.hpp"
#include "Enemy.hpp"

class EnemyManta : public Enemy
{
public:
	EnemyManta();
	EnemyManta(const XMFLOAT3& pos);

	constexpr GOBJECT_TAGS GetTag() const override { return GOBJECT_TAGS::ENEMY_MANTA; }
};
