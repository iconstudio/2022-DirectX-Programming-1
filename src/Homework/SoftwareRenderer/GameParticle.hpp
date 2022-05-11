#pragma once
#include "stdafx.hpp"
#include "GameEntity.hpp"

class GameParticle : public GameEntity
{
public:
	virtual constexpr GOBJECT_TAGS GetTag() const { return GOBJECT_TAGS::NONE; }
};
