#pragma once
#include "stdafx.hpp"
#include "GameStaticObject.hpp"

class RailBorder : public GameStaticObject
{
public:
	RailBorder();
	RailBorder(const XMFLOAT3& pos);

	void SetExit(const XMFLOAT3& pos);

	XMFLOAT3 myExit;
};

class Rail : public GameStaticObject
{
public:
	Rail();
	Rail(const XMFLOAT3& pos);
};

class Pillar : public GameStaticObject
{
public:
	Pillar();
	Pillar(const XMFLOAT3& pos);

	void SetHeight(float height);
	void SetBefore(Pillar* before);
	void SetNext(Pillar* next);

	float myHeight;
	XMFLOAT3 myBottom, myTop;

	Pillar* nodeBefore, *nodeNext;
	float distBefore, distNext;
};
