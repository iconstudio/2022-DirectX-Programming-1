#include "pch.hpp"
#include "stdafx.h"
#include "Scene.h"

CScene::CScene(GameFramework& framework, const char* name)
	: TaggedObject(framework, name)
	, d3dDevice(nullptr), d3dTaskList(nullptr), d3dShaderParameters(nullptr)
{}

CScene::~CScene()
{
	if (d3dShaderParameters)
	{
		d3dShaderParameters->Release();
	}
	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

void CScene::Awake(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list)
{
	d3dDevice = device;
	d3dTaskList = cmd_list;

	Build();
}

void CScene::Build()
{
	d3dShaderParameters = CreateGraphicsRootSignature();

	CMaterial::PrepareShaders(d3dDevice, d3dTaskList, d3dShaderParameters);

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
	GameObject* pApacheModel = GameObject::LoadGeometryFromFile(d3dDevice, d3dTaskList, d3dShaderParameters, "Model/Rock.bin");
	CApacheObject* pApacheObject = NULL;

	pApacheObject = new CApacheObject();
	pApacheObject->SetChild(pApacheModel, true);
	pApacheObject->OnInitialize();
	pApacheObject->SetPosition(+130.0f, 0.0f, 160.0f);
	pApacheObject->SetScale(10.5f, 10.5f, 10.5f);
	pApacheObject->Rotate(0.0f, 90.0f, 0.0f);
	myInstances.emplace_back(pApacheObject);
	
	pApacheObject = new CApacheObject();
	pApacheObject->SetChild(pApacheModel, true);
	pApacheObject->OnInitialize();
	pApacheObject->SetPosition(-75.0f, 0.0f, 80.0f);
	pApacheObject->SetScale(10.5f, 10.5f, 10.5f);
	pApacheObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pApacheObject);

	// Gunship
	GameObject* pGunshipModel = GameObject::LoadGeometryFromFile(d3dDevice, d3dTaskList, d3dShaderParameters, "Model/PoliceCar.bin");
	CGunshipObject* pGunshipObject = NULL;

	pGunshipObject = new CGunshipObject();
	pGunshipObject->SetChild(pGunshipModel, true);
	pGunshipObject->OnInitialize();
	pGunshipObject->SetPosition(135.0f, 40.0f, 220.0f);
	pGunshipObject->SetScale(8.5f, 8.5f, 8.5f);
	pGunshipObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pGunshipObject);

	// SuperCobra
	GameObject* pSuperCobraModel = GameObject::LoadGeometryFromFile(d3dDevice, d3dTaskList, d3dShaderParameters, "Model/Cactus.bin");
	CSuperCobraObject* pSuperCobraObject = NULL;

	pSuperCobraObject = new CSuperCobraObject();
	pSuperCobraObject->SetChild(pSuperCobraModel, true);
	pSuperCobraObject->OnInitialize();
	pSuperCobraObject->SetPosition(95.0f, 50.0f, 50.0f);
	pSuperCobraObject->SetScale(9.5f, 9.5f, 9.5f);
	pSuperCobraObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pSuperCobraObject);

	// Mi24
	GameObject* pMi24Model = GameObject::LoadGeometryFromFile(d3dDevice, d3dTaskList, d3dShaderParameters, "Model/RallyCar.bin");
	CMi24Object* pMi24Object = NULL;

	pMi24Object = new CMi24Object();
	pMi24Object->SetChild(pMi24Model, true);
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

	memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));

	memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return d3dShaderParameters;
}

ID3D12RootSignature const* CScene::GetGraphicsRootSignature() const
{
	return d3dShaderParameters;
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

ID3D12RootSignature* CScene::CreateGraphicsRootSignature()
{
	D3D12_ROOT_PARAMETER pd3dRootParameters[3]{};

	// register(b1)
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// register(b2)
	pd3dRootParameters[1].Constants.ShaderRegister = 2; // GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].Constants.Num32BitValues = 32; // 최대 32개
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// register(b4)
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC rootsignature_desc{};
	::ZeroMemory(&rootsignature_desc, sizeof(D3D12_ROOT_SIGNATURE_DESC));

	rootsignature_desc.NumParameters = std::size(pd3dRootParameters);
	rootsignature_desc.pParameters = pd3dRootParameters;
	rootsignature_desc.NumStaticSamplers = 0;
	rootsignature_desc.pStaticSamplers = nullptr;
	rootsignature_desc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = nullptr;
	ID3DBlob* pd3dErrorBlob = nullptr;
	D3D12SerializeRootSignature(&rootsignature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	ID3D12RootSignature* root_signature = nullptr;

	UINT gpu_mask = 0;
	void* blob = pd3dSignatureBlob->GetBufferPointer();
	size_t blob_size = pd3dSignatureBlob->GetBufferSize();

	auto uuid = __uuidof(ID3D12RootSignature);
	void** place = reinterpret_cast<void**>(&root_signature);
	d3dDevice->CreateRootSignature(gpu_mask, blob, blob_size, uuid, place);

	if (pd3dSignatureBlob)
	{
		pd3dSignatureBlob->Release();
	}

	if (pd3dErrorBlob)
	{
		pd3dErrorBlob->Release();
	}

	return  root_signature;
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
	d3dTaskList->SetGraphicsRootSignature(d3dShaderParameters);

	pCamera->SetViewportsAndScissorRects(d3dTaskList);
	pCamera->UpdateShaderVariables(d3dTaskList);

	UpdateShaderVariables();

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	d3dTaskList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress);
	
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
