#pragma once
#include "stdafx.hpp"
#include "GameStaticObject.hpp"

class RailBorder : public GameStaticObject
{
public:
	RailBorder();

	XMFLOAT3 myExit;
};

class Rail : public GameStaticObject
{
public:
	Rail();
};

class Pillar : public GameStaticObject
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
