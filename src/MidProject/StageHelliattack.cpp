#include "pch.hpp"
#include "StageHelliattack.hpp"
#include "GameFramework.h"
#include "Arithmetics.hpp"
#include "HelicopterPlayer.hpp"
#include "Terrains.hpp"

StageHelliattack::StageHelliattack(GameFramework& framework, HWND hwnd)
	: IlluminatedScene(framework, "Helliattack")
	, globalTime(0.0f)
	, playerSpawnPoint(), playerLightRotations()
	, myTerrain(256, 256), matrixTerrain(Matrix4x4::Identity())
	, handleWindow(hwnd)
{}

StageHelliattack::~StageHelliattack()
{}

void StageHelliattack::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	IlluminatedScene::Awake(device, cmdlist);

	constexpr float wscale = 10.0f;
	constexpr float hscale = 10.0f;

	// 지형 불러오기
	myTerrain.Awake("Resources/HeightMap.raw");
	myTerrain.Start(d3dDevice, d3dTaskList, { wscale, 1000.0f, hscale });

	worldWidth = myTerrain.myData.myMapWidth * wscale;
	worldHeight = myTerrain.myData.myMapHeight * hscale;

	XMFLOAT4 orientation{};
	XMStoreFloat4(&orientation, XMQuaternionIdentity());

	auto collider_copter = make_shared<BoundingOrientedBox>(XMFLOAT3()
		, XMFLOAT3(9.0f, 12.0f, 18.0f)
		, orientation);

	auto model_copter1 = myFramework.GetModel("Hellicoter1").lock();
	model_copter1->SetPosition(0.0f, 0.0f, 0.0f);
	model_copter1->SetScale(1.0f, 1.0f, 1.0f);

	playerSpawnPoint = XMFLOAT3(worldWidth * 0.5f, 350.0f, worldHeight * 0.5f);

	auto player = new HellicopterPlayer();
	player->Attach(model_copter1.get());
	player->Awake(d3dDevice, d3dTaskList);
	player->SetOriginalCollider(collider_copter);
	player->BuildCollider();
	player->SetPosition(playerSpawnPoint);
	player->LookTo(XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	auto cam = player->GetCamera();
	cam->SetPosition(playerSpawnPoint);
	cam->SetOffset(XMFLOAT3(0.0f, 50.0f, -90.0f));

	SetCamera(cam);
	myPlayer = player;

	//
	m_xmf4GlobalAmbient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	//
	numberLights = 5;
	myLights = new CLight[numberLights];
	ZeroMemory(myLights, sizeof(CLight) * numberLights);

	auto& light_global = myLights[0];
	light_global.m_bEnable = true;
	light_global.m_nType = DIRECTIONAL_LIGHT;
	light_global.m_xmf4Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	light_global.m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	light_global.m_xmf4Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	light_global.m_xmf3Direction = XMFLOAT3(0.3f, -0.5f, 0.5f);

	auto& light_for_player1 = myLights[1];
	light_for_player1.m_bEnable = true;
	light_for_player1.m_nType = SPOT_LIGHT;
	light_for_player1.m_xmf3Position = playerSpawnPoint;
	light_for_player1.m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	light_for_player1.m_fRange = 600.0f;
	light_for_player1.m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	light_for_player1.m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.5f, 1.0f);
	light_for_player1.m_xmf4Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	light_for_player1.m_xmf3Attenuation = XMFLOAT3(1.0f, 0.05f, 0.0001f);
	light_for_player1.m_fFalloff = 6.0f;
	light_for_player1.m_fPhi = std::cosf(XMConvertToRadians(70.0f));
	light_for_player1.m_fTheta = std::cosf(XMConvertToRadians(30.0f));

	auto& light_for_player2 = myLights[2];
	light_for_player2 = light_for_player1;
	/*light_for_player2.m_bEnable = true;
	light_for_player2.m_nType = SPOT_LIGHT;
	light_for_player2.m_xmf3Position = playerSpawnPoint;
	light_for_player2.m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	light_for_player2.m_fRange = 600.0f;
	light_for_player2.m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	light_for_player2.m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.5f, 1.0f);
	light_for_player2.m_xmf4Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	light_for_player2.m_xmf3Attenuation = XMFLOAT3(1.0f, 0.05f, 0.0001f);
	light_for_player2.m_fFalloff = 6.0f;
	light_for_player2.m_fPhi = std::cosf(XMConvertToRadians(70.0f));
	light_for_player2.m_fTheta = std::cosf(XMConvertToRadians(30.0f));*/
}

void StageHelliattack::Start()
{
	IlluminatedScene::Start();
}

void StageHelliattack::Reset()
{
	IlluminatedScene::Reset();
}

void StageHelliattack::Update(float delta_time)
{
	globalTime += delta_time;

	IlluminatedScene::Update(delta_time);

	if (myCamera)
	{
		const auto& campos = myCamera->GetPosition();
		const auto overlapped_pos = myTerrain.GetHeight(campos.x, campos.z);

		if (campos.y < overlapped_pos)
		{

		}
	}
}

void StageHelliattack::PrepareRendering()
{
	IlluminatedScene::PrepareRendering();
}

void StageHelliattack::Render() const
{
	IlluminatedScene::Render();

	// 게임 객체 대신에 변환 행렬 전달
	d3dTaskList->SetGraphicsRoot32BitConstants(1, 16, &matrixTerrain, 0);

	myTerrain.Render(d3dTaskList);
}

void StageHelliattack::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
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
			GetCursorPos(&posCursor);
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

void StageHelliattack::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			switch (key)
			{
				case VK_F1:
				{
					auto cam = myPlayer->GetCamera();
					cam->SetOffset(XMFLOAT3(0.0f, 500.0f, -900.0f));
				}
				break;

				case VK_F2:
				{
					auto cam = myPlayer->GetCamera();
					cam->SetOffset(XMFLOAT3(0.0f, 50.0f, -90.0f));
				}
				break;

				case VK_F5:
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

void StageHelliattack::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageHelliattack::ProcessInput(UCHAR * pKeysBuffer)
{}
