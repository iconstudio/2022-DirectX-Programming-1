#include "pch.hpp"
#include "StageGame.hpp"
#include "GameFramework.h"
#include "Object.h"
#include "Vehicles.hpp"
#include "Terrains.hpp"
#include "Obstacles.hpp"
#include "Arithmetics.hpp"
#include "Mesh.h"
#include "Vertex.hpp"
#include "Transformer.hpp"

constexpr float roadWidth = 100.0f;
constexpr float roadHeight = 3000.0f;
Transformer roadTransform{};

float MakeRandom()
{
	return (float(rand()) / float(RAND_MAX));
}

StageGame::StageGame(GameFramework& framework, HWND hwnd)
	: IlluminatedScene(framework, "Game")
	, myGoalie(), playerSpawnPoint()
	, roadData(), roadMesh(nullptr)
	, roadStartPoint(), roadDestPoint()
	, handleWindow(hwnd)
	, raceColors
{ { 0.2f, 0.2f, 0.2f, 1.0f }, { 0.4f, 0.6f, 0.6f, 1.0f }
, { 0.3f, 0.8f, 0.6f, 1.0f }, {0.2f, 0.6f, 0.4f, 1.0f } }
{
	SetBackgroundColor(raceColors[0]);
}

StageGame::~StageGame()
{}

void StageGame::ProcessInput(UCHAR* pKeysBuffer)
{
	float pitch = 0.0f;

	DWORD dwDirection = 0;
	if (pKeysBuffer[VK_UP] & 0xF0 || pKeysBuffer['W'] & 0xF)
	{
		dwDirection |= DIR_FORWARD;
	}
	if (pKeysBuffer[VK_DOWN] & 0xF0 || pKeysBuffer['S'] & 0xF0)
	{
		dwDirection |= DIR_BACKWARD;
	}

	if (pKeysBuffer[VK_LEFT] & 0xF0 || pKeysBuffer['A'] & 0xF0)
	{
		pitch -= 1.0f;
	}
	if (pKeysBuffer[VK_RIGHT] & 0xF0 || pKeysBuffer['D'] & 0xF0)
	{
		pitch += 1.0f;
	}

	if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
	if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

	float cxDelta = 0.0f;
	POINT ptCursorPos;

	if (GetCapture() == handleWindow)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - posCursor.x) / 3.0f;
		SetCursorPos(posCursor.x, posCursor.y);
	}

	cxDelta += pitch * 20.0f * lastDeltaTime;

	if ((dwDirection != 0) || (cxDelta != 0.0f))
	{
		if (cxDelta)
		{
			myPlayer->Rotate(0.0f, cxDelta, 0.0f);
		}

		if (dwDirection)
		{
			myPlayer->Move(dwDirection, 2.0f, true);
		}
	}
}

void StageGame::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	IlluminatedScene::Awake(device, cmdlist);
	//
	m_xmf4GlobalAmbient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//
	numberLights = 6;
	myLights = new CLight[numberLights];
	::ZeroMemory(myLights, sizeof(CLight) * numberLights);

	myLights[0].m_bEnable = true;
	myLights[0].m_nType = POINT_LIGHT;
	myLights[0].m_fRange = 1000.0f;
	myLights[0].m_xmf4Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	myLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	myLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	myLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	myLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	myLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	myLights[1].m_bEnable = true;
	myLights[1].m_nType = SPOT_LIGHT;
	myLights[1].m_fRange = 500.0f;
	myLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	myLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	myLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	myLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	myLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	myLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	myLights[1].m_fFalloff = 8.0f;
	myLights[1].m_fPhi = (float)cos(XMConvertToRadians(45.0f));
	myLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	myLights[2].m_bEnable = true;
	myLights[2].m_nType = DIRECTIONAL_LIGHT;
	myLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	myLights[2].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.8f, 0.6f, 1.0f);
	myLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	myLights[2].m_xmf3Direction = XMFLOAT3(1.0f, -0.5f, 0.0f);

	myLights[3].m_bEnable = true;
	myLights[3].m_nType = SPOT_LIGHT;
	myLights[3].m_fRange = 600.0f;
	myLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	myLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	myLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	myLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	myLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	myLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	myLights[3].m_fFalloff = 8.0f;
	myLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	myLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	myLights[4].m_bEnable = true;
	myLights[4].m_nType = POINT_LIGHT;
	myLights[4].m_fRange = 300.0f;
	myLights[4].m_xmf4Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	myLights[4].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.9f, 1.0f);
	myLights[4].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	myLights[4].m_xmf3Position = XMFLOAT3(roadWidth * 0.5f, 0.0f, roadHeight * 0.5f);
	myLights[4].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	myLights[4].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	// 결승선
	XMFLOAT3 goal = XMFLOAT3(roadWidth * 0.5f, 0.0f, roadHeight);

	// 도로 생성
	roadStartPoint = XMFLOAT3(roadWidth * 0.5f, 0.0f, -10.0f);
	roadDestPoint = goal;

	constexpr auto black = XMFLOAT4{ 0.0f, 0.0f, 0.0f, 1.0f };

	constexpr auto lt_pos = CVertex(0.0f, 0.0f, 0.0f);
	constexpr auto lt = CDiffusedVertex(lt_pos, black);
	constexpr auto rt = CDiffusedVertex(CVertex(roadWidth, 0.0f, 0.0f), black);
	constexpr auto lb = CDiffusedVertex(CVertex(0.0f, 0.0f, roadHeight), black);
	constexpr auto rb = CDiffusedVertex(CVertex(roadWidth, 0.0f, roadHeight), black);
	roadData.AddVertex(lt);
	roadData.AddVertex(lb);
	roadData.AddVertex(rt);
	roadData.AddVertex(rb);

	roadMesh = new CMaterialMesh(device, cmdlist, roadData);
	if (!roadMesh)
	{
		throw "도로 생성 실패!";
	}
	roadMesh->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	auto road_mat = make_shared<CMaterial>();
	road_mat->m_xmf4Diffuse = black;
	road_mat->SetShader(Pipeline::illuminatedShader);

	roadMesh->AddMaterial(road_mat);
	roadMesh->myDefaultMaterial = road_mat;
	//roadMesh->SetMaterial(0, road_mat);
	//roadMesh->AssignShader(0, Pipeline::illuminatedShader);

	myLights[5].m_bEnable = true;
	myLights[5].m_nType = POINT_LIGHT;
	myLights[5].m_fRange = 100.0f;
	myLights[5].m_xmf4Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	myLights[5].m_xmf4Diffuse = XMFLOAT4(0.1f, 1.0f, 0.5f, 1.0f);
	myLights[5].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	myLights[5].m_xmf3Position = XMFLOAT3(goal);
	myLights[5].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	myLights[5].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	//
	myInstances.reserve(100);
	myInstances.clear();

	auto model_rock1 = myFramework.GetModel("Rock1").lock();
	model_rock1->SetScale(15.0f, 15.0f, 15.0f);

	auto model_rock2 = myFramework.GetModel("Rock2").lock();
	model_rock2->SetScale(15.0f, 15.0f, 15.0f);

	XMFLOAT4 orientation{};
	XMStoreFloat4(&orientation, XMQuaternionIdentity());

	auto collider_rock = make_shared<BoundingOrientedBox>(XMFLOAT3()
		, XMFLOAT3(6.0f, 6.0f, 6.0f)
		, orientation);

	XMFLOAT3 stone_place;

	constexpr float width = 100.0f;
	constexpr float height = 3000.0f;
	constexpr int count = 50;
	constexpr auto place_gap = height / float(count);

	Rock* stone = nullptr;
	for (int i = 0; i < 2; ++i)
	{
		stone_place = XMFLOAT3{ i * width, 0.0f, 0.0f };

		for (int j = 0; j < 100; ++j)
		{
			stone = new Rock();
			stone->Attach(model_rock1.get());
			stone->SetOriginalCollider(collider_rock);
			stone->BuildCollider();
			stone->SetPosition(stone_place);
			stone->Rotate(0.0f, 90.0f, 0.0f);
			myInstances.emplace_back(stone);

			stone_place.z += place_gap;

			stone = new Rock();
			stone->Attach(model_rock2.get());
			stone->SetOriginalCollider(collider_rock);
			stone->BuildCollider();
			stone->SetPosition(stone_place);
			stone->Rotate(0.0f, -90.0f, 0.0f);
			myInstances.emplace_back(stone);

			stone_place.z += place_gap;
		}
	}

	auto collider_car = make_shared<BoundingOrientedBox>(XMFLOAT3()
		, XMFLOAT3(9.0f, 12.0f, 18.0f)
		, orientation);

	auto model_policecar = myFramework.GetModel("PoliceCar").lock();
	model_policecar->SetScale(8.5f, 8.5f, 8.5f);

	auto policecar = new Vehicles();
	policecar->Attach(model_policecar.get());
	policecar->SetOriginalCollider(collider_car);
	policecar->BuildCollider();
	policecar->SetPosition(135.0f, 0.0f, 620.0f);
	policecar->Rotate(0.0f, -50.0f, 0.0f);
	myInstances.emplace_back(policecar);

	policecar = new Vehicles();
	policecar->Attach(model_policecar.get());
	policecar->SetOriginalCollider(collider_car);
	policecar->BuildCollider();
	policecar->SetPosition(-15.0f, 0.0f, 130.0f);
	policecar->Rotate(0.0f, 60.0f, 0.0f);
	myInstances.emplace_back(policecar);

	policecar = new Vehicles();
	policecar->Attach(model_policecar.get());
	policecar->SetOriginalCollider(collider_car);
	policecar->BuildCollider();
	policecar->SetPosition(140.0f, 0.0f, 1750.0f);
	policecar->Rotate(0.0f, 200.0f, 0.0f);
	myInstances.emplace_back(policecar);

	auto model_tree = myFramework.GetModel("Tree").lock();
	model_tree->SetScale(9.5f, 9.5f, 9.5f);

	auto collider_tree = make_shared<BoundingOrientedBox>(XMFLOAT3()
		, XMFLOAT3(6.0f, 30.0f, 6.0f)
		, orientation);

	float cx = 0.0f;
	float cz = 0.0f;
	Tree* tree = nullptr;
	for (int i = 0; i < 10; ++i)
	{
		tree = new Tree();
		tree->Attach(model_tree.get());
		tree->SetOriginalCollider(collider_tree);
		tree->BuildCollider();
		tree->SetPosition(150.0f + cx, 0.0f, 50.0f + cz);
		tree->Rotate(0.0f, MakeRandom() * 360.0f, 0.0f);
		myInstances.emplace_back(tree);

		cx = MakeRandom() * 50.0f - 25.0f;
		cz += MakeRandom() * 30.0f + 20.0f;
	}

	for (int i = 0; i < 10; ++i)
	{
		tree = new Tree();
		tree->Attach(model_tree.get());
		tree->SetOriginalCollider(collider_tree);
		tree->BuildCollider();
		tree->SetPosition(-110.0f + cx, 0.0f, 2490.0f + cz);
		tree->Rotate(0.0f, MakeRandom() * 360.0f, 0.0f);
		myInstances.emplace_back(tree);

		cx = MakeRandom() * 50.0f - 25.0f;
		cz += MakeRandom() * 60.0f + 10.0f;
	}

	auto model_rallycar = myFramework.GetModel("RallyCar").lock();
	model_rallycar->SetPosition(-8.0f, -0.0f, 0.0f);
	model_rallycar->SetScale(10.0f, 10.0f, 10.0f);

	playerSpawnPoint = XMFLOAT3(width * 0.5f, 0.0f, -3.0f);

	auto player = new CAirplanePlayer();
	player->Awake(d3dDevice, d3dTaskList);
	player->Attach(model_rallycar.get());
	player->SetOriginalCollider(collider_car);
	player->BuildCollider();
	player->SetPosition(playerSpawnPoint);
	player->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	SetCamera(player->GetCamera());
	myPlayer = player;

	myGoalie.Center = goal;
	myGoalie.Radius = 5.0f;

	auto model_cactus = myFramework.GetModel("Cactus").lock();
	model_cactus->SetPosition(0.0f, 0.0f, 0.0f);
	model_cactus->SetScale(30.0f, 30.0f, 30.0f);

	auto goaline = new GameObject();
	goaline->Attach(model_cactus.get());
	goaline->SetPosition(goal);
	myInstances.emplace_back(goaline);

	goaline = new GameObject();
	goaline->Attach(model_cactus.get());
	goaline->SetPosition(Vector3::Add(goal, XMFLOAT3(-20.0f, 0.0f, 0.0f)));
	myInstances.emplace_back(goaline);

	goaline = new GameObject();
	goaline->Attach(model_cactus.get());
	goaline->SetPosition(Vector3::Add(goal, XMFLOAT3(+20.0f, 0.0f, 0.0f)));
	myInstances.emplace_back(goaline);
}

void StageGame::Start()
{
	IlluminatedScene::Start();
}

void StageGame::Reset()
{
	IlluminatedScene::Reset();

	myPlayer->SetPosition(playerSpawnPoint);
	myPlayer->SetVelocity(XMFLOAT3());
	raceTimer = 4.0f;
}

void StageGame::Update(float delta_time)
{
	IlluminatedScene::Update(delta_time);

	raceTimer -= delta_time;

	if (raceTimer <= 0)
	{
		BYTE keystate[256]{};
		ZeroMemory(keystate, sizeof(keystate));

		if (GetKeyboardState(keystate))
		{
			ProcessInput(keystate);
		}

		auto& player_collider = myPlayer->myCollider;
		player_collider->Center = myPlayer->GetPosition();

		if (player_collider->Intersects(myGoalie))
		{
			myFramework.JumpToNextStage();
		}

		SetBackgroundColor(defaultColor);
	}
	else
	{
		const int raceApproxtime = int(raceTimer);
		const int raceIndex = 3 - std::min(3, raceApproxtime);

		SetBackgroundColor(raceColors[raceIndex]);
	}
}

void StageGame::PrepareRendering()
{
	IlluminatedScene::PrepareRendering();
}

void StageGame::Render() const
{
	IlluminatedScene::Render();

	XMFLOAT4X4 xmf4x4World{};

	const auto& road_mat = roadTransform.GetMatrix();
	const auto my_mat = XMLoadFloat4x4(&road_mat);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(my_mat));

	// 두번째 루트 매개인자에서 0번째 메모리에 float 16개 전달
	d3dTaskList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

	roadMesh->Render(d3dTaskList);
}

void StageGame::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageGame::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		{
			SetCapture(hwnd);
			GetCursorPos(&posCursor);
		}
		break;

		case WM_RBUTTONDOWN:
		{
		}
		break;

		case WM_LBUTTONUP:
		{
			ReleaseCapture();
		}
		break;

		case WM_RBUTTONUP:
		{
		}
		break;

		case WM_MOUSEMOVE:
		{
		}
		break;

		default:
		{}
		break;
	}
}

void StageGame::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
	auto& something = myInstances.at(0);

	switch (msg)
	{
		case WM_KEYDOWN:
		{
			switch (key)
			{
				case VK_F1:
				case VK_F2:
				case VK_F3:
				{
					//myCamera = myPlayer->ChangeCamera((DWORD)(key - VK_F1 + 1), 1.0f);
				}
				break;

				case VK_UP:
				case 'W':
				{
				}
				break;

				case VK_DOWN:
				case 'S':
				{
				}
				break;

				case VK_F4:
				{
					myFramework.JumpToNextStage();
				}
				break;
			}
			break;
		}

		default:
		{}
		break;
	}
}
