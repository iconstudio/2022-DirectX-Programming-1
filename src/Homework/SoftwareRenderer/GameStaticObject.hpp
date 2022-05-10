#pragma once
#include "stdafx.hpp"
#include "GameEntity.hpp"

class GameStaticObject : public GameEntity
{
public:
	virtual constexpr bool IsStatic() const override { return true; }
};
