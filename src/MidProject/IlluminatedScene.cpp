#include "pch.hpp"
#include "IlluminatedScene.hpp"

IlluminatedScene::IlluminatedScene(GameFramework& framework, const char* name)
	: Scene(framework, name)
	, m_xmf4GlobalAmbient(XMFLOAT4(1, 1, 1, 1))
{}

IlluminatedScene::~IlluminatedScene()
{
	if (myLights)
	{
		delete[] myLights;
	}

	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void IlluminatedScene::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	Scene::Awake(device, cmdlist);

	// 256�� ���
	UINT ncbElementBytes = ((sizeof(StaticLights) + 255) & ~255);

	// ��� ���� ������ �ڿ� ����
	m_pd3dcbLights = CreateBufferResource(device, cmdlist
		, NULL, ncbElementBytes
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);

	// GPU�� �޸𸮿� CPU�� �޸� ����
	auto place = reinterpret_cast<void**>(&myStaticLights);
	auto valid = m_pd3dcbLights->Map(0, nullptr, place);
	if (FAILED(valid))
	{
		throw "������ ���� ��� ����!";
	}
}

void IlluminatedScene::Start()
{
	Scene::Start();

	if (myPlayer)
	{
		myPlayer->Awake(d3dDevice, d3dTaskList);
	}
}

void IlluminatedScene::Reset()
{
	Scene::Reset();
}

void IlluminatedScene::Update(float delta_time)
{
	Scene::Update(delta_time);

	if (myPlayer)
	{
		myPlayer->Animate(delta_time);

		if (myLights)
		{
			myLights[1].m_xmf3Position = myPlayer->GetPosition();
			myLights[1].m_xmf3Direction = myPlayer->GetLookVector();
		}

		for (auto& instance : myInstances)
		{
			if (myPlayer->CheckCollisionWith(instance.get()))
			{
				myPlayer->CollideWith(instance.get());
			}
		}
	}
}

void IlluminatedScene::PrepareRendering()
{
	auto& pipeline = Pipeline::illuminatedShader;
	if (!pipeline)
	{
		throw "���� ���̴��� �������� ����!";
	}

	d3dTaskList->SetGraphicsRootSignature(pipeline->GetRootSignature());

	myCamera->SetViewportsAndScissorRects(d3dTaskList);
	myCamera->UpdateUniforms(d3dTaskList);

	memcpy(myStaticLights->everyLights, myLights, sizeof(CLight) * numberLights);

	memcpy(&myStaticLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));

	memcpy(&myStaticLights->m_nLights, &numberLights, sizeof(int));
}

void IlluminatedScene::Render() const
{
	if (!myCamera)
	{
		throw "��鿡 ī�޶� ����!";
	}

	auto gpu_lights_address = m_pd3dcbLights->GetGPUVirtualAddress();
	d3dTaskList->SetGraphicsRootConstantBufferView(2, gpu_lights_address);

	for (auto& instance : myInstances)
	{
		instance->EnumerateTransforms();
		instance->Render(d3dTaskList, myCamera);
	}

#ifdef _WITH_PLAYER_TOP
	d3dTaskList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle
		, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL
		, 1.0f, 0, 0
		, NULL);
#endif

	if (myPlayer)
	{
		myPlayer->Render(d3dTaskList, myCamera);
	}
}

void IlluminatedScene::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
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

void IlluminatedScene::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{}

void IlluminatedScene::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}
