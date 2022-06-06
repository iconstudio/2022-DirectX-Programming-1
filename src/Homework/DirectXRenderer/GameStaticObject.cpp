#include "pch.hpp"
#include "GameStaticObject.hpp"

GameStaticObject::GameStaticObject()
	: GameObject()
{}

GameStaticObject::GameStaticObject(const XMFLOAT3& position)
	: GameObject(position)
{}

GameStaticObject::GameStaticObject(XMFLOAT3&& position)
	: GameObject(std::forward<XMFLOAT3>(position))
{}

GameStaticObject::~GameStaticObject()
{}
