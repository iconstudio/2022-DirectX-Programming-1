#pragma once
#include "stdafx.hpp"
#include "GameEntity.hpp"

class GameStaticObject : public GameEntity
{
public:
	GameStaticObject();
	GameStaticObject(const XMFLOAT3& position);
	GameStaticObject(XMFLOAT3&& position);

	virtual constexpr GOBJECT_TAGS GetTag() const { return GOBJECT_TAGS::STATIC; }

	virtual constexpr bool IsStatic() const override { return true; }
};
