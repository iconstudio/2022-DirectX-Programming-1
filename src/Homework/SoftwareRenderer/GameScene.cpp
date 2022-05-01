#include "stdafx.hpp"
#include "GameScene.hpp"
#include "GameFramework.hpp"
#include "GamePipeline.hpp"
#include "GameObject.hpp"
#include "GameCamera.hpp"
#include "Player.hpp"
#include "CubeMesh.hpp"

GameScene::GameScene(GameFramework& framework, size_t sz_x, size_t height, size_t sz_y)
	: Framework(framework), Window(NULL)
	, worldSizeH(long(sz_x)), worldSizeV(long(height)), worldSizeU(long(sz_y))
	, collisionAreaIndex(0), worldPlayerPositionIndex(0)
	, Instances(), collisionAreas(), preparedCollisionAreas(), Fragments()
	, myPlayer(nullptr), myCamera(nullptr)
{
	Fragments.reserve(300);
}

GameScene::~GameScene()
{}

void GameScene::SetHwnd(HWND hwnd)
{
	Window = hwnd;
}

void GameScene::SetCamera(std::shared_ptr<GameCamera>& cam)
{
	myCamera = cam;
}

void GameScene::Start()
{
	BuildCollisionGroups();
	BuildWorld();
	BuildObjects();
	BuildEnemies();
}

void GameScene::BuildCollisionGroups()
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
}

void GameScene::BuildWorld()
{
	auto pillar_mesh_ptr = new CubeMesh(2.0f, 4.0f, 2.0f);
	auto pillar_mesh = std::shared_ptr<CMesh>(pillar_mesh_ptr);

	constexpr UINT pillar_count = 40;
	constexpr float pillar_place_z_gap = (WORLD_U * 0.8f) / pillar_count;
	XMFLOAT3 place{};

	for (UINT i = 0; i < pillar_count; ++i)
	{
		place.x = 0.5f * WORLD_H + std::cos(1.0f + i / 3.141592f) * 5.0f;
		place.y = 0.0f;
		place.z = WORLD_U * 0.1f + i * pillar_place_z_gap;

		auto pillar = CreateInstance<GameObject>(place);
		pillar->SetMesh(pillar_mesh);
		pillar->SetColor(RGB(110, 30, 30));
	}
}

void GameScene::BuildObjects()
{
	auto player_mesh_ptr = new CubeMesh(5.0f, 5.0f, 5.0f);
	auto player_mesh = std::shared_ptr<CMesh>(player_mesh_ptr);

	myPlayer = std::make_shared<Player>(*this);
	myPlayer->SetHwnd(Window);
	myPlayer->SetPosition(playerSpawnPoint);
	myPlayer->SetMesh(player_mesh);
	myPlayer->SetColor(RGB(0, 0, 255));
	myPlayer->SetCamera(myCamera);

	myCamera->SetFollower(myPlayer.get());
	myCamera->SetLocalPosition(XMFLOAT3(0.0f, 9.0f, -7.0f));
	myCamera->SetLookOffset(XMFLOAT3(0.0f, 0.0f, 10.0f));

	auto cube = CreateInstance<GameObject>(XMFLOAT3(40.0f, 0.0f, 60.0f));
	cube->SetMesh(player_mesh);
}

void GameScene::BuildEnemies()
{}

void GameScene::Update(float elapsed_time)
{
	if (myPlayer)
	{
		myPlayer->Update(elapsed_time);
	}

	for (auto& instance : Instances)
	{
		instance->Update(elapsed_time);
	}
}

void GameScene::PrepareRendering()
{
	GamePipeline::SetProjection(myCamera->projectionPerspective);

	for (const auto& group : collisionAreas)
	{
		if (myCamera->IsInFrustum(group->Collider))
		{
			preparedCollisionAreas.push_back(group);
			group->PrepareRendering();
		}
	}

	if (myPlayer)
	{
		myPlayer->PrepareRendering(*this);
	}
}

void GameScene::Render(HDC surface)
{
	for (const auto& group : preparedCollisionAreas)
	{
		group->Render(surface);
	}

	if (myPlayer)
	{
		myPlayer->Render(surface);
	}

	preparedCollisionAreas.clear();
}

void GameScene::AddFragment(const CFragment& fragment)
{
	Fragments.push_back(fragment);
}

CGroupPtr GameScene::CreateCollisionGroup()
{
	const size_t index = collisionAreas.size();
	auto&& ptr = std::make_shared<GameCollsionGroup>(*this, index, COLLIDE_AREA_H, COLLIDE_AREA_V, COLLIDE_AREA_U);
	collisionAreas.push_back(ptr);

	return ptr;
}

CGroupPtr GameScene::FindProperGroup(const XMFLOAT3& position)
{
	auto cx = Clamp(position.x, -worldSizeH * 0.5f, worldSizeH * 0.5f);
	auto cy = Clamp(position.y, -worldSizeV * 0.5f, worldSizeV * 0.5f);
	auto cz = Clamp(position.z, -worldSizeU * 0.5f, worldSizeU * 0.5f);

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
Type* GameScene::CreateInstance(float x, float y, float z)
{
	return CreateInstance<Type>(std::move(XMFLOAT3(x, y, z)));
}

template<class Type>
Type* GameScene::CreateInstance(const XMFLOAT3& position)
{
	return CreateInstance<Type>(std::move(XMFLOAT3(position)));
}

template<class Type>
Type* GameScene::CreateInstance(XMFLOAT3&& position)
{
	auto inst = new Type(*this);
	inst->SetPosition(position);
	inst->SetCamera(myCamera);

	auto ptr = ObjectPtr(inst);
	Instances.push_back(ptr);

	auto group = FindProperGroup(position);
	group->AddInstance(ptr);

	return inst;
}

void GameScene::OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (myPlayer)
	{
		myPlayer->OnMouse(hwnd, msg, wp, lp);
	}

	switch (msg)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{
		}
		break;
	}
}

void GameScene::OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (myPlayer)
	{
		myPlayer->OnKeyboard(hwnd, msg, wp, lp);
	}

	switch (msg)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
		}
		break;
	}
}

void GameScene::OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (myPlayer)
	{
		myPlayer->OnHWND(hwnd, msg, wp, lp);
	}
}

GameCollsionGroup::GameCollsionGroup(GameScene& scene, const size_t index, size_t sz_x, size_t height, size_t sz_y)
	: Scene(scene), Index(index)
	, Collider()
{
	Collider.Extents = XMFLOAT3(0.5f * sz_x, 0.5f * height, 0.5f * sz_y);
}

void GameCollsionGroup::SetPosition(XMFLOAT3&& position)
{
	Collider.Center = position;
}

void GameCollsionGroup::AddInstance(ObjectPtr& ptr)
{
	Instances.push_back(ptr);
}

void GameCollsionGroup::PrepareRendering()
{
	for (const auto& instance : Instances)
	{
		instance->PrepareRendering(Scene);
	}
}

void GameCollsionGroup::Render(HDC surface) const
{
	for (const auto& instance : Instances)
	{
		instance->Render(surface);
	}
}

bool GameCollsionGroup::Contains(const XMFLOAT3& point)
{
	auto vector = XMLoadFloat3(&point);
	auto result = Collider.Contains(vector);

	return ContainmentType::DISJOINT != result;
}
