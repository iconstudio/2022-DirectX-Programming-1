#include "pch.hpp"
#include "StageGame.hpp"
#include "GameFramework.h"

StageGame::StageGame(GameFramework& framework, HWND hwnd)
	: IlluminatedScene(framework, hwnd, "Game")
	, myGoal()
{
	constexpr float colors[] = { 0.0f, 0.125f, 0.3f, 1.0f };
	SetBackgroundColor(colors);
}

void StageGame::ProcessInput(UCHAR* pKeysBuffer)
{
	DWORD dwDirection = 0;
	if (pKeysBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeysBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeysBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
	if (pKeysBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;

	if (pKeysBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeysBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeysBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
	if (pKeysBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;

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

	if ((dwDirection != 0) || (cxDelta != 0.0f))
	{
		if (cxDelta)
		{
			myPlayer->Rotate(0.0f, cxDelta, 0.0f);
		}

		if (dwDirection)
		{
			myPlayer->Move(dwDirection, 1.5f, true);
		}
	}
}

void StageGame::Awake(P3DDevice device, P3DGrpCommandList cmd_list)
{
	IlluminatedScene::Awake(device, cmd_list);
}

void StageGame::Start()
{
	IlluminatedScene::Start();
}

void StageGame::Reset()
{
	IlluminatedScene::Reset();
}

void StageGame::Update(float elapsed_time)
{
	IlluminatedScene::Update(elapsed_time);

	BYTE keystate[256];

	if (GetKeyboardState(keystate))
	{
		ProcessInput(keystate);
	}
}

void StageGame::Render()
{
	IlluminatedScene::Render();
}

void StageGame::OnAwake()
{
	IlluminatedScene::OnAwake();

	//
	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.2f, 0.15f, 1.0f);

	//
	numberLights = 4;
	myLights = new CLight[numberLights];
	::ZeroMemory(myLights, sizeof(CLight) * numberLights);

	myLights[0].m_bEnable = true;
	myLights[0].m_nType = POINT_LIGHT;
	myLights[0].m_fRange = 1000.0f;
	myLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
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
	myLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	myLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	myLights[2].m_bEnable = true;
	myLights[2].m_nType = DIRECTIONAL_LIGHT;
	myLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	myLights[2].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	myLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	myLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
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

	//
	myInstances.reserve(100);
	myInstances.clear();

	auto model_rock1 = myFramework.GetModel("Rock1").lock();
	model_rock1->SetScale(5.0f, 5.0f, 5.0f);

	auto model_rock2 = myFramework.GetModel("Rock2").lock();
	model_rock2->SetScale(5.0f, 5.0f, 5.0f);

	XMFLOAT3 stone_place;

	constexpr float width = 100.0f;
	constexpr float height = 2000.0f;
	constexpr int count = 100;
	constexpr auto place_gap = height / float(count);

	GameObject* stone = NULL;
	for (int i = 0; i < 2; ++i)
	{
		stone_place = XMFLOAT3{ i * width, 0.0f, 0.0f };

		for (int j = 0; j < 100; ++j)
		{
			stone = new GameObject();
			stone->Attach(model_rock1.get(), true);
			stone->SetPosition(stone_place);
			stone->Rotate(0.0f, 90.0f, 0.0f);
			myInstances.emplace_back(stone);

			stone_place.z += place_gap;
			//stone_place = Vector3::Add(stone_place, XMFLOAT3{0.0f, 0.0f, });

			stone = new GameObject();
			stone->Attach(model_rock2.get(), true);
			stone->SetPosition(stone_place);
			stone->Rotate(0.0f, -90.0f, 0.0f);
			myInstances.emplace_back(stone);

			stone_place.z += place_gap;
		}
	}

	auto model_policecar = myFramework.GetModel("PoliceCar").lock();

	CGunshipObject* pGunshipObject = NULL;
	pGunshipObject = new CGunshipObject();
	pGunshipObject->Attach(model_policecar.get(), true);
	pGunshipObject->Awake();
	pGunshipObject->SetPosition(135.0f, 0.0f, 220.0f);
	pGunshipObject->SetScale(8.5f, 8.5f, 8.5f);
	pGunshipObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pGunshipObject);

	auto model_tree = myFramework.GetModel("Tree").lock();

	CSuperCobraObject* pSuperCobraObject = NULL;
	pSuperCobraObject = new CSuperCobraObject();
	pSuperCobraObject->Attach(model_tree.get(), true);
	pSuperCobraObject->Awake();
	pSuperCobraObject->SetPosition(95.0f, 0.0f, 50.0f);
	pSuperCobraObject->SetScale(9.5f, 9.5f, 9.5f);
	pSuperCobraObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pSuperCobraObject);

	auto model_rallycar = myFramework.GetModel("RallyCar").lock();
	model_rallycar->SetPosition(-8.0f, -0.0f, 0.0f);
	model_rallycar->SetScale(10.0f, 10.0f, 10.0f);
	model_rallycar->Rotate(15.0f, 0.0f, 0.0f);
	//model_rallycar->Rotate(0.0f, -90.0f, 0.0f);

	auto player = new CAirplanePlayer(d3dDevice, d3dTaskList, GetRootSignature());
	player->Attach(model_rallycar.get(), true);
	player->SetPosition(XMFLOAT3(width * 0.5f, 0.0f, -3.0f));
	player->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	SetCamera(player->GetCamera());
	myPlayer = player;

	//myTestPlayer = player;
}

void StageGame::OnInialized()
{
	IlluminatedScene::OnInialized();
}

void StageGame::OnUpdate()
{
	IlluminatedScene::OnUpdate();
}

void StageGame::OnRender()
{
	IlluminatedScene::OnRender();
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
					//const auto look = myPlayer->GetLook();
					//const auto dir = Vector3::Normalize(look);
					//const auto velocity = Vector3::ScalarProduct(look, 20.0f);

					//myPlayer->Move(velocity, true);
					//myPlayer->Move(velocity, false);
				}
				break;

				case VK_DOWN:
				case 'S':
				{
					//const auto look = myPlayer->GetLook();
					//const auto dir = Vector3::Normalize(look);
					//const auto velocity = Vector3::ScalarProduct(look, -20.0f);

					//myPlayer->Move(velocity, true);
					//myPlayer->Move(velocity, false);
				}
				break;

				case 'A':
				{
					myPlayer->Rotate(0.0f, -1.0f, 0.0f);
				}
				break;

				case 'D':
				{
					myPlayer->Rotate(0.0f, 1.0f, 0.0f);
				}
				break;

				case 'Q': myPlayer->MoveUp(+1.0f); break;
				case 'R': myPlayer->MoveUp(-1.0f); break;

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
