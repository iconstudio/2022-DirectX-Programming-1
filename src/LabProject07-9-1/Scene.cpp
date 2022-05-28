#include "pch.hpp"
#include "stdafx.h"
#include "Scene.h"
#include "GameFramework.h"

CScene::CScene(GameFramework& framework, const char* name)
	: TaggedObject(framework, name)
	, d3dDevice(nullptr), d3dTaskList(nullptr)
{}

CScene::~CScene()
{
	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

void CScene::Awake(PtrDevice device, PtrGrpCommandList cmd_list)
{
	d3dDevice = device;
	d3dTaskList = cmd_list;

	Build();
}

void CScene::Build()
{
	if (!d3dDevice)
	{
		throw "프레임워크에서 D3D 장치를 먼저 설정해야함!";
	}

	globalAmbientLight = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

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
	auto pApacheModel = myFramework.LoadModel("Apache", "Model/Rock.bin");
	CApacheObject* pApacheObject = NULL;

	pApacheObject = new CApacheObject();
	pApacheObject->SetChild(pApacheModel.get(), true);
	pApacheObject->OnInitialize();
	pApacheObject->SetPosition(+130.0f, 0.0f, 160.0f);
	pApacheObject->SetScale(10.5f, 10.5f, 10.5f);
	pApacheObject->Rotate(0.0f, 90.0f, 0.0f);
	myInstances.emplace_back(pApacheObject);

	pApacheObject = new CApacheObject();
	pApacheObject->SetChild(pApacheModel.get(), true);
	pApacheObject->OnInitialize();
	pApacheObject->SetPosition(-75.0f, 0.0f, 80.0f);
	pApacheObject->SetScale(10.5f, 10.5f, 10.5f);
	pApacheObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pApacheObject);

	// Gunship
	auto pGunshipModel = myFramework.LoadModel("Police Car", "Model/PoliceCar.bin");
	CGunshipObject* pGunshipObject = NULL;

	pGunshipObject = new CGunshipObject();
	pGunshipObject->SetChild(pGunshipModel.get(), true);
	pGunshipObject->OnInitialize();
	pGunshipObject->SetPosition(135.0f, 40.0f, 220.0f);
	pGunshipObject->SetScale(8.5f, 8.5f, 8.5f);
	pGunshipObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pGunshipObject);

	// SuperCobra
	auto pSuperCobraModel = myFramework.LoadModel("Cactus", "Model/Cactus.bin");
	CSuperCobraObject* pSuperCobraObject = NULL;

	pSuperCobraObject = new CSuperCobraObject();
	pSuperCobraObject->SetChild(pSuperCobraModel.get(), true);
	pSuperCobraObject->OnInitialize();
	pSuperCobraObject->SetPosition(95.0f, 50.0f, 50.0f);
	pSuperCobraObject->SetScale(9.5f, 9.5f, 9.5f);
	pSuperCobraObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pSuperCobraObject);

	// Mi24
	auto pMi24Model = myFramework.LoadModel("Rally Car", "Model/RallyCar.bin");
	CMi24Object* pMi24Object = NULL;

	pMi24Object = new CMi24Object();
	pMi24Object->SetChild(pMi24Model.get(), true);
	pMi24Object->OnInitialize();
	pMi24Object->SetPosition(-95.0f, 50.0f, 50.0f);
	pMi24Object->SetScale(4.5f, 4.5f, 4.5f);
	pMi24Object->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pMi24Object);

	CreateShaderVariables();
}

void CScene::CreateShaderVariables()
{
	// 256의 배수
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255);

	// 메모리 리소스 생성
	m_pd3dcbLights = CreateBufferResource(d3dDevice, d3dTaskList
		, NULL, ncbElementBytes
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	// GPU에서 CPU로 복사
	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables()
{
	memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(CLight) * m_nLights);

	memcpy(&m_pcbMappedLights->globalAmbientLight, &globalAmbientLight, sizeof(XMFLOAT4));

	memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (auto& instance : myInstances)
	{
		instance->ReleaseUploadBuffers();
	}
}

void CScene::Start()
{}

void CScene::Reset()
{}

void CScene::Restart()
{}

void CScene::Update(float elapsed_time)
{
	lastDeltaTime = elapsed_time;

	for (auto& instance : myInstances)
	{
		instance->Animate(elapsed_time, nullptr);
	}

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void CScene::Render(GameCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(d3dTaskList);
	pCamera->UpdateShaderVariables(d3dTaskList);

	UpdateShaderVariables();

	auto lights_address = m_pd3dcbLights->GetGPUVirtualAddress();
	d3dTaskList->SetGraphicsRootConstantBufferView(2, lights_address);

	for (auto& instance : myInstances)
	{
		instance->UpdateTransform(NULL);
		instance->Render(d3dTaskList, pCamera);
	}
}

bool CScene::OnMouseEvent(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnKeyboardEvent(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	auto& something = myInstances.at(0);

	switch (nMessageID)
	{
		case WM_KEYDOWN:
		switch (wParam)
		{
			case 'W': something->MoveForward(+1.0f); break;
			case 'S': something->MoveForward(-1.0f); break;
			case 'A': something->MoveStrafe(-1.0f); break;
			case 'D': something->MoveStrafe(+1.0f); break;
			case 'Q': something->MoveUp(+1.0f); break;
			case 'R': something->MoveUp(-1.0f); break;
			default:
			break;
		}
		break;
		default:
		break;
	}
	return(false);
}

bool CScene::OnWindowsEvent(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	return false;
}
