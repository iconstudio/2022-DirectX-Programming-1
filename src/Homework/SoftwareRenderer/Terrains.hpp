#pragma once
#include "stdafx.hpp"
#include "GameStaticObject.hpp"

class RailBorder : public GameStaticObject
{
public:
	RailBorder();

	void SetExit(const XMFLOAT3& pos);

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
	XMFLOAT3 normalToNext;
};
