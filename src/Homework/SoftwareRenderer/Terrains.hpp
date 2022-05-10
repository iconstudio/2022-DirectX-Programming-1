#pragma once
#include "stdafx.hpp"
#include "GameStaticObject.hpp"

class Rail : public GameObject
{
public:
	Rail();
};

class Pillar : public GameObject
{
public:
	Pillar();

	void SetHeight(float height);
	void SetBefore(Pillar* before);
	void SetNext(Pillar* next);

	float myHeight;
	XMFLOAT3 myBottom, myTop;

	Pillar* nodeBefore, *nodeNext;
	float distBefore, distNext;
};
