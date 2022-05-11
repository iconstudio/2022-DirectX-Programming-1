#include "stdafx.hpp"
#include "Terrains.hpp"
#include "GameStaticObject.hpp"
#include "GameScene.hpp"
#include "GameMesh.hpp"

RailBorder::RailBorder()
	: GameStaticObject()
	, myExit()
{
	SetColor(RGB(120, 160, 40));
}

void RailBorder::SetExit(const XMFLOAT3& pos)
{
	myExit = pos;
}

Rail::Rail()
	: GameStaticObject()
{
	SetColor(RGB(10, 128, 255));
}

Pillar::Pillar()
	: GameStaticObject()
	, myHeight(0.0f), myBottom(), myTop()
	, nodeBefore(nullptr), nodeNext(nullptr)
	, distBefore(0.0f), distNext(0.0f)
{
	SetColor(RGB(110, 30, 30));
}

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