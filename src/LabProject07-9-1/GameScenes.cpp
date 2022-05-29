#include "stdafx.h"
#include "GameScenes.hpp"

StageIntro::StageIntro(GameFramework& framework, HWND hwnd)
	: Scene(framework, hwnd, "Intro")
{}

void StageIntro::Awake(P3DDevice device, P3DGrpCommandList cmd_list)
{
	Scene::Awake(device, cmd_list);
}

void StageIntro::Start()
{
	Scene::Start();
}

void StageIntro::Reset()
{
	Scene::Reset();
}

void StageIntro::Update(float elapsed_time)
{
	Scene::Update(elapsed_time);
}

void StageIntro::Render()
{
	Scene::Render();
}

void StageIntro::RenderUI(HDC surface)
{
	Scene::RenderUI(surface);
}

void StageIntro::OnAwake()
{}

void StageIntro::OnInialized()
{}

void StageIntro::OnUpdate()
{}

void StageIntro::OnRender()
{}

void StageIntro::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageIntro::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
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

void StageIntro::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{}

StageMain::StageMain(GameFramework& framework, HWND hwnd)
	: Scene(framework, hwnd, "Main")
{}

void StageMain::Awake(P3DDevice device, P3DGrpCommandList cmd_list)
{
	Scene::Awake(device, cmd_list);
}

void StageMain::Start()
{
	Scene::Start();
}

void StageMain::Reset()
{
	Scene::Reset();
}

void StageMain::Update(float elapsed_time)
{
	Scene::Update(elapsed_time);
}

void StageMain::Render()
{
	Scene::Render();
}

void StageMain::RenderUI(HDC surface)
{
	Scene::RenderUI(surface);
}

void StageMain::OnAwake()
{}

void StageMain::OnInialized()
{}

void StageMain::OnUpdate()
{}

void StageMain::OnRender()
{}

void StageMain::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageMain::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
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

void StageMain::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
}

StageGame::StageGame(GameFramework& framework, HWND hwnd)
	: IlluminatedScene(framework, hwnd, "Game")
{}

bool StageGame::ProcessInput(UCHAR* pKeysBuffer)
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

	return false;
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
	m_nLights = 4;
	m_pLights = new CLight[m_nLights];
	::ZeroMemory(m_pLights, sizeof(CLight) * m_nLights);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	//
	myInstances.reserve(100);
	myInstances.clear();

	// Apache
	GameObject* pApacheModel = GameObject::LoadGeometryFromFile(d3dDevice, d3dTaskList, mySignature, "Model/Rock.bin");
	CApacheObject* pApacheObject = NULL;

	pApacheObject = new CApacheObject();
	pApacheObject->Attach(pApacheModel, true);
	pApacheObject->Awake();
	pApacheObject->SetPosition(+130.0f, 0.0f, 160.0f);
	pApacheObject->SetScale(10.5f, 10.5f, 10.5f);
	pApacheObject->Rotate(0.0f, 90.0f, 0.0f);
	myInstances.emplace_back(pApacheObject);

	pApacheObject = new CApacheObject();
	pApacheObject->Attach(pApacheModel, true);
	pApacheObject->Awake();
	pApacheObject->SetPosition(-75.0f, 0.0f, 80.0f);
	pApacheObject->SetScale(10.5f, 10.5f, 10.5f);
	pApacheObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pApacheObject);

	// Gunship
	GameObject* pGunshipModel = GameObject::LoadGeometryFromFile(d3dDevice, d3dTaskList, mySignature, "Model/PoliceCar.bin");
	CGunshipObject* pGunshipObject = NULL;

	pGunshipObject = new CGunshipObject();
	pGunshipObject->Attach(pGunshipModel, true);
	pGunshipObject->Awake();
	pGunshipObject->SetPosition(135.0f, 40.0f, 220.0f);
	pGunshipObject->SetScale(8.5f, 8.5f, 8.5f);
	pGunshipObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pGunshipObject);

	// SuperCobra
	GameObject* pSuperCobraModel = GameObject::LoadGeometryFromFile(d3dDevice, d3dTaskList, mySignature, "Model/Cactus.bin");
	CSuperCobraObject* pSuperCobraObject = NULL;

	pSuperCobraObject = new CSuperCobraObject();
	pSuperCobraObject->Attach(pSuperCobraModel, true);
	pSuperCobraObject->Awake();
	pSuperCobraObject->SetPosition(95.0f, 50.0f, 50.0f);
	pSuperCobraObject->SetScale(9.5f, 9.5f, 9.5f);
	pSuperCobraObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pSuperCobraObject);

	// Mi24
	GameObject* pMi24Model = GameObject::LoadGeometryFromFile(d3dDevice, d3dTaskList, mySignature, "Model/RallyCar.bin");
	CMi24Object* pMi24Object = NULL;

	pMi24Object = new CMi24Object();
	pMi24Object->Attach(pMi24Model, true);
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
}

StageGameEnd::StageGameEnd(GameFramework& framework, HWND hwnd)
	: Scene(framework, hwnd, "Complete")
{}

void StageGameEnd::Awake(P3DDevice device, P3DGrpCommandList cmd_list)
{
	Scene::Awake(device, cmd_list);
}

void StageGameEnd::Start()
{
	Scene::Start();
}

void StageGameEnd::Reset()
{
	Scene::Reset();
}

void StageGameEnd::Update(float elapsed_time)
{
	Scene::Update(elapsed_time);
}

void StageGameEnd::Render()
{
	Scene::Render();
}

void StageGameEnd::RenderUI(HDC surface)
{
	Scene::RenderUI(surface);
}

void StageGameEnd::OnAwake()
{}

void StageGameEnd::OnInialized()
{}

void StageGameEnd::OnUpdate()
{}

void StageGameEnd::OnRender()
{}

void StageGameEnd::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageGameEnd::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
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

void StageGameEnd::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
}

StageCredit::StageCredit(GameFramework& framework, HWND hwnd)
	: Scene(framework, hwnd, "Credit")
{}

void StageCredit::Awake(P3DDevice device, P3DGrpCommandList cmd_list)
{
	Scene::Awake(device, cmd_list);
}

void StageCredit::Start()
{
	Scene::Start();
}

void StageCredit::Reset()
{
	Scene::Reset();
}

void StageCredit::Update(float elapsed_time)
{
	Scene::Update(elapsed_time);
}

void StageCredit::Render()
{
	Scene::Render();
}

void StageCredit::RenderUI(HDC surface)
{
	Scene::RenderUI(surface);
}

void StageCredit::OnAwake()
{}

void StageCredit::OnInialized()
{}

void StageCredit::OnUpdate()
{}

void StageCredit::OnRender()
{}

void StageCredit::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageCredit::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
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

void StageCredit::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
}
