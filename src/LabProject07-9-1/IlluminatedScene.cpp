#include "pch.hpp"
#include "IlluminatedScene.hpp"

IlluminatedScene::IlluminatedScene(GameFramework& framework, HWND hwnd, const char* name)
	: Scene(framework, hwnd, name)
	, m_xmf4GlobalAmbient(XMFLOAT4(1, 1, 1, 1))
{}

IlluminatedScene::~IlluminatedScene()
{
	if (myLights)
	{
		delete[] myLights;
	}
}

void IlluminatedScene::Awake(P3DDevice device, P3DGrpCommandList cmd_list)
{
	Scene::Awake(device, cmd_list);
}

void IlluminatedScene::Start()
{
	Scene::Start();
}

void IlluminatedScene::Reset()
{
	Scene::Reset();
}

void IlluminatedScene::Update(float elapsed_time)
{
	Scene::Update(elapsed_time);

	if (myPlayer)
	{
		myPlayer->Animate(elapsed_time, NULL);
		myPlayer->Update(elapsed_time);

		if (myPlayer)
		{
			myLights[1].m_xmf3Position = myPlayer->GetPosition();
			myLights[1].m_xmf3Direction = myPlayer->GetLookVector();
		}
	}

	for (auto& instance : myInstances)
	{
		instance->Animate(elapsed_time, nullptr);
	}
}

void IlluminatedScene::Render()
{
	Scene::Render();

	if (!myCamera)
	{
		throw "장면에 카메라가 없음!";
	}

	auto gpu_lights_address = m_pd3dcbLights->GetGPUVirtualAddress();
	d3dTaskList->SetGraphicsRootConstantBufferView(2, gpu_lights_address);

	for (auto& instance : myInstances)
	{
		instance->UpdateTransform(nullptr);
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

void IlluminatedScene::RenderUI(HDC surface)
{
	Scene::RenderUI(surface);
}

void IlluminatedScene::OnAwake()
{
}

void IlluminatedScene::OnInialized()
{
	ReleaseUploadBuffers();
}

void IlluminatedScene::OnUpdate()
{
}

void IlluminatedScene::OnRender()
{
	d3dTaskList->SetGraphicsRootSignature(mySignature);

	myCamera->SetViewportsAndScissorRects(d3dTaskList);
	myCamera->UpdateUniforms(d3dTaskList);

	UpdateUniforms();
}

void IlluminatedScene::InitializeUniforms()
{
	Scene::InitializeUniforms();

	// 256의 배수
	UINT ncbElementBytes = ((sizeof(StaticLights) + 255) & ~255);

	// 메모리 리소스 생성
	m_pd3dcbLights = CreateBufferResource(d3dDevice, d3dTaskList
		, NULL, ncbElementBytes
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);

	// GPU에서 CPU로 복사
	auto place = reinterpret_cast<void**>(&myStaticLights);
	auto valid = m_pd3dcbLights->Map(0, nullptr, place);
	if (FAILED(valid))
	{
		throw "광원의 정보 얻기 실패!";
	}
}

void IlluminatedScene::UpdateUniforms()
{
	Scene::UpdateUniforms();

	memcpy(myStaticLights->everyLights, myLights, sizeof(CLight) * numberLights);

	memcpy(&myStaticLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));

	memcpy(&myStaticLights->m_nLights, &numberLights, sizeof(int));
}

void IlluminatedScene::ReleaseUniforms()
{
	Scene::ReleaseUniforms();

	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void IlluminatedScene::ReleaseUploadBuffers()
{
	Scene::ReleaseUploadBuffers();

	if (myPlayer)
	{
		myPlayer->ReleaseUploadBuffers();
	}
}

P3DSignature IlluminatedScene::CreateGraphicsRootSignature()
{
	P3DSignature signature = nullptr;

	D3D12_ROOT_PARAMETER shader_params[3]{};
	ZeroMemory(&shader_params, sizeof(shader_params));

	shader_params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	shader_params[0].Descriptor.ShaderRegister = 1; // Camera
	shader_params[0].Descriptor.RegisterSpace = 0;
	shader_params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	shader_params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	shader_params[1].Constants.Num32BitValues = 32;
	shader_params[1].Constants.ShaderRegister = 2; // GameObject

	shader_params[1].Constants.RegisterSpace = 0;
	shader_params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	shader_params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	shader_params[2].Descriptor.ShaderRegister = 4; // Lights
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
