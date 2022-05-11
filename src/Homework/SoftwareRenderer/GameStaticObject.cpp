#include "stdafx.hpp"
#include "GameStaticObject.hpp"

GameStaticObject::GameStaticObject()
	: GameEntity()
{}

GameStaticObject::GameStaticObject(const XMFLOAT3& position)
	: GameEntity(position)
{}

GameStaticObject::GameStaticObject(XMFLOAT3&& position)
	: GameEntity(std::forward<XMFLOAT3>(position))
{}
