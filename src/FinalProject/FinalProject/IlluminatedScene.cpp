#include "pch.hpp"
#include "IlluminatedScene.hpp"

IlluminatedScene::IlluminatedScene(Framework& framework, const char* name, P3DDevice device, P3DGrpCommandList cmdlist)
	: Scene(framework, name)
	, dxDevice(device), dxTaskList(cmdlist)
	, m_xmf4GlobalAmbient(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
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
}

void IlluminatedScene::Reset()
{
	Scene::Reset();
}

void IlluminatedScene::Update(float delta_time)
{
	Scene::Update(delta_time);
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
