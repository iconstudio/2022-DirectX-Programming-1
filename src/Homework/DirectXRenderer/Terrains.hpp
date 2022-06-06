#pragma once
#include "GameStaticObject.hpp"

class RailBorder : public GameStaticObject
{
public:
	RailBorder();
	RailBorder(const XMFLOAT3& pos);

	void SetExit(const XMFLOAT3& pos);
	void SetExitLook(const float angle);
	void SetIndex(const size_t index);

	size_t myIndex;
	XMFLOAT3 myExit;
	float myExitLook;
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
