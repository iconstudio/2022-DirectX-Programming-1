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
	, Instances(), staticStart(), Fragments()
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

		pillar->SetMesh(meshPillars[int(i) % 16]);
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
	myCamera->GenerateViewMatrix();

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

void GameScene::Kill(GameObject* obj)
{}

void GameScene::Kill(ObjectPtr& obj)
{}

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

	for (auto it = Instances.begin(); it != Instances.end(); ++it)
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
			//PrepareRenderingCollider(inst->Collider);
		}
	});

	if (myPlayer)
	{
		myPlayer->PrepareRendering(*this);
		//PrepareRenderingCollider(myCamera->StaticCollider);
		//PrepareRenderingCollider(myCamera->Collider);
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

void GameScene::PrepareRenderingCollider(const BoundingBox& collider)
{}

void GameScene::PrepareRenderingCollider(const BoundingOrientedBox& collider)
{
	auto corners = make_unique<XMFLOAT3[]>(collider.CORNER_COUNT + 1);

	auto place = corners.get();
	collider.GetCorners(place);
	place[collider.CORNER_COUNT] = place[0];

	int j = 0;
	for (int i = 0; i <= 4; ++i)
	{
		if (4 == i)
			j = 0;
		else
			j = i;

		const auto& vtx_from = GamePipeline::ProjectTransform(place[j]);
		const auto& vtx_to = GamePipeline::ProjectTransform(place[j + 1]);
		const auto& vtx_far = GamePipeline::ProjectTransform(place[j + 4]);

		const auto inside_from = CheckProjection(vtx_from.x, vtx_from.y);
		const auto inside_to = CheckProjection(vtx_to.x, vtx_to.y);
		const auto inside_far = CheckProjection(vtx_far.x, vtx_far.y);

		if (CheckDepth(vtx_from.z) && CheckDepth(vtx_to.z))
			AddFragment(CFragment{ vtx_from, vtx_to, RGB(128, 128, 128) });

		if (CheckDepth(vtx_from.z) && CheckDepth(vtx_far.z))
			AddFragment(CFragment{ vtx_from, vtx_far, RGB(128, 128, 128) });

		if (CheckDepth(vtx_to.z) && CheckDepth(vtx_far.z))
			AddFragment(CFragment{ vtx_to, vtx_far, RGB(128, 128, 128) });
	}
}

void GameScene::PrepareRenderingCollider(const BoundingFrustum& collider)
{
	auto corners = make_unique<XMFLOAT3[]>(collider.CORNER_COUNT + 1);

	auto place = corners.get();
	collider.GetCorners(place);
	place[collider.CORNER_COUNT] = place[0];

	int j = 0;
	for (int i = 0; i <= 4; ++i)
	{
		if (4 == i)
			j = 0;
		else
			j = i;

		const auto& vtx_from = GamePipeline::ProjectTransform(place[j]);
		const auto& vtx_to = GamePipeline::ProjectTransform(place[j + 1]);
		const auto& vtx_far = GamePipeline::ProjectTransform(place[j + 4]);

		const auto inside_from = CheckProjection(vtx_from.x, vtx_from.y);
		const auto inside_to = CheckProjection(vtx_to.x, vtx_to.y);
		const auto inside_far = CheckProjection(vtx_far.x, vtx_far.y);

		AddFragment(CFragment{ vtx_from, vtx_to, RGB(128, 128, 128) });
		AddFragment(CFragment{ vtx_from, vtx_far, RGB(128, 128, 128) });
		AddFragment(CFragment{ vtx_to, vtx_far, RGB(128, 128, 128) });
	}
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

		Instances.push_back(ObjectPtr(inst));

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
