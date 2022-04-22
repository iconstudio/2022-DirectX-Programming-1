#include "stdafx.hpp"
#include "GameScene.hpp"
#include "GameObject.hpp"
#include "GameCamera.hpp"
#include "Player.hpp"

GameScene::GameScene(UINT sz_horizontal, UINT sz_vertical, UINT sz_up)
	: worldSizeH(sz_horizontal), worldSizeV(sz_vertical), worldSizeU(sz_up), collisionAreaIndex(0)
	, Instances()
	, myPlayer(nullptr), myCamera(nullptr)
{}

GameScene::~GameScene()
{}

void GameScene::SetCamera(std::shared_ptr<GameCamera> cam)
{
	myCamera = cam;
}

void GameScene::Start()
{
	const auto cgrp_cnt_x = worldSizeH / COLLIDE_AREA_H;
	const auto cgrp_cnt_y = worldSizeV / COLLIDE_AREA_V;
	const auto cgrp_cnt_z = worldSizeU / COLLIDE_AREA_U;

	for (UINT i = 0; i < cgrp_cnt_x; ++i)
	{
		for (UINT j = 0; j < cgrp_cnt_y; ++j)
		{
			for (UINT k = 0; k < cgrp_cnt_z; ++k)
			{
				XMFLOAT3 position{};
				position.x = i * COLLIDE_AREA_H + 0.5f * COLLIDE_AREA_H;
				position.y = j * COLLIDE_AREA_V + 0.5f * COLLIDE_AREA_V;
				position.z = k * COLLIDE_AREA_U + 0.5f * COLLIDE_AREA_U;

				auto&& group = CreateCollisionGroup();
				group->SetPosition(std::move(position));
			}
		}
	}

	auto player = CreateInstance<Player>(playerSpawnPoint);
}

void GameScene::Update(float elapsed_time)
{
	for (auto& instance : Instances)
	{
		instance->Update(elapsed_time);
	}
}

void GameScene::Render(HDC surface)
{
	for (auto& instance : Instances)
	{
		instance->Render(surface);
	}
}

CGroupPtr GameScene::CreateCollisionGroup()
{
	auto index = collisionAreas.size();
	auto&& ptr = std::make_shared<GameCollsionGroup>(index, COLLIDE_AREA_H, COLLIDE_AREA_V, COLLIDE_AREA_U);
	collisionAreas.push_back(ptr);

	return ptr;
}

CGroupPtr GameScene::FindProperGroup(const XMFLOAT3& position)
{
	auto cx = std::clamp(position.x, -worldSizeH * 0.5f, worldSizeH * 0.5f);
	auto cy = std::clamp(position.y, -worldSizeV * 0.5f, worldSizeV * 0.5f);
	auto cz = std::clamp(position.z, -worldSizeU * 0.5f, worldSizeU * 0.5f);

	for (auto& group : collisionAreas)
	{
		if (group->Contains(position))
		{
			return group;
		}
	}

	return CGroupPtr(nullptr);
}

template<class Type>
ObjectPtr GameScene::CreateInstance(float x, float y, float z)
{
	return CreateInstance<Type>(std::move(XMFLOAT3(x, y, z)));
}

template<class Type>
ObjectPtr GameScene::CreateInstance(const XMFLOAT3& position)
{
	return CreateInstance<Type>(std::move(XMFLOAT3(position)));
}

template<class Type>
ObjectPtr GameScene::CreateInstance(XMFLOAT3&& position)
{
	auto group = FindProperGroup(position);
	auto&& inst = std::make_shared<GameObject>(*this, position);
	inst->SetCamera(myCamera);

	group->AddInstance(inst);

	Instances.push_back(inst);

	return inst;
}

GameCollsionGroup::GameCollsionGroup(const UINT index, UINT sz_horizontal, UINT sz_vertical, UINT sz_up)
	: Index(index)
	, Collider()
{
	Collider.Extents = XMFLOAT3(0.5f * sz_horizontal, 0.5f * sz_vertical, 0.5f * sz_up);
}

void GameCollsionGroup::SetPosition(XMFLOAT3&& position)
{
	Collider.Center = position;
}

bool GameCollsionGroup::Contains(const XMFLOAT3& point)
{
	auto vector = XMLoadFloat3(&point);
	auto result = Collider.Contains(vector);

	return ContainmentType::DISJOINT != result;
}

void GameCollsionGroup::AddInstance(ObjectPtr& ptr)
{
	Instances.push_back(ptr);
}
