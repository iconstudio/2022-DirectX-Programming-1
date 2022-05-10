#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"

class Rail : public GameObject
{
public:
	Rail(GameScene& scene);
};

class Pillar : public GameObject
{
public:
	Pillar(GameScene& scene);

	void SetHeight(float height);
	void SetBefore(Pillar* before);
	void SetNext(Pillar* next);

	float myHeight;
	XMFLOAT3 myBottom, myTop;

	Pillar* nodeBefore, *nodeNext;
	float distBefore, distNext;
};
