#include "stdafx.hpp"
#include "GameScene.hpp"
#include "GameFramework.hpp"
#include "GameCamera.hpp"
#include "GamePipeline.hpp"
#include "GameMesh.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"
#include "GameObject.hpp"
#include "Player.hpp"
#include "PlayerBullet.hpp"
#include "Enemy.hpp"
#include "EnemyCube.hpp"
#include "EnemyManta.hpp"
#include "Terrains.hpp"
#include "CubeMesh.hpp"
#include "PlaneMesh.hpp"

// 경계 타일의 가로 너비 (x)
constexpr float COLLIDE_AREA_H = 30.0f;
// 경계 타일의 높이 (y)
constexpr float COLLIDE_AREA_V = 10.0f;
// 경계 타일의 세로 너비 (z)
constexpr float COLLIDE_AREA_U = 30.0f;
// 선로의 길이 (z)
constexpr float RAIL_LENGTH = 16.0f;

GameScene::GameScene(GameFramework& framework)
	: Framework(framework), Window(NULL)
	, globalMatrix(Matrix4x4::Identity())
	, myInstances(100), staticInstances(200), myParticles(200), Fragments()
	, numberPillars(100), Pillars(), boardFront(nullptr), boardBack(nullptr)
	, myPlayer(nullptr), myCamera(nullptr)

	, isPlayerRiding(false), worldPlayerPositionIndex(0)
	, playerPosition(0.0f), playerWorldRelativePosition(0.0f)

	, meshPlayer(nullptr), meshPlayerBullet(nullptr)
	, meshEnemyCube(nullptr), meshEnemyManta(nullptr), meshEnemyBullet(nullptr)
	, meshPillars(), meshRail(nullptr)
{
	ZeroMemory(meshPillars, sizeof(meshPillars));

	Fragments.clear();
	staticInstances.clear();
	myParticles.clear();
	myInstances.clear();
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
	auto MakeCubeMesh = [](float h, float v, float d) -> shared_ptr<CMesh> {
		return std::move(static_pointer_cast<CMesh>(make_shared<CubeMesh>(h, v, d)));
	};

	auto MakePlaneMesh = [](float w, float h) -> shared_ptr<CMesh> {
		return std::move(static_pointer_cast<CMesh>(make_shared<PlaneMesh>(w, h)));
	};

	meshPlayer = MakeCubeMesh(5.0f, 5.0f, 5.0f);
	meshPlayerBullet = MakeCubeMesh(2.0f, 2.0f, 10.0f);

	meshEnemyCube = MakeCubeMesh(3.0f, 3.0f, 3.0f);
	meshEnemyManta = MakeCubeMesh(6.0f, 2.0f, 9.0f);
	meshEnemyBullet = MakeCubeMesh(1.0f, 1.0f, 10.0f);

	meshEntrance = MakePlaneMesh(8.0f, 4.0f);

	meshFloor = MakePlaneMesh(COLLIDE_AREA_H, COLLIDE_AREA_U);
	meshSide = MakePlaneMesh(COLLIDE_AREA_U, COLLIDE_AREA_V);

	for (int i = 0; i < 15; ++i)
	{
		meshPillars[i] = MakeCubeMesh(2.0f, 1.0f + float(i) * 1.5f, 2.0f);
	}
	meshRail = MakeCubeMesh(6.0f, 1.5f, RAIL_LENGTH);
}

void GameScene::BuildWorld()
{
	constexpr auto floor_width = (COLLIDE_AREA_H + 10.0f);
	constexpr auto floor_height = (COLLIDE_AREA_U + 10.0f);

	constexpr int boundary_cnt_x = int(WORLD_H / floor_width);
	constexpr int boundary_cnt_y = int(WORLD_V / COLLIDE_AREA_V);
	constexpr int boundary_cnt_z = int(WORLD_U / floor_height);

	float cx, cz;
	for (int i = 0; i < boundary_cnt_z; ++i)
	{
		for (int k = 0; k < boundary_cnt_x; ++k)
		{
			cx = k * floor_width - floor_width * 0.5f;
			cz = i * floor_height - floor_height * 0.5f;

			auto floor = CreateInstance<GameStaticObject>(cx, 0.0f, cz);
			floor->SetMesh(meshFloor);
			floor->SetColor(0);
		}
	}

	constexpr float pillar_place_z_gap = 14.0f;
	XMFLOAT3 place{};

	int peek = 0;
	float temp;
	float end_ratio;

	Pillars.reserve(numberPillars);
	for (int i = 0; i < numberPillars; i++)
	{
		place.x = pillarStartPoint.x + std::cos(1.0f + float(i) / PI) * 30.0f;
		place.y = pillarStartPoint.y;
		place.z = pillarStartPoint.z + i * pillar_place_z_gap;

		if (i < 10)
		{
			peek = i;
		}
		else if (i < 90)
		{
			temp = std::sin(DegreeToRadian(float(i - 10) * 10.0f)) * 6.0f;
			peek = std::min(14, 10 + int(temp));
		}
		else
		{
			// float(90 - 10) * 10.0f
			temp = std::sin(DegreeToRadian(80.0f)) * 5.0f;
			end_ratio = float(numberPillars - i) / 10.0f;
			peek = std::max(1, int((10.0 + temp) * end_ratio));
		}

		auto pillar = CreateInstance<Pillar>(place);

		pillar->SetMesh(meshPillars[peek]);
		pillar->SetHeight(1.0f + float(peek) * 1.5f);
		Pillars.push_back(pillar);
	}

	if (0 < Pillars.size())
	{
		GameTransform transform{};
		auto& world_mat = transform.GetWorldMatrix();

		Pillar* current, * before;

		XMFLOAT3 from, to, vector, right, look, up;
		Rail* rail;
		for (auto pit = Pillars.begin() + 1; pit != Pillars.end(); pit++)
		{
			current = *pit;
			before = *(pit - 1);

			current->SetBefore(before);
			before->SetNext(current);

			from = before->myTop;
			to = current->myTop;

			vector = Vector3::Subtract(to, from);
			right = transform.Right;
			look = Vector3::Normalize(vector);
			up = Vector3::Normalize(Vector3::CrossProduct(look, right));

			transform.SetPosition(from);
			transform.LookAt(to, up);
			transform.Move(XMFLOAT3(transform.GetLook()), RAIL_LENGTH * 0.5f);

			auto rail = CreateInstance<Rail>(before->myTop);
			rail->SetMesh(meshRail);
			rail->SetWorldMatrix(world_mat);

			auto chunk = make_shared<TerrainChunk>();
			chunk->from = from;
			chunk->to = to;
			chunk->vector = look;
			chunk->normal = up;
			chunk->axis = right;
			chunk->myLength = Vector3::Length(vector);
			Terrain.push_back(chunk);
		}

		auto first = Pillars.front();
		boardFront = CreateInstance<RailBorder>(first->GetPosition());
		boardFront->SetMesh(meshEntrance);

		XMFLOAT3 from_pos = first->GetPosition();
		from_pos.y = 0.0f;

		XMFLOAT3 to_pos = Vector3::Add(from_pos, XMFLOAT3(0, 0, -2));
		transform.SetPosition(from_pos);
		transform.LookAt(to_pos, GameTransform::Up);
		boardFront->SetWorldMatrix(world_mat);
		boardFront->SetExit(to_pos);
		boardFront->SetExitLook(270.0f);
		boardFront->SetIndex(0);

		auto last = Pillars.back();
		boardBack = CreateInstance<RailBorder>(last->GetPosition());
		boardBack->SetMesh(meshEntrance);

		from_pos = last->GetPosition();
		from_pos.y = 0.0f;

		to_pos = Vector3::Add(from_pos, XMFLOAT3(0, 0, -2));
		transform.SetPosition(from_pos);
		transform.LookAt(to_pos, GameTransform::Up);
		boardBack->SetWorldMatrix(world_mat);
		boardBack->SetExit(to_pos);
		boardFront->SetExitLook(90.0f);
		boardFront->SetIndex(Terrain.size() - 1);
	}
}

void GameScene::BuildObjects()
{
	myCamera->SetLocalPosition(XMFLOAT3(0.0f, 7.0f, -10.0f));
	myCamera->GenerateViewMatrix();

	myPlayer = make_shared<Player>();
	myPlayer->SetHwnd(Window);
	myPlayer->SetCamera(myCamera);
	myPlayer->SetLookOffset(XMFLOAT3(0.0f, 0.0f, myCamera->myFarPlane));
	myPlayer->SetMesh(meshPlayer);
	myPlayer->SetColor(RGB(128, 128, 255));
	myPlayer->SetPosition(playerSpawnPoint);

	const auto count = myPlayer->myBulletMax;
	for (int i = 0; i < count; ++i)
	{
		auto bullet = CreateInstance<PlayerBullet>(playerSpawnPoint);
		bullet->SetMesh(meshPlayerBullet);
		bullet->SetDamage(3.0f);

		myPlayer->AddBullet(bullet);
	}

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

	if (myPlayer)
	{
		myPlayer->UpdateBoundingBox();
		myPlayer->Start();
	}
}

void GameScene::PlayerJumpToRail(const shared_ptr<TerrainChunk>& node)
{
	worldCurrentTerrain = node;
}

void GameScene::PlayerJumpToBefore()
{
	if (0 == worldPlayerPositionIndex)
	{
		// 첫번째
	}
	else
	{
		const auto& chunk = Terrain.at(--worldPlayerPositionIndex);
		playerPosition += chunk->myLength;

		PlayerJumpToRail(chunk);
	}
}

void GameScene::PlayerJumpToNext()
{
	if (numberPillars <= worldPlayerPositionIndex - 1)
	{
		// 마지막
	}
	else
	{
		const auto& chunk = Terrain.at(++worldPlayerPositionIndex);
		playerPosition -= chunk->myLength;

		PlayerJumpToRail(chunk);
	}
}

bool GameScene::PlayerMoveOnRail(float value)
{
	playerPosition += value;

	const auto& rail_length = worldCurrentTerrain->myLength;
	playerWorldRelativePosition = playerPosition / rail_length;

	if (rail_length < playerPosition)
	{
		PlayerJumpToBefore();
		return true;
	}
	else if (playerPosition < 0)
	{
		PlayerJumpToNext();
		return true;
	}
	else
	{
		return false;
	}
}

void GameScene::Update(float elapsed_time)
{
	if (myPlayer)
	{
		if (isPlayerRiding)
		{
			const auto& chunk = worldCurrentTerrain;
			const auto& from = chunk->from;
			const auto& to = chunk->to;
			const auto& look = chunk->vector;
			const auto& up = chunk->normal;
			const auto& len = chunk->myLength * playerWorldRelativePosition;

			const auto& vector = Vector3::ScalarProduct(look, len);
			const auto& newpos = Vector3::Add(from, vector);

			myPlayer->SetPosition(newpos);
		}
		else
		{

		}
		myPlayer->Update(elapsed_time);
	}

	for (auto it = myInstances.begin(); it != myInstances.end();)
	{
		auto& instance = *it;

		if (instance->IsActivated())
		{
			if (instance->isKilled)
			{
				it = myInstances.erase(it);
				continue;
			}
			else
			{
				for (auto& other : myInstances)
				{
					auto other_ptr = other.get();
					if (other != instance
						&& other->isActivated
						&& instance->CheckCollideWith(other_ptr))
					{
						instance->OnCollisionEnter(other_ptr);
						other->OnCollisionEnter(instance.get());
					}
				}

				instance->Update(elapsed_time);
			}
		}

		it++;
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
Type* GameScene::CreateInstance()
{
	return CreateInstance<Type>(std::move(XMFLOAT3()));
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

		if constexpr (std::is_base_of_v<GameParticle, Type>)
		{
			myParticles.push_back(EffectPtr(inst));
		}
		else if constexpr (std::is_base_of_v<GameStaticObject, Type>)
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
			switch (wp)
			{
				case 'A':
				case 'W':
				{
					if (isPlayerRiding)
					{
						if (PlayerMoveOnRail(playerMoveSpeed))
						{
							
						}
					}
				}
				break;

				case 'D':
				case 'S':
				{
					if (isPlayerRiding)
					{
						if (PlayerMoveOnRail(-playerMoveSpeed))
						{

						}
					}
				}
				break;

				case VK_SPACE:
				{
					if (myPlayer)
					{
						bool is_normal = (PLAYER_STATES::NORMAL == myPlayer->myStatus);
						bool is_riding = (PLAYER_STATES::RIDING == myPlayer->myStatus);
						if (myPlayer->CheckCollideWith(boardFront))
						{
							if (is_normal)
							{
								myPlayer->RideOn(boardFront);
								worldCurrentTerrain = Terrain.at(boardFront->myIndex);
								isPlayerRiding = true;
							}
							else if (is_riding)
							{
								myPlayer->TakeOff(boardFront);
								isPlayerRiding = false;
							}
						}
						else if (myPlayer->CheckCollideWith(boardBack))
						{
							if (is_normal)
							{
								myPlayer->RideOn(boardBack);
								worldCurrentTerrain = Terrain.at(boardFront->myIndex);
								isPlayerRiding = true;
							}
							else if (is_riding)
							{
								myPlayer->TakeOff(boardBack);
								isPlayerRiding = false;
							}
						}
					}
				}
				break;

				case VK_HOME:
				{
					if (myPlayer)
					{
						myPlayer->SetPosition(boardFront->GetPosition());
						myPlayer->MoveUp(2.5f);
					}
				}
				break;

				case VK_END:
				{
					if (myPlayer)
					{
						myPlayer->SetPosition(boardBack->GetPosition());
						myPlayer->MoveUp(2.5f);
					}
				}
				break;

				case 'R':
				{

				}
				break;
			}
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
