#include "stdafx.h"
#include "StageGame.hpp"
#include "GameFramework.h"

StageGame::StageGame(GameFramework& framework, HWND hwnd)
	: IlluminatedScene(framework, hwnd, "Game")
{}

void StageGame::ProcessInput(UCHAR* pKeysBuffer)
{
	DWORD dwDirection = 0;
	if (pKeysBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeysBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
	if (pKeysBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeysBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
	if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
	if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;

	if (GetCapture() == handleWindow)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - posCursor.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - posCursor.y) / 3.0f;
		SetCursorPos(posCursor.x, posCursor.y);
	}

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeysBuffer[VK_RBUTTON] & 0xF0)
				myPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				myPlayer->Rotate(cyDelta, cxDelta, 0.0f);
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
}

void StageGame::Render()
{
	IlluminatedScene::Render();
}

void StageGame::RenderUI(HDC surface)
{
	IlluminatedScene::RenderUI(surface);
}

void StageGame::OnAwake()
{
	IlluminatedScene::OnAwake();

	//
	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

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

	// Apache
	auto model_rock1 = myFramework.GetModel("Rock").lock();

	CApacheObject* pApacheObject = NULL;
	pApacheObject = new CApacheObject();
	pApacheObject->Attach(model_rock1.get(), true);
	pApacheObject->Awake();
	pApacheObject->SetPosition(+130.0f, 0.0f, 160.0f);
	pApacheObject->SetScale(10.5f, 10.5f, 10.5f);
	pApacheObject->Rotate(0.0f, 90.0f, 0.0f);
	myInstances.emplace_back(pApacheObject);

	auto model_rock2 = myFramework.GetModel("Rock").lock();

	pApacheObject = new CApacheObject();
	pApacheObject->Attach(model_rock2.get(), true);
	pApacheObject->Awake();
	pApacheObject->SetPosition(-75.0f, 0.0f, 80.0f);
	pApacheObject->SetScale(10.5f, 10.5f, 10.5f);
	pApacheObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pApacheObject);

	auto model_policecar = myFramework.GetModel("PoliceCar").lock();

	CGunshipObject* pGunshipObject = NULL;
	pGunshipObject = new CGunshipObject();
	pGunshipObject->Attach(model_policecar.get(), true);
	pGunshipObject->Awake();
	pGunshipObject->SetPosition(135.0f, 40.0f, 220.0f);
	pGunshipObject->SetScale(8.5f, 8.5f, 8.5f);
	pGunshipObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pGunshipObject);

	auto model_tree = myFramework.GetModel("Tree").lock();

	CSuperCobraObject* pSuperCobraObject = NULL;
	pSuperCobraObject = new CSuperCobraObject();
	pSuperCobraObject->Attach(model_tree.get(), true);
	pSuperCobraObject->Awake();
	pSuperCobraObject->SetPosition(95.0f, 50.0f, 50.0f);
	pSuperCobraObject->SetScale(9.5f, 9.5f, 9.5f);
	pSuperCobraObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pSuperCobraObject);

	auto model_rallycar = myFramework.GetModel("RallyCar").lock();

	CMi24Object* pMi24Object = NULL;
	pMi24Object = new CMi24Object();
	pMi24Object->Attach(model_rallycar.get(), true);
	pMi24Object->Awake();
	pMi24Object->SetPosition(-95.0f, 50.0f, 50.0f);
	pMi24Object->SetScale(4.5f, 4.5f, 4.5f);
	pMi24Object->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pMi24Object);

	auto pAirplanePlayer = new CAirplanePlayer(d3dDevice, d3dTaskList, GetRootSignature());
	pAirplanePlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, -3.0f));
	pAirplanePlayer->m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	myPlayer = pAirplanePlayer;
	SetCamera(pAirplanePlayer->GetCamera());
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
		}
		break;

		case WM_RBUTTONDOWN:
		{
		}
		break;

		case WM_LBUTTONUP:
		{
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
					myCamera = myPlayer->ChangeCamera((DWORD)(key - VK_F1 + 1), 1.0f);
				}
				break;

				case 'W': something->MoveForward(+1.0f); break;
				case 'S': something->MoveForward(-1.0f); break;
				case 'A': something->MoveStrafe(-1.0f); break;
				case 'D': something->MoveStrafe(+1.0f); break;
				case 'Q': something->MoveUp(+1.0f); break;
				case 'R': something->MoveUp(-1.0f); break;
			}
			break;
		}

		default:
		{}
		break;
	}
}
