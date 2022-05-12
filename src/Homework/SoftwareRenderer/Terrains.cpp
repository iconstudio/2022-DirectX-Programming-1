#include "stdafx.hpp"
#include "Terrains.hpp"
#include "GameStaticObject.hpp"
#include "GameScene.hpp"
#include "GameMesh.hpp"

RailBorder::RailBorder()
	: RailBorder(XMFLOAT3())
{
	SetColor(RGB(120, 160, 40));
}

RailBorder::RailBorder(const XMFLOAT3& pos)
	: GameStaticObject(pos)
	, myExit(pos)
{}

void RailBorder::SetExit(const XMFLOAT3& pos)
{
	myExit = pos;
}

void RailBorder::SetExitLook(const float angle)
{
	myExitLook = angle;
}

void RailBorder::SetIndex(const size_t index)
{
	myIndex = index;
}

Rail::Rail()
	: Rail(XMFLOAT3())
{
	SetColor(RGB(255, 128, 10));
}

Rail::Rail(const XMFLOAT3& pos)
	: GameStaticObject(pos)
{}

Pillar::Pillar()
	: Pillar(XMFLOAT3())
{
	SetColor(RGB(110, 30, 30));
}

Pillar::Pillar(const XMFLOAT3& pos)
	: GameStaticObject(pos)
	, myHeight(0.0f), myBottom(), myTop()
	, nodeBefore(nullptr), nodeNext(nullptr)
	, distBefore(0.0f), distNext(0.0f)
{}

void Pillar::SetHeight(float height)
{
	myHeight = height;

	auto& pos = Transform.GetPosition();
	pos.y = height * 0.5f;

	XMFLOAT3 field = XMFLOAT3(pos);
	field.y += height * 0.5f;
	myTop = (field);

	field.y = 0.0f;
	myBottom = (field);
}

void Pillar::SetBefore(Pillar* before)
{
	nodeBefore = before;
	distBefore = Vector3::Distance(myTop, before->myTop);
}

void Pillar::SetNext(Pillar* next)
{
	nodeNext = next;
	distNext = Vector3::Distance(myTop, next->myTop);
}