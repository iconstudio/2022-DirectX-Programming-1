#pragma once
#include "stdafx.hpp"
#include "Enemy.hpp"

class EnemyCube : public Enemy
{
public:
	EnemyCube();
	EnemyCube(const XMFLOAT3& pos);

	virtual constexpr GOBJECT_TAGS GetTag() const { return GOBJECT_TAGS::ENEMY_CUBE; }
};
