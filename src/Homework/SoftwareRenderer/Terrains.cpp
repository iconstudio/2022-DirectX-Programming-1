#include "stdafx.hpp"
#include "Terrains.hpp"
#include "GameStaticObject.hpp"
#include "GameScene.hpp"
#include "GameMesh.hpp"

RailBorder::RailBorder()
	: GameStaticObject()
	, myExit()
{}

Rail::Rail()
	: GameStaticObject()
{}

Pillar::Pillar()
	: GameStaticObject()
	, myHeight(0.0f), myBottom(), myTop()
	, nodeBefore(nullptr), nodeNext(nullptr)
	, distBefore(0.0f), distNext(0.0f)
{}

void Pillar::SetHeight(float height)
{
	myHeight = height;

	//auto& pos = Transform.GetPosition();
	//pos.y = height * 0.5f;

	auto field = Transform.GetPosition();
	field.y += height * 0.5f;
	myTop = XMFLOAT3(field);

	field.y = 0.0f;
	myBottom = XMFLOAT3(field);
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