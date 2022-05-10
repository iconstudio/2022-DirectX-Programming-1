#include "stdafx.hpp"
#include "GameScene.hpp"
#include "GameFramework.hpp"
#include "GameCamera.hpp"
#include "GamePipeline.hpp"
#include "GameEntity.hpp"
#include "GameObject.hpp"
#include "GameStaticObject.hpp"
#include "GameMesh.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"
#include "Player.hpp"
#include "PlayerBullet.hpp"
#include "Enemy.hpp"
#include "EnemyCube.hpp"
#include "EnemyManta.hpp"
#include "Terrains.hpp"
#include "CubeMesh.hpp"
#include "PlaneMesh.hpp"

GameScene::GameScene(GameFramework& framework, int sz_x, int height, int sz_y)
	: Framework(framework), Window(NULL)
	, collisionAreaIndex(0), worldPlayerPositionIndex(0)
	, playerPosition(0.0f), playerWorldRelativePosition(0)
	//, worldBoundary{ -sz_x / 2, -sz_y / 2, sz_x / 2, sz_y / 2 }
	, globalMatrix(Matrix4x4::Identity())
	, myInstances(), staticInstances(), Fragments()
	, myPlayer(nullptr), myCamera(nullptr)
	, meshPlayer(nullptr), meshPlayerBullet(nullptr)
	, meshEnemyCube(nullptr), meshEnemyManta(nullptr), meshEnemyBullet(nullptr)
	, meshPillars(), meshRail(nullptr)
{
	Fragments.reserve(300);
	staticInstances.reserve(100);
	myInstances.reserve(300);
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
	meshPlayerBullet = static_pointer_cast<CMesh>(make_shared<CubeMesh>(2.0f, 2.0f, 10.0f));

	meshEnemyCube = static_pointer_cast<CMesh>(make_shared<CubeMesh>(3.0f, 3.0f, 3.0f));
	meshEnemyManta = static_pointer_cast<CMesh>(make_shared<CubeMesh>(6.0f, 2.0f, 9.0f));
	meshEnemyBullet = static_pointer_cast<CMesh>(make_shared<CubeMesh>(1.0f, 1.0f, 10.0f));

	meshFloor = static_pointer_cast<CMesh>(make_shared<PlaneMesh>(float(COLLIDE_AREA_H), float(COLLIDE_AREA_U)));
	meshSide = static_pointer_cast<CMesh>(make_shared<PlaneMesh>(float(COLLIDE_AREA_U), float(COLLIDE_AREA_V)));

	for (int i = 0; i < 15; ++i)
	{
		meshPillars[i] = static_pointer_cast<CMesh>(make_shared<CubeMesh>(2.0f, float(i), 2.0f));
	}
	meshRail = static_pointer_cast<CMesh>(make_shared<CubeMesh>(1.0f, 1.5f, 10.0f));
}

void GameScene::BuildWorld()
{
	constexpr int boundary_cnt_x = WORLD_H / COLLIDE_AREA_H;
	constexpr int boundary_cnt_y = WORLD_V / COLLIDE_AREA_V;
	constexpr int boundary_cnt_z = WORLD_U / COLLIDE_AREA_U;

	float cx, cz;
	for (int i = 0; i < boundary_cnt_z; ++i)
	{
		for (int k = 0; k < boundary_cnt_x; ++k)
		{
			cx = k * COLLIDE_AREA_H - COLLIDE_AREA_H * 0.5f;
			cz = i * COLLIDE_AREA_U - COLLIDE_AREA_U * 0.5f;

			auto floor = CreateInstance<GameStaticObject>(k * 50.0f, 0.0f, i * 20.0f);
			floor->SetMesh(meshFloor);
			floor->SetColor(0);
		}
	}

	//constexpr float pillar_count = 40.0f;
	constexpr float pillar_place_z_gap = 8.0f;
	XMFLOAT3 place{};

	for (float i = 0.0f; i < 100; i++)
	{
		place.x = pillarStartPoint.x + std::cos(1.0f + i / PI) * 5.0f;
		place.y = pillarStartPoint.y;
		place.z = pillarStartPoint.z + i * pillar_place_z_gap;

		auto pillar = CreateInstance<Pillar>(place);
		pillar->SetMesh(meshPillars[int(i) % 15]);
		pillar->SetColor(RGB(110, 30, 30));
	}
}

void GameScene::BuildObjects()
{
	myPlayer = make_shared<Player>();
	myPlayer->SetHwnd(Window);
	myPlayer->SetPosition(playerSpawnPoint);
	myPlayer->SetMesh(meshPlayer);
	myPlayer->SetColor(RGB(0, 0, 255));
	myPlayer->SetCamera(myCamera);

	const auto count = myPlayer->myBulletMax;
	for (int i = 0; i < count; ++i)
	{
		auto bullet = CreateInstance<PlayerBullet>(playerSpawnPoint);
		bullet->SetMesh(meshPlayerBullet);
		bullet->SetColor(0);
		bullet->SetDamage(3.0f);

		myPlayer->AddBullet(bullet);
	}

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
	for (auto& inst : staticInstances)
	{
		inst->UpdateBoundingBox();
	}

	for (auto& inst : myInstances)
	{
		inst->UpdateBoundingBox();
	}
}

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

	for (auto& instance : myInstances)
	{
		if (instance->IsActivated())
		{
			instance->Update(elapsed_time);
		}
	}
}

void GameScene::PrepareRendering()
{
	GamePipeline::SetProjection(myCamera->projectionPerspective);

	for (const auto& instance : staticInstances)
	{
		if (instance->IsActivated() && CheckCameraBounds(instance.get()))
		{
			instance->PrepareRendering(*this);
		}
	}

	for (const auto& instance : myInstances)
	{
		if (instance->IsActivated() && CheckCameraBounds(instance.get()))
		{
			instance->PrepareRendering(*this);
			//PrepareRenderingCollider(inst->Collider);
		}
	}

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

bool GameScene::CheckCameraBounds(const GameEntity* obj) const
{
	return myCamera->IsInFrustum(obj->Collider);
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

		if (inside_from && inside_to)
			AddFragment(CFragment{ vtx_from, vtx_to, RGB(128, 128, 128) });
		if (inside_from && inside_far)
			AddFragment(CFragment{ vtx_from, vtx_far, RGB(128, 128, 128) });
		if (inside_to && inside_far)
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
	auto inst = new Type();
	if (inst)
	{
		inst->SetPosition(std::forward<XMFLOAT3>(position));

		if constexpr (std::is_base_of_v<GameStaticObject, Type>)
		{
			staticInstances.push_back(StaticPtr(inst));
		}
		else
		{
			myInstances.push_back(ObjectPtr(inst));
		}

		return inst;
	}
	else
	{
		throw "인스턴스 생성 오류!";
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
		{

		}
		break;

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
