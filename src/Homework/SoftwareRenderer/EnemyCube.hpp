#pragma once
#include "stdafx.hpp"
#include "Enemy.hpp"

class EnemyCube : public Enemy
{
public:
	EnemyCube();
	EnemyCube(const XMFLOAT3& pos);

	constexpr GOBJECT_TAGS GetTag() const override { return GOBJECT_TAGS::ENEMY_CUBE; }
};
