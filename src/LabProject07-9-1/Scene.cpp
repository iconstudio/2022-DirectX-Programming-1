#include "stdafx.h"
#include "Scene.h"

CScene::CScene(GameFramework& framework, const char* name)
	: myFramework(framework), myName(name)
	, myWindow(NULL), posCursor{ 0, 0 }
	, d3dDevice(nullptr), d3dTaskList(nullptr), d3dShaderParameters(nullptr)
	, m_xmf4GlobalAmbient(XMFLOAT4())
{}

CScene::~CScene()
{
	if (d3dShaderParameters)
	{
		d3dShaderParameters->Release();
	}
	ReleaseUniforms();

	if (m_pLights) delete[] m_pLights;
}

void CScene::Awake(HWND hwnd, P3DDevice device, P3DGrpCommandList cmd_list)
{
	myWindow = hwnd;
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
	GameObject* pGunshipModel = GameObject::LoadGeometryFromFile(d3dDevice, d3dTaskList, d3dShaderParameters, "Model/PoliceCar.bin");
	CGunshipObject* pGunshipObject = NULL;

	pGunshipObject = new CGunshipObject();
	pGunshipObject->Attach(pGunshipModel, true);
	pGunshipObject->Awake();
	pGunshipObject->SetPosition(135.0f, 40.0f, 220.0f);
	pGunshipObject->SetScale(8.5f, 8.5f, 8.5f);
	pGunshipObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pGunshipObject);

	// SuperCobra
	GameObject* pSuperCobraModel = GameObject::LoadGeometryFromFile(d3dDevice, d3dTaskList, d3dShaderParameters, "Model/Cactus.bin");
	CSuperCobraObject* pSuperCobraObject = NULL;

	pSuperCobraObject = new CSuperCobraObject();
	pSuperCobraObject->Attach(pSuperCobraModel, true);
	pSuperCobraObject->Awake();
	pSuperCobraObject->SetPosition(95.0f, 50.0f, 50.0f);
	pSuperCobraObject->SetScale(9.5f, 9.5f, 9.5f);
	pSuperCobraObject->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pSuperCobraObject);

	// Mi24
	GameObject* pMi24Model = GameObject::LoadGeometryFromFile(d3dDevice, d3dTaskList, d3dShaderParameters, "Model/RallyCar.bin");
	CMi24Object* pMi24Object = NULL;

	pMi24Object = new CMi24Object();
	pMi24Object->Attach(pMi24Model, true);
	pMi24Object->Awake();
	pMi24Object->SetPosition(-95.0f, 50.0f, 50.0f);
	pMi24Object->SetScale(4.5f, 4.5f, 4.5f);
	pMi24Object->Rotate(0.0f, -90.0f, 0.0f);
	myInstances.emplace_back(pMi24Object);

	InitializeUniforms();
}

void CScene::InitializeUniforms()
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

void CScene::UpdateUniforms()
{
	memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(CLight) * m_nLights);

	memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));

	memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

const std::string& CScene::GetName() const noexcept
{
	return myName;
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return d3dShaderParameters;
}

ID3D12RootSignature const* CScene::GetGraphicsRootSignature() const
{
	return d3dShaderParameters;
}

void CScene::ReleaseUniforms()
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
	ID3D12RootSignature* signature = NULL;

	D3D12_ROOT_PARAMETER shader_params[3]{};
	ZeroMemory(&shader_params, sizeof(shader_params));

	// 메모리 배열
	shader_params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	shader_params[0].Descriptor.ShaderRegister = 1; //Camera
	shader_params[0].Descriptor.RegisterSpace = 0;
	shader_params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// 상수 (x, y, z, w) * 64개
	shader_params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	shader_params[1].Constants.Num32BitValues = 32;
	shader_params[1].Constants.ShaderRegister = 2; //GameObject

	shader_params[1].Constants.RegisterSpace = 0;
	shader_params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// 메모리 배열
	shader_params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	shader_params[2].Descriptor.ShaderRegister = 4; //Lights
	shader_params[2].Descriptor.RegisterSpace = 0;
	shader_params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// 정점 쉐이더, 픽셀 쉐이더, 입력 조립기, 출력 병합기 
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC signature_desc;
	ZeroMemory(&signature_desc, sizeof(signature_desc));

	signature_desc.NumParameters = _countof(shader_params);
	signature_desc.pParameters = shader_params;
	signature_desc.NumStaticSamplers = 0; // 텍스처
	signature_desc.pStaticSamplers = NULL;
	signature_desc.Flags = flags;

	ID3DBlob* signature_blob = nullptr;
	ID3DBlob* error_blob = nullptr;

	auto valid = D3D12SerializeRootSignature(&signature_desc
		, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob
		, &error_blob);
	if (FAILED(valid))
	{
		throw "루트 서명의 메모리를 할당하지 못함!";
	}

	UINT gpu_mask = 0;
	auto uuid = __uuidof(ID3D12RootSignature);
	auto place = reinterpret_cast<void**>(&signature);
	valid = d3dDevice->CreateRootSignature(gpu_mask
		, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize()
		, uuid, place);
	if (FAILED(valid))
	{
		throw "루트 서명을 셍성하지 못함!";
	}

	if (signature_blob)
	{
		signature_blob->Release();
	}

	if (error_blob)
	{
		error_blob->Release();
	}

	return signature;
}

void CScene::Start()
{}

void CScene::Reset()
{}

void CScene::Update(float elapsed_time)
{
	lastDeltaTime = elapsed_time;

	for (auto& instance : myInstances)
	{
		instance->Animate(elapsed_time, nullptr);
	}

	if (m_pLights && m_pPlayer)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
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

	if (GetCapture() == myWindow)
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
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}

		if (dwDirection)
		{
			m_pPlayer->Move(dwDirection, 1.5f, true);
		}
	}

	return false;
}

void CScene::Render(GameCamera* pCamera)
{
	d3dTaskList->SetGraphicsRootSignature(d3dShaderParameters);

	pCamera->SetViewportsAndScissorRects(d3dTaskList);
	pCamera->UpdateUniforms(d3dTaskList);

	UpdateUniforms();

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	d3dTaskList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress);
	// Lights

	for (auto& instance : myInstances)
	{
		instance->UpdateTransform(NULL);
		instance->Render(d3dTaskList, pCamera);
	}
}

void CScene::OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		{
			SetCapture(hwnd);
			GetCursorPos(&posCursor);
		}
		break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		{
			ReleaseCapture();
		}
		break;

		case WM_MOUSEMOVE:
		{}
		break;

		default:
		{}
		break;
	}
}

void CScene::OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
	auto& something = myInstances.at(0);

	switch (msg)
	{
		case WM_KEYDOWN:
		{
			switch (key)
			{
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
