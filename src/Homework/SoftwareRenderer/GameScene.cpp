#include "stdafx.hpp"
#include "GameScene.hpp"
#include "GameFramework.hpp"
#include "GamePipeline.hpp"
#include "GameObject.hpp"
#include "GameMesh.hpp"
#include "GameCamera.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"
#include "Player.hpp"
#include "PlayerBullet.hpp"
#include "Enemy.hpp"
#include "EnemyCube.hpp"
#include "EnemyManta.hpp"
#include "CubeMesh.hpp"

GameScene::GameScene(GameFramework& framework, size_t sz_x, size_t height, size_t sz_y)
	: Framework(framework), Window(NULL)
	, collisionAreaIndex(0), worldPlayerPositionIndex(0)
	, Instances(), staticStart()
	, collisionAreas(), preparedCollisionAreas(), Fragments()
	, myPlayer(nullptr), myCamera(nullptr)
	, meshPlayer(nullptr), meshPlayerBullet(nullptr)
	, meshEnemyCube(nullptr), meshEnemyManta(nullptr), meshEnemyBullet(nullptr)
	, meshPillars(), meshRail(nullptr)
{
	Fragments.reserve(300);
	Instances.reserve(300);
}

GameScene::~GameScene()
{
	for (auto& pair : Pens)
	{
		DeleteObject(pair.second);
	}
}

void GameScene::SetHwnd(HWND hwnd)
{
	Window = hwnd;
}

void GameScene::SetCamera(shared_ptr<GameCamera>& cam)
{
	myCamera = cam;
}

void GameScene::Start()
{
	BuildMeshes();
	BuildCollisionGroups();
	BuildWorld();
	BuildObjects();
	CompleteBuilds();
}

void GameScene::BuildMeshes()
{
	meshPlayer = static_pointer_cast<CMesh>(make_shared<CubeMesh>(5.0f, 5.0f, 5.0f));
	meshPlayerBullet = static_pointer_cast<CMesh>(make_shared<CubeMesh>(1.0f, 1.0f, 15.0f));

	meshEnemyCube = static_pointer_cast<CMesh>(make_shared<CubeMesh>(3.0f, 3.0f, 3.0f));
	meshEnemyManta = static_pointer_cast<CMesh>(make_shared<CubeMesh>(6.0f, 2.0f, 9.0f));
	meshEnemyBullet = static_pointer_cast<CMesh>(make_shared<CubeMesh>(1.0f, 1.0f, 15.0f));

	for (int i = 0; i < 15; ++i)
	{
		meshPillars[i] = static_pointer_cast<CMesh>(make_shared<CubeMesh>(2.0f, 1.0f * i, 2.0f));
	}
	meshRail = static_pointer_cast<CMesh>(make_shared<CubeMesh>(1.0f, 1.5f, 10.0f));
}

void GameScene::BuildCollisionGroups()
{
	constexpr auto cgrp_cnt_x = WORLD_H / COLLIDE_AREA_H;
	constexpr auto cgrp_cnt_y = WORLD_V / COLLIDE_AREA_V;
	constexpr auto cgrp_cnt_z = WORLD_U / COLLIDE_AREA_U;

	for (int i = 0; i < cgrp_cnt_x; ++i)
	{
		for (int j = 0; j < cgrp_cnt_y; ++j)
		{
			for (int k = 0; k < cgrp_cnt_z; ++k)
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
	constexpr float pillar_count = 40.0f; // 40U
	constexpr float pillar_place_z_gap = (WORLD_U * 0.9f) / pillar_count;
	XMFLOAT3 place{};

	for (float i = 0.0f; i < pillar_count; i++)
	{
		place.x = 0.5f * WORLD_H + std::cos(1.0f + i / 3.141592f) * 5.0f;
		place.y = 0.0f;
		place.z = WORLD_U * 0.05f + i * pillar_place_z_gap;

		auto pillar = CreateInstance<GameObject>(place);
		if (!pillar)
			continue;

		pillar->SetMesh(meshPillar);
		pillar->SetColor(RGB(110, 30, 30));
	}
}

void GameScene::BuildObjects()
{
	myPlayer = make_shared<Player>(*this);
	myPlayer->SetHwnd(Window);
	myPlayer->SetPosition(playerSpawnPoint);
	myPlayer->SetMesh(meshPlayer);
	myPlayer->SetColor(RGB(0, 0, 255));
	myPlayer->SetCamera(myCamera);

	myCamera->SetFollower(myPlayer.get());
	myCamera->SetLocalPosition(XMFLOAT3(0.0f, 9.0f, -7.0f));
	myCamera->SetLookOffset(XMFLOAT3(0.0f, 2.0f, 6.0f));

	auto cube1 = SpawnEnemy(ENEMY_TYPES::CUBE, XMFLOAT3(40.0f, 0.0f, 60.0f));
	auto cube3 = SpawnEnemy(ENEMY_TYPES::CUBE, XMFLOAT3(60.0f, 0.0f, 60.0f));


	auto cube2 = SpawnEnemy(ENEMY_TYPES::MANTA, XMFLOAT3(50.0f, 0.0f, 70.0f));
}

void GameScene::CompleteBuilds()
{
	if (1 < Instances.size())
	{
		staticStart = std::stable_partition(Instances.begin(), Instances.end()
			, [](const ObjectPtr& obj) -> bool {
			return obj->IsStatic();
		});
	}
}

Enemy* GameScene::SpawnEnemy(ENEMY_TYPES type, const XMFLOAT3& pos)
{
	switch (type)
	{
		case ENEMY_TYPES::CUBE:
		{
			Enemy* instance_ptr = CreateInstance<EnemyCube>(pos);
			instance_ptr->SetMesh(meshEnemyCube);
			instance_ptr->SetColor(RGB(255, 0, 0));

			return instance_ptr;
		}
		break;

		case ENEMY_TYPES::MANTA:
		{
			Enemy* instance_ptr = CreateInstance<EnemyManta>(pos);
			instance_ptr->SetMesh(meshEnemyManta);
			instance_ptr->SetColor(RGB(255, 0, 0));

			return instance_ptr;
		}
		break;
	}

	return nullptr;
}

void GameScene::Update(float elapsed_time)
{
	if (myPlayer)
	{
		myPlayer->Update(elapsed_time);
	}

	for (auto it = staticStart; it != Instances.end(); ++it)
	{
		auto& instance = *it;

		instance->Update(elapsed_time);
	}
}

void GameScene::PrepareRendering()
{
	GamePipeline::SetProjection(myCamera->projectionPerspective);

	std::for_each(Instances.cbegin(), Instances.cend(), [&](const auto& inst) {
		if (inst->CheckCameraBounds())
		{
			inst->PrepareRendering(*this);
		}
	});

	if (myPlayer)
	{
		myPlayer->PrepareRendering(*this);
	}

	std::sort(Fragments.begin(), Fragments.end()
		, [](const CFragment& lhs, const CFragment& rhs) {
		return (rhs.start.z < lhs.start.z) && (rhs.dest.z < lhs.dest.z);
	});
}

void GameScene::Render(HDC surface)
{
	HPEN pen = NULL;
	HPEN old_pen = NULL;

	for (const auto& frag : Fragments)
	{
		old_pen = HPEN(SelectObject(surface, ReadyPen(frag.Colour)));

		DrawSide(surface, frag.start, frag.dest);
		SelectObject(surface, old_pen);
	}

	Fragments.clear();
}

void GameScene::AddFragment(const CFragment& fragment)
{
	Fragments.push_back(fragment);
}

HPEN GameScene::ReadyPen(COLORREF color)
{
	auto it = Pens.find(color);
	if (Pens.end() == it)
	{
		auto pen = CreatePen(PS_SOLID, 0, color);
		return Pens.try_emplace(color, pen).first->second;
	}
	else
	{
		return it->second;
	}
}

CGroupPtr GameScene::CreateCollisionGroup()
{
	const size_t index = collisionAreas.size();
	auto&& ptr = make_shared<GameCollsionGroup>(*this, index, COLLIDE_AREA_H, COLLIDE_AREA_V, COLLIDE_AREA_U);
	collisionAreas.push_back(ptr);

	return ptr;
}

CGroupPtr GameScene::FindProperGroup(const XMFLOAT3& position)
{
	auto cx = Clamp(position.x, -WORLD_H * 0.5f, WORLD_H * 0.5f);
	auto cy = Clamp(position.y, -WORLD_V * 0.5f, WORLD_V * 0.5f);
	auto cz = Clamp(position.z, -WORLD_U * 0.5f, WORLD_U * 0.5f);
	XMFLOAT3 check_pos = XMFLOAT3(cx, cy, cz);

	for (auto& group : collisionAreas)
	{
		if (group->Contains(check_pos))
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
	if (inst)
	{
		inst->SetPosition(position);
		inst->SetCamera(myCamera);

		auto ptr = ObjectPtr(inst);
		Instances.push_back(ptr);

		return inst;
	}
	return nullptr;
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
	, Collider(XMFLOAT3(0, 0, 0), XMFLOAT3(0.5f * sz_x, 0.5f * height, 0.5f * sz_y))
	, Instances()
{
	Instances.reserve(100);
}

void GameCollsionGroup::SetPosition(XMFLOAT3&& position)
{
	Collider.Center = position;
}

void GameCollsionGroup::AddInstance(ObjectPtr& ptr)
{
	Instances.push_back(ptr);
}

void GameCollsionGroup::Update(float elapsed_time)
{

}

void GameCollsionGroup::PrepareRendering()
{
	//myMesh.PrepareRendering(Scene, RGB(70, 70, 70));

	for (auto& inst : Instances)
	{
		if (inst->isActivated)
		{
			inst->PrepareRendering(Scene);
		}
	}
}

void GameCollsionGroup::Render(HDC surface) const
{
	for (const auto& inst : Instances)
	{
		if (inst->isActivated)
		{
			inst->Render(surface);
		}
	}
}

bool GameCollsionGroup::Contains(const XMFLOAT3& point)
{
	auto vector = XMLoadFloat3(&point);
	auto result = Collider.Contains(vector);

	return ContainmentType::DISJOINT != result;
}
