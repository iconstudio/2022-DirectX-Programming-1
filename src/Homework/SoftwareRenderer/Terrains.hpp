#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"

class Rail : public GameObject
{
public:
	Rail(GameScene& scene, const XMFLOAT3& pos);
};

class Pillar : public GameObject
{
public:
	Pillar(GameScene& scene, const XMFLOAT3& pos);
};
