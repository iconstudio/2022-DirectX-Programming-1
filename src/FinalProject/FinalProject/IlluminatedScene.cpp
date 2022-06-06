#include "pch.hpp"
#include "IlluminatedScene.hpp"
#include "GameCamera.hpp"

IlluminatedScene::IlluminatedScene(Framework& framework, const char* name, P3DDevice device, P3DGrpCommandList cmdlist)
	: Scene(framework, name)
	, dxDevice(device), dxTaskList(cmdlist)
	//, myTestVertices()
	, m_xmf4GlobalAmbient(XMFLOAT4(0.1f, 0.1f, 0.1f, 0.5f))
{}

void IlluminatedScene::Awake()
{
	// 256의 배수
	UINT ncbElementBytes = ((sizeof(StaticLights) + 255) & ~255);

	// 메모리 리소스 생성
	m_pd3dcbLights = CreateBufferResource(dxDevice, dxTaskList
		, NULL, ncbElementBytes
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);

	// CPU에서 GPU로 복사
	auto place = reinterpret_cast<void**>(&myStaticLights);
	auto valid = m_pd3dcbLights->Map(0, nullptr, place);
	if (FAILED(valid))
	{
		throw "광원의 메모리 할당 실패함!";
	}
}

void IlluminatedScene::Start()
{
	Scene::Start();

	GetCursorPos(&posCursor);

	//
	m_xmf4GlobalAmbient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//
	numberLights = 4;
	myLights = new CLight[numberLights];
	ZeroMemory(myLights, sizeof(CLight) * numberLights);

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
}

void IlluminatedScene::Reset()
{
	Scene::Reset();
}

void IlluminatedScene::Update(float delta_time)
{
	Scene::Update(delta_time);

	float cxDelta = 0.0f;
	POINT ptCursorPos;

	//if (GetCapture() == handleWindow)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - posCursor.x) / 3.0f;
		SetCursorPos(posCursor.x, posCursor.y);
	}

	if (myCamera && (cxDelta != 0.0f))
	{
		if (cxDelta)
		{
			myCamera->Rotate(0.0f, cxDelta, 0.0f);
		}
	}
}

void IlluminatedScene::PrepareRendering(P3DGrpCommandList cmdlist) const
{
	Scene::PrepareRendering(cmdlist);

	memcpy(myStaticLights->everyLights, myLights, sizeof(CLight) * numberLights);

	memcpy(&myStaticLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));

	memcpy(&myStaticLights->m_nLights, &numberLights, sizeof(int));
}

void IlluminatedScene::Render(P3DGrpCommandList cmdlist) const
{
	Scene::Render(cmdlist);

	auto gpu_lights_address = m_pd3dcbLights->GetGPUVirtualAddress();
	cmdlist->SetGraphicsRootConstantBufferView(2, gpu_lights_address);
}

void IlluminatedScene::Release()
{
	Scene::Release();

	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}
