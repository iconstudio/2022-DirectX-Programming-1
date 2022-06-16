#include "pch.hpp"
#include "StageHelliattack.hpp"
#include "GameFramework.h"
#include "Arithmetics.hpp"
#include "HelicopterPlayer.hpp"
#include "Terrains.hpp"

Transformer lightTransform{};

StageHelliattack::StageHelliattack(GameFramework& framework, HWND hwnd)
	: IlluminatedScene(framework, "Helliattack")
	, globalTime(0.0f)
	, playerSpawnPoint(), playerLightRotations()
	, myTerrain(256, 256), matrixTerrain(Matrix4x4::Identity())
	, worldWidth(0.0f), worldHeight(0.0f)
	, handleWindow(hwnd)
{}

StageHelliattack::~StageHelliattack()
{}

void StageHelliattack::ProcessInput(UCHAR pKeysBuffer[256])
{
	if (pKeysBuffer[VK_UP] & 0xF0 || pKeysBuffer['W'] & 0xF0)
	{
		myPlayer->MoveForward(80.0f * lastDeltaTime);
	}
	else if (pKeysBuffer[VK_DOWN] & 0xF0 || pKeysBuffer['S'] & 0xF0)
	{
		myPlayer->MoveForward(-60.0f * lastDeltaTime);
	}

	if (pKeysBuffer['Z'] & 0xF0)
	{
		myPlayer->MoveStrafe(60.0f * lastDeltaTime);
	}
	if (pKeysBuffer['C'] & 0xF0)
	{
		myPlayer->MoveStrafe(-60.0f * lastDeltaTime);
	}

	if (pKeysBuffer[VK_LEFT] & 0xF0 || pKeysBuffer['A'] & 0xF0)
	{
		myPlayer->Rotate(0, 40.0f * lastDeltaTime, 0);
	}
	if (pKeysBuffer[VK_RIGHT] & 0xF0 || pKeysBuffer['D'] & 0xF0)
	{
		myPlayer->Rotate(0, -40.0f * lastDeltaTime, 0);
	}

	if (pKeysBuffer['Q'] & 0xF0)
	{
		myPlayer->MoveUp(50.0f * lastDeltaTime);
	}
	else if (pKeysBuffer['E'] & 0xF0)
	{
		myPlayer->MoveUp(-40.0f * lastDeltaTime);
	}

	float cxDelta = 0.0f;
	float cyDelta = 0.0f;
	POINT ptCursorPos;

	if (GetCapture() == handleWindow)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - posCursor.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - posCursor.y) / 4.0f;
		SetCursorPos(posCursor.x, posCursor.y);
	}

	if (cxDelta || cyDelta)
	{
		const auto cam_mode = myCamera->GetMode();

		if (SPACESHIP_CAMERA == cam_mode)
		{
			myCamera->Rotate(cyDelta, cxDelta, 0.0f);
		}
		else
		{
			myCamera->Rotate(cyDelta, 0.0f, 0.0f);
			myPlayer->Rotate(0.0f, cxDelta, 0.0f);
		}
	}
}

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

	// 플레이어
	auto player = new HellicopterPlayer();
	player->Attach(model_copter1.get());
	player->SetPosition(playerSpawnPoint);
	player->LookTo(XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	player->Awake(d3dDevice, d3dTaskList);
	player->SetOriginalCollider(collider_copter);
	player->BuildCollider();
	myPlayer = player;

	// 카메라
	auto cam = player->GetCamera();
	cam->SetOffset(XMFLOAT3(0.0f, 50.0f, -90.0f));
	SetCamera(cam);

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
	light_for_player1.m_xmf3Direction = XMFLOAT3(0.0f, 0.5f, 1.0f);
	light_for_player1.m_fRange = 600.0f;
	light_for_player1.m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	light_for_player1.m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.5f, 1.0f);
	light_for_player1.m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	light_for_player1.m_xmf3Attenuation = XMFLOAT3(1.0f, 0.05f, 0.0001f);
	light_for_player1.m_fFalloff = 6.0f;
	light_for_player1.m_fPhi = std::cosf(XMConvertToRadians(70.0f));
	light_for_player1.m_fTheta = std::cosf(XMConvertToRadians(30.0f));

	auto& light_for_player2 = myLights[2];
	light_for_player2 = light_for_player1;
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

	BYTE keystate[256]{};
	if (GetKeyboardState(keystate))
	{
		ProcessInput(keystate);
	}

	if (myPlayer)
	{
		const auto player_pos = myPlayer->GetPosition();
		const auto pl_overlapped_pos = myTerrain.GetHeight(player_pos.x, player_pos.z, false);

		const auto pl_addition = pl_overlapped_pos - player_pos.y;
		if (player_pos.y < pl_overlapped_pos) // 겹침!
		{
			const auto movement = pl_addition * delta_time;

			if (1.0f <= pl_addition)
			{
				// 뒤로 물러나는 반대 속도 계산
				const auto pl_look = myPlayer->GetLook();
				const auto pl_moved = Vector3::Add(player_pos, XMFLOAT3(0, movement, 0));

				// 이동했을 때 향한 벡터
				auto mv_look = Vector3::Normalize(Vector3::Subtract(pl_moved, player_pos));
				mv_look.y = 0.0f;

				myPlayer->Translate(Vector3::ScalarProduct(mv_look, -movement * 10.0f - 0.1f));
				myPlayer->Translate(XMFLOAT3(0, movement, 0));
			}
			else
			{
				myPlayer->Translate(XMFLOAT3(0, pl_addition, 0));
			}
		}

		const auto light_deg = globalTime * 80.0f;
		const auto light_cos = std::cosf(XMConvertToRadians(light_deg));

		const auto pitch = abs(light_cos * 20.0f) - 10.0f;
		const auto yaw = light_cos * 80.0f - 40.0f;
		const auto roll = abs(light_cos * 20.0f);

		// 
		lightTransform.SetMatrix(Matrix4x4::Identity());
		lightTransform.SetPosition(player_pos);

		lightTransform.SetRotation(myPlayer->worldMatrix);
		lightTransform.Rotate(XMFLOAT3(0, 1, 0), yaw);
		lightTransform.Rotate(XMFLOAT3(1, 0, 0), pitch);
		lightTransform.Rotate(XMFLOAT3(0, 0, 1), roll);

		auto& light_for_player1 = myLights[1];
		light_for_player1.m_xmf3Position = player_pos;
		light_for_player1.m_xmf3Direction = XMFLOAT3(lightTransform.myLook);

		lightTransform.SetRotation(myPlayer->worldMatrix);
		lightTransform.Rotate(XMFLOAT3(0, 1, 0), -yaw);
		lightTransform.Rotate(XMFLOAT3(1, 0, 0), pitch);
		lightTransform.Rotate(XMFLOAT3(0, 0, 1), roll);

		auto& light_for_player2 = myLights[2];
		light_for_player2.m_xmf3Position = player_pos;
		light_for_player2.m_xmf3Direction = XMFLOAT3(lightTransform.myLook);

		if (myCamera)
		{
			const auto& campos = myCamera->GetPosition();
			const auto mid_pos = Vector3::ScalarProduct(Vector3::Add(player_pos, campos), 0.5f, false);

			const auto overlapped_pos = myTerrain.GetHeight(mid_pos.x, mid_pos.z, false);

			const auto addition = overlapped_pos - mid_pos.y;
			if (0 < addition)
			{
				myCamera->Move(XMFLOAT3(0, addition, 0));
			}
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

	// 게임 객체 대신에  지형에 변환 행렬 전달
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
