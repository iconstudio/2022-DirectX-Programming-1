#include "stdafx.hpp"
#include "Terrains.hpp"
#include "GameObject.hpp"
#include "GameScene.hpp"
#include "GameMesh.hpp"

Rail::Rail(GameScene& scene)
	: GameObject(scene)
{
	SetMesh(scene.meshRail);
}

Pillar::Pillar(GameScene& scene)
	: GameObject(scene)
	, myHeight(0.0f), myBottom(), myTop()
	, nodeBefore(nullptr), nodeNext(nullptr)
	, distBefore(0.0f), distNext(0.0f)
{
	SetMesh(scene.meshPlayer);
}

void Pillar::SetHeight(float height)
{
	myHeight = height;

	auto& pos = Transform.GetPosition();
	pos.y = height * 0.5f;

	auto field = pos;
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
