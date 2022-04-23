#include "stdafx.hpp"
#include "GameScene.hpp"
#include "GameFramework.hpp"
#include "GamePipeline.hpp"
#include "GameObject.hpp"
#include "GameCamera.hpp"
#include "Player.hpp"
#include "CubeMesh.hpp"

GameScene::GameScene(GameFramework& framework, UINT sz_horizontal, UINT sz_vertical, UINT sz_up)
	: Framework(framework), Window(NULL)
	, worldSizeH(sz_horizontal), worldSizeV(sz_vertical), worldSizeU(sz_up), collisionAreaIndex(0)
	, Instances()
	, myPlayer(nullptr), myCamera(nullptr)
{}

GameScene::~GameScene()
{}

void GameScene::SetHwnd(HWND hwnd)
{
	Window = hwnd;
}

void GameScene::SetCamera(std::shared_ptr<GameCamera> cam)
{
	myCamera = cam;
}

void GameScene::Start()
{
	BuildCollisionGroups();
	BuildObjects();
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

void GameScene::BuildObjects()
{
	auto player_mesh = std::make_shared<CubeMesh>(5, 5, 5);

	myPlayer = std::make_shared<Player>(*this);
	myPlayer->SetHwnd(Window);
	myPlayer->SetMesh(player_mesh);
	myPlayer->SetColor(RGB(0, 0, 255));
	myPlayer->SetCamera(myCamera);
	myPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -10.0f));

	auto cube = CreateInstance<GameObject>(XMFLOAT3(40.0f, 0.0f, 60.0f));
	cube->SetMesh(player_mesh);
}

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

void GameScene::Render(HDC surface)
{
	GamePipeline::SetViewport(myCamera->m_Viewport);

	GamePipeline::SetViewPerspectiveProjectTransform(myCamera->m_xmf4x4ViewPerspectiveProject);

	if (myPlayer)
	{
		myPlayer->Render(surface);
	}

	for (auto& instance : Instances)
	{
		instance->Render(surface);
	}
}

void GameScene::OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{
			if (myPlayer)
			{
				myPlayer->OnMouse(hwnd, msg, wp, lp);
			}
		}
		break;
	}
}

void GameScene::OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			if (myPlayer)
			{
				myPlayer->OnKeyboard(hwnd, msg, wp, lp);
			}
		}
		break;
	}
}

void GameScene::OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

CGroupPtr GameScene::CreateCollisionGroup()
{
	auto index = collisionAreas.size();
	auto&& ptr = std::make_shared<GameCollsionGroup>(index, COLLIDE_AREA_H, COLLIDE_AREA_V, COLLIDE_AREA_U);
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
