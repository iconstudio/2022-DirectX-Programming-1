#include "pch.hpp"
#include "GameFramework.h"
#include "StageMain.hpp"
#include "StageGame.hpp"
#include "StageGameEnd.hpp"
#include "StageCredit.hpp"
#include "Shader.h"
#include "Model.hpp"

GameFramework::GameFramework(unsigned int width, unsigned int height)
	: frameWidth(width), frameHeight(height)
	, frameBasisColour{ 0.0f, 0.125f, 0.3f, 1.0f }
	, isAntiAliasingEnabled(false), levelAntiAliasing(0)
	, indexFrameBuffer(0)
	, myFactory(nullptr), myDevice(nullptr)
	, myRenderFence(nullptr), myFences(), eventFence(NULL)
	, myCommandList(nullptr), myCommandQueue(nullptr), myCommandAlloc(nullptr)
	, myRootSignature(nullptr)
	, mySwapChain(nullptr), resSwapChainBackBuffers(), myBarriers()
	, heapRtvDesc(nullptr), szRtvDescIncrements(0)
	, myDepthStencilBuffer(nullptr)
	, heapDsvDesc(nullptr), szDsvDescIncrements(0)
#ifdef _DEBUG
	, myDebugController(nullptr)
#endif //  _DEBUG
	, myModels()
	, myScenes(), myStages(), myStageIterator(), currentScene()
{
	ZeroMemory(resSwapChainBackBuffers, sizeof(resSwapChainBackBuffers));
	ZeroMemory(myBarriers, sizeof(myBarriers));

	myModels.reserve(10);
	myScenes.reserve(10);
	myStages.reserve(10);

	auto& barrier_render = myBarriers[0];
	barrier_render.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier_render.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier_render.Transition.pResource = nullptr;
	barrier_render.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_render.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier_render.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	auto& barrier_swap = myBarriers[1];
	barrier_swap.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier_swap.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier_swap.Transition.pResource = nullptr;
	barrier_swap.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier_swap.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_swap.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
}

GameFramework::~GameFramework()
{
	WaitForGpuComplete();

	CloseHandle(eventFence);

	if (myDepthStencilBuffer) myDepthStencilBuffer->Release();
	if (heapDsvDesc) heapDsvDesc->Release();

	for (int i = 0; i < numberFrameBuffers; i++)
	{
		if (resSwapChainBackBuffers[i])
		{
			resSwapChainBackBuffers[i]->Release();
		}
	}
	if (heapRtvDesc) heapRtvDesc->Release();

	if (mySwapChain)
	{
		mySwapChain->SetFullscreenState(FALSE, NULL);
		mySwapChain->Release();
	}

	if (myRootSignature)
	{
		myRootSignature->Release();
	}

	if (myCommandAlloc) myCommandAlloc->Release();
	if (myCommandQueue) myCommandQueue->Release();
	if (myCommandList) myCommandList->Release();

	if (myRenderFence) myRenderFence->Release();

	if (myDevice) myDevice->Release();
	if (myFactory) myFactory->Release();

#if defined(_DEBUG)
	if (myDebugController) myDebugController->Release();

	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);

	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void GameFramework::Awake(HINSTANCE hInstance, HWND hMainWnd)
{
	myAppInstance = hInstance;
	myWindow = hMainWnd;

	CreateDirect3DDevice();
	CreateCommanders();
	CreateSwapChain();
	CreateRenderTargetViews();
	CreateDepthStencilView();
}

void GameFramework::CreateDirect3DDevice()
{
	HRESULT valid = 0;
	IID uiid{};
	void** place = nullptr;
	ZeroMemory(&uiid, sizeof(uiid));

	UINT factory_flags = 0;
#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = nullptr;

	uiid = __uuidof(ID3D12Debug);
	place = reinterpret_cast<void**>(&pd3dDebugController);
	valid = D3D12GetDebugInterface(uiid, place);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}

	factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	uiid = __uuidof(IDXGIFactory4);
	place = reinterpret_cast<void**>(&myFactory);
	valid = CreateDXGIFactory2(factory_flags, uiid, place);
	if (FAILED(valid))
	{
		throw "IDXGIFactory4 생성 오류!";
	}

	IDXGIAdapter1* adapter = nullptr;
	DXGI_ADAPTER_DESC1 adapter_desc;
	D3D_FEATURE_LEVEL d3d_level = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != myFactory->EnumAdapters1(i, &adapter); i++)
	{
		valid = adapter->GetDesc1(&adapter_desc);
		if (FAILED(valid))
		{
			throw "IDXGIAdapter1 어댑터의 서술자 얻기 실패!";
		}

		if (adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		uiid = __uuidof(ID3D12Device);
		place = reinterpret_cast<void**>(&myDevice);
		valid = D3D12CreateDevice(adapter, d3d_level, uiid, place);
		if (SUCCEEDED(valid))
		{
			break;
		}
	}

	if (!adapter)
	{
		uiid = __uuidof(IDXGIFactory4);
		place = reinterpret_cast<void**>(&adapter);
		valid = myFactory->EnumWarpAdapter(uiid, place);
		if (FAILED(valid))
		{
			throw "팩토리에서 대체 어댑터 열거 실패!";
		}

		uiid = __uuidof(ID3D12Device);
		place = reinterpret_cast<void**>(&myDevice);
		valid = D3D12CreateDevice(adapter, d3d_level, uiid, place);
		if (FAILED(valid))
		{
			throw "디바이스 열거 & 생성 실패!";
		}
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS aa_desc{};
	ZeroMemory(&aa_desc, sizeof(aa_desc));

	aa_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	aa_desc.SampleCount = 4;
	aa_desc.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	aa_desc.NumQualityLevels = 0;
	const auto aa_feature = D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS;
	const auto aa_sz = sizeof(aa_desc);

	valid = myDevice->CheckFeatureSupport(aa_feature, &aa_desc, aa_sz);
	if (FAILED(valid))
	{
		throw "안티 앨리어싱 지원 확인 실패!";
	}
	levelAntiAliasing = aa_desc.NumQualityLevels;
	isAntiAliasingEnabled = 1 < levelAntiAliasing;

	const auto fence_flag = D3D12_FENCE_FLAG_NONE;
	uiid = __uuidof(ID3D12Fence);
	place = reinterpret_cast<void**>(&myRenderFence);
	valid = myDevice->CreateFence(0, fence_flag, uiid, place);
	if (FAILED(valid))
	{
		throw "GPU 메모리 장벽 생성 실패!";
	}

	// CreateEvent(attr, reset, init, name);
	eventFence = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == eventFence)
	{
		throw "장벽 이벤트 객체 생성 실패!";
	}

	if (adapter)
	{
		adapter->Release();
	}
}

void GameFramework::CreateCommanders()
{
	HRESULT valid = 0;
	IID uiid{};
	void** place = nullptr;
	ZeroMemory(&uiid, sizeof(uiid));

	D3D12_COMMAND_QUEUE_DESC queue_desc{};
	ZeroMemory(&queue_desc, sizeof(queue_desc));

	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	uiid = __uuidof(ID3D12CommandQueue);
	place = reinterpret_cast<void**>(&myCommandQueue);
	valid = myDevice->CreateCommandQueue(&queue_desc, uiid, place);
	if (FAILED(valid))
	{
		throw "명령 큐 생성 실패!";
	}

	const auto list_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	uiid = __uuidof(ID3D12CommandAllocator);
	place = reinterpret_cast<void**>(&myCommandAlloc);
	valid = myDevice->CreateCommandAllocator(list_type, uiid, place);
	if (FAILED(valid))
	{
		throw "명령 할당자 생성 실패!";
	}

	uiid = __uuidof(ID3D12GraphicsCommandList);
	place = reinterpret_cast<void**>(&myCommandList);
	valid = myDevice->CreateCommandList(0, list_type, myCommandAlloc, NULL, uiid, place);
	if (FAILED(valid))
	{
		throw "명령 리스트 생성 실패!";
	}

	valid = myCommandList->Close();
	if (FAILED(valid))
	{
		throw "명령 리스트를 닫는 중에 문제 발생!";
	}
}

void GameFramework::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sw_desc{};
	ZeroMemory(&sw_desc, sizeof(sw_desc));

	sw_desc.BufferCount = numberFrameBuffers;
	sw_desc.BufferDesc.Width = frameWidth;
	sw_desc.BufferDesc.Height = frameHeight;
	sw_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sw_desc.BufferDesc.RefreshRate.Numerator = 60;
	sw_desc.BufferDesc.RefreshRate.Denominator = 1;
	sw_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sw_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sw_desc.OutputWindow = myWindow;
	if (isAntiAliasingEnabled)
	{
		sw_desc.SampleDesc.Count = 4;
		sw_desc.SampleDesc.Quality = levelAntiAliasing - 1;
	}
	else
	{
		sw_desc.SampleDesc.Count = 1;
		sw_desc.SampleDesc.Quality = 0;
	}
	sw_desc.Windowed = TRUE;
	sw_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto place = reinterpret_cast<IDXGISwapChain**>(&mySwapChain);
	HRESULT valid = myFactory->CreateSwapChain(myCommandQueue, &sw_desc, place);
	if (FAILED(valid))
	{
		throw "스왑 체인 생성 실패!";
	}

	indexFrameBuffer = mySwapChain->GetCurrentBackBufferIndex();

	valid = myFactory->MakeWindowAssociation(myWindow, DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(valid))
	{
		throw "스왑 체인의 윈도우 연결 실패!";
	}
}

void GameFramework::CreateRenderTargetViews()
{
	HRESULT valid = 0;
	IID uiid{};
	void** place = nullptr;
	ZeroMemory(&uiid, sizeof(uiid));

	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc{};
	ZeroMemory(&rtv_heap_desc, sizeof(rtv_heap_desc));

	const auto buffer_type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtv_heap_desc.NumDescriptors = numberFrameBuffers;
	rtv_heap_desc.Type = buffer_type;
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtv_heap_desc.NodeMask = 0; // GPU 마스크

	uiid = __uuidof(ID3D12DescriptorHeap);
	place = reinterpret_cast<void**>(&heapRtvDesc);
	valid = myDevice->CreateDescriptorHeap(&rtv_heap_desc, uiid, place);
	if (FAILED(valid))
	{
		throw "렌더 타겟 뷰의 서술자 힙을 생성하는데 실패!";
	}
	szRtvDescIncrements = myDevice->GetDescriptorHandleIncrementSize(buffer_type);

	// CPU 메모리에 렌더링 버퍼 할당
	auto cpu_ptr_handle = heapRtvDesc->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < numberFrameBuffers; i++)
	{
		uiid = __uuidof(ID3D12Resource);
		place = reinterpret_cast<void**>(&resSwapChainBackBuffers[i]);
		valid = mySwapChain->GetBuffer(i, uiid, place);
		if (FAILED(valid))
		{
			throw "후면 렌더링 버퍼의 메모리 할당 실패!";
		}

		myDevice->CreateRenderTargetView(resSwapChainBackBuffers[i], NULL, cpu_ptr_handle);

		cpu_ptr_handle.ptr += szRtvDescIncrements;
	}
}

void GameFramework::CreateDepthStencilView()
{
	HRESULT valid = 0;
	IID uiid{};
	void** place = nullptr;
	ZeroMemory(&uiid, sizeof(uiid));

	D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc{};
	ZeroMemory(&dsv_heap_desc, sizeof(dsv_heap_desc));

	const auto buffer_type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsv_heap_desc.NumDescriptors = 1;
	dsv_heap_desc.Type = buffer_type;
	dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsv_heap_desc.NodeMask = 0;

	uiid = __uuidof(ID3D12DescriptorHeap);
	place = reinterpret_cast<void**>(&heapDsvDesc);
	valid = myDevice->CreateDescriptorHeap(&dsv_heap_desc, uiid, place);
	if (FAILED(valid))
	{
		throw "깊이 스텐실 뷰의 서술자 힙을 생성하는데 실패!";
	}

	szDsvDescIncrements = myDevice->GetDescriptorHandleIncrementSize(buffer_type);


	D3D12_RESOURCE_DESC stencil_desc{};
	ZeroMemory(&stencil_desc, sizeof(stencil_desc));

	stencil_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	stencil_desc.Alignment = 0;
	stencil_desc.Width = UINT64(frameWidth);
	stencil_desc.Height = UINT64(frameHeight);
	stencil_desc.DepthOrArraySize = 1;
	stencil_desc.MipLevels = 1;
	stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (isAntiAliasingEnabled)
	{
		stencil_desc.SampleDesc.Count = 4;
		stencil_desc.SampleDesc.Quality = levelAntiAliasing;
	}
	else
	{
		stencil_desc.SampleDesc.Count = 1;
		stencil_desc.SampleDesc.Quality = 0;
	}
	stencil_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	stencil_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 메모리 할당 방법 정의
	D3D12_HEAP_PROPERTIES heap_properties{};
	ZeroMemory(&heap_properties, sizeof(heap_properties));

	heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_properties.CreationNodeMask = 1;
	heap_properties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue{};
	ZeroMemory(&d3dClearValue, sizeof(d3dClearValue));

	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	uiid = __uuidof(ID3D12Resource);
	place = reinterpret_cast<void**>(&myDepthStencilBuffer);
	myDevice->CreateCommittedResource(&heap_properties, D3D12_HEAP_FLAG_NONE
		, &stencil_desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue
		, uiid, place);
	if (FAILED(valid))
	{
		throw "깊이 스텐실 뷰의 버퍼를 생성하는데 실패!";
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
	ZeroMemory(&dsv_desc, sizeof(dsv_desc));

	dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

	auto cpu_ptr_handle = heapDsvDesc->GetCPUDescriptorHandleForHeapStart();

	myDevice->CreateDepthStencilView(myDepthStencilBuffer, &dsv_desc, cpu_ptr_handle);
}

void GameFramework::Start()
{
	ResetCmdList();
	BuildPipeline();
	BuildAssets();
	BuildStages();
	BuildWorld();
	BuildParticles();
	BuildPlayer();
	BuildTerrains();
	BuildObjects();
	CloseCmdList();

	// 3D 객체, 인덱스 버퍼, 업로드 힙, 파이프라인과 쉐이더 생성
	P3DCommandList cmd_lists[] = { myCommandList };
	ExecuteCmdList(cmd_lists, std::size(cmd_lists));

	WaitForGpuComplete();

	CleanupBuilds();
}

void GameFramework::BuildPipeline()
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
	valid = myDevice->CreateRootSignature(gpu_mask
		, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize()
		, uuid, place);
	if (FAILED(valid))
	{
		throw "루트 서명을 생성하지 못함!";
	}

	if (signature_blob)
	{
		signature_blob->Release();
	}

	if (error_blob)
	{
		error_blob->Release();
	}

	myRootSignature = signature;

	myDefaultShader = new CIlluminatedShader();
	myDefaultShader->CreateShader(myDevice, myCommandList, myRootSignature);
	myDefaultShader->InitializeUniforms(myDevice, myCommandList);
}

void GameFramework::BuildAssets()
{
	auto model_rallycar = RegisterModel("Model/RallyCar.bin", "RallyCar");
	auto model_policecar = RegisterModel("Model/PoliceCar.bin", "PoliceCar");
	auto model_rock1 = RegisterModel("Model/Rock.bin", "Rock1");
	auto model_rock2 = RegisterModel("Model/Rock2.bin", "Rock2");
	auto model_tree = RegisterModel("Model/Tree.bin", "Tree");
	auto model_cactus = RegisterModel("Model/Cactus.bin", "Cactus");
}

void GameFramework::BuildStages()
{
	auto room_main = RegisterScene(StageMain(*this, myWindow));
	auto room_game = RegisterScene(StageGame(*this, myWindow));
	auto room_complete = RegisterScene(StageGameEnd(*this, myWindow));
	auto room_credit = RegisterScene(StageCredit(*this, myWindow));
	
	//AddStage(room_intro);
	AddStage(room_main);
	AddStage(room_game);
	AddStage(room_complete);
	AddStage(room_credit);
	JumpToStage(0);

	for (auto& stage_pair : myScenes)
	{
		auto& scene = stage_pair.second;

		if (scene)
		{
			scene->Awake(myDevice, myCommandList);
			scene->SetRootSignature(myRootSignature);
		}
		else
		{
			throw "잘못된 스테이지가 있음!";
		}
	}
}

void GameFramework::BuildWorld()
{

}

void GameFramework::BuildParticles()
{}

void GameFramework::BuildPlayer()
{
}

void GameFramework::BuildTerrains()
{}

void GameFramework::BuildObjects()
{}

void GameFramework::CleanupBuilds()
{
	if (currentScene)
	{
		currentScene->OnInialized();
	}
}

void GameFramework::Update(float elapsed_time)
{
	static UCHAR pKeysBuffer[256];
	const auto input = GetKeyboardState(pKeysBuffer);

	if (currentScene)
	{
		//currentScene->ProcessInput(pKeysBuffer);
	}

	if (TRUE == input)
	{
	}

	if (currentScene)
	{
		currentScene->Update(elapsed_time);
	}
}

void GameFramework::PrepareRendering()
{
	ResetCmdAllocator();
	ResetCmdList();
	SetBarrier(barrierSwap);

	auto cpu_rtv_handle = GetRTVHandle();
	auto frame_ptr = static_cast<size_t>(indexFrameBuffer * szRtvDescIncrements);
	AddtoDescriptor(cpu_rtv_handle, frame_ptr);
	ClearRenderTargetView(cpu_rtv_handle);

	auto cpu_dsv_handle = GetDSVHandle();
	ClearDepthStencilView(cpu_dsv_handle);

	ReadyOutputMerger(cpu_rtv_handle, cpu_dsv_handle);
}

void GameFramework::Render()
{
	PrepareRendering();

	if (currentScene)
	{
		currentScene->Render();
	}

	SetBarrier(barrierRender);

	CloseCmdList();
	P3DCommandList cmd_lists[] = { myCommandList };
	ExecuteCmdList(cmd_lists, std::size(cmd_lists));

	WaitForGpuComplete();

	mySwapChain->Present(0, 0);

	AfterRendering();
}

void GameFramework::AfterRendering()
{
	// 프레임 갱신
	indexFrameBuffer = mySwapChain->GetCurrentBackBufferIndex();

	WaitForGpuComplete();
}

void GameFramework::WaitForGpuComplete()
{
	const auto signal = ++myFences[indexFrameBuffer];
	SignalToFence(signal);

	if (myRenderFence->GetCompletedValue() < signal)
	{
		SetFenceEvent(eventFence, signal);

		WaitForSingleObject(eventFence, INFINITE);
	}
}

template<typename SceneType>
	requires(std::is_base_of_v<Scene, SceneType>)
constexpr shared_ptr<Scene> GameFramework::RegisterScene(SceneType&& stage)
{
	auto handle = shared_ptr<SceneType>(std::forward<SceneType*>(new SceneType(stage)));
	auto ptr = std::static_pointer_cast<Scene>(handle);

	myScenes.try_emplace(ptr->GetName(), ptr);

	return ptr;
}

void GameFramework::AddStage(const shared_ptr<Scene>& stage)
{
	myStages.push_back(stage);
}

bool GameFramework::JumpToStage(const size_t index)
{
	if (0 <= index && index < myStages.size())
	{
		myStageIterator = myStages.begin() + index;

		const auto& target = GetStage(index);
		if (currentScene)
		{
			currentScene->Reset();
		}

		currentScene = target.lock();
		currentScene->Start();

		return true;
	}

	return false;
}

bool GameFramework::JumpToStage(const std::vector<shared_ptr<Scene>>::iterator it)
{
	myStageIterator = it;

	if (currentScene)
	{
		currentScene->Reset();
	}
	currentScene = *myStageIterator;
	currentScene->Start();

	return nullptr != currentScene;
}

bool GameFramework::JumpToNextStage()
{
	if (myStages.end() != myStageIterator)
	{
		return JumpToStage(myStageIterator + 1);
	}

	return false;
}

shared_ptr<Model> GameFramework::RegisterModel(const char* path, const char* name)
{
	auto handle = Model::Load(myDevice, myCommandList, myDefaultShader, path);
	auto ptr = shared_ptr<Model>(handle);

	myModels.insert({ name, ptr });

	return ptr;
}

weak_ptr<Scene> GameFramework::GetScene(const char* name) const
{
	return myScenes.find(name)->second;
}

weak_ptr<Scene> GameFramework::GetStage(const size_t index) const
{
	return myStages.at(index);
}

weak_ptr<Scene> GameFramework::GetNextStage() const
{
	return *(myStageIterator + 1);
}

weak_ptr<Scene> GameFramework::GetCurrentScene() const
{
	return currentScene;
}

weak_ptr<Model> GameFramework::GetModel(const char* name) const
{
	return myModels.find(name)->second;
}

void GameFramework::ToggleFullscreen()
{
	HRESULT valid = 0;

	BOOL fullscreen = FALSE;
	valid = mySwapChain->GetFullscreenState(&fullscreen, NULL);
	if (FAILED(valid))
	{
		throw "스왑 체인에서 전체 화면 상태를 얻어오는데에 실패!";
	}

	valid = mySwapChain->SetFullscreenState(!fullscreen, NULL);
	if (FAILED(valid))
	{
		throw "스왑 체인의 전체 화면 상태를 설정하는데에 실패!";
	}

	DXGI_MODE_DESC sw_mode{};
	ZeroMemory(&sw_mode, sizeof(sw_mode));

	sw_mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sw_mode.Width = frameWidth;
	sw_mode.Height = frameHeight;
	sw_mode.RefreshRate.Numerator = 60;
	sw_mode.RefreshRate.Denominator = 1;
	sw_mode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sw_mode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	valid = mySwapChain->ResizeTarget(&sw_mode);
	if (FAILED(valid))
	{
		throw "스왑 체인의 상태를 설정하는데 실패!";
	}

	for (UINT i = 0; i < numberFrameBuffers; i++)
	{
		if (resSwapChainBackBuffers[i])
		{
			resSwapChainBackBuffers[i]->Release();
		}
	}

	DXGI_SWAP_CHAIN_DESC frame_desc{};
	ZeroMemory(&frame_desc, sizeof(frame_desc));

	valid = mySwapChain->GetDesc(&frame_desc);
	if (FAILED(valid))
	{
		throw "스왑 체인의 서술자를 가져오는데 실패!";
	}

	valid = mySwapChain->ResizeBuffers(numberFrameBuffers, frameWidth, frameHeight, frame_desc.BufferDesc.Format, frame_desc.Flags);
	if (FAILED(valid))
	{
		throw "후면 렌더링 버퍼의 크기를 재정의 하는데에 실패!";
	}

	indexFrameBuffer = mySwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

void GameFramework::OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{
	if (currentScene)
	{
		currentScene->OnMouse(hwnd, msg, btn, info);
	}
}

void GameFramework::OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
	if (currentScene)
	{
		currentScene->OnKeyboard(hwnd, msg, key, state);
	}

	switch (msg)
	{
		case WM_KEYUP:
		{
			switch (key)
			{
				case VK_ESCAPE:
				{
					WaitForGpuComplete();

					PostQuitMessage(0);
				}
				break;

				case VK_RETURN:
				{}
				break;

				case VK_F9:
				{
					ToggleFullscreen();
				}
				break;

				case VK_F5:
				{}
				break;

				default:
				{}
				break;
			}
		}
		break;

		default:
		{}
		break;
	}
}

void GameFramework::OnWindowsEvent(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (currentScene)
	{
		currentScene->OnWindows(hwnd, msg, wp, lp);
	}

	switch (msg)
	{
		case WM_SIZE:
		{}
		break;
	}
}

bool GameFramework::D3DAssert(HRESULT valid, const char* error)
{
	if (FAILED(valid))
	{
		throw error;
		return false;
	}
	else
	{
		return true;
	}
}

void GameFramework::ResetCmdAllocator()
{
	D3DAssert(myCommandAlloc->Reset()
		, "명령어 할당자의 초기화 실패!");
}

void GameFramework::ResetCmdList(ID3D12PipelineState* pipeline)
{
	D3DAssert(myCommandList->Reset(myCommandAlloc, pipeline)
		, "명령어 리스트의 초기화 실패!");
}

void GameFramework::CloseCmdList()
{
	D3DAssert(myCommandList->Close()
		, "명령어 리스트의 닫기 실패");
}

void GameFramework::ExecuteCmdList(P3DCommandList list[], size_t count)
{
	myCommandQueue->ExecuteCommandLists(static_cast<UINT>(count), list);
}

DESC_HANDLE& GameFramework::AddtoDescriptor(DESC_HANDLE& handle, const size_t increment)
{
	handle.ptr += increment;
	return handle;
}

DESC_HANDLE GameFramework::GetRTVHandle() const
{
	return heapRtvDesc->GetCPUDescriptorHandleForHeapStart();
}

DESC_HANDLE GameFramework::GetDSVHandle() const
{
	return heapDsvDesc->GetCPUDescriptorHandleForHeapStart();
}

inline void GameFramework::ClearRenderTargetView(DESC_HANDLE& handle
	, D3D12_RECT* erase_rects, size_t erase_count)
{
	myCommandList->ClearRenderTargetView(handle
		, currentScene->myBackgroundColor
		, static_cast<UINT>(erase_count), erase_rects);
}

void GameFramework::ClearDepthStencilView(DESC_HANDLE& handle
	, float depth, UINT8 stencil
	, D3D12_RECT* erase_rects, size_t erase_count)
{
	myCommandList->ClearDepthStencilView(handle
		, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL
		, depth, stencil
		, static_cast<UINT>(erase_count), erase_rects);
}

void GameFramework::ReadyOutputMerger(DESC_HANDLE& rtv, DESC_HANDLE& dsv)
{
	constexpr BOOL is_single = TRUE;
	myCommandList->OMSetRenderTargets(1, &rtv, is_single, &dsv);
}

void GameFramework::SetFenceEvent(HANDLE signal, UINT64 limit)
{
	D3DAssert(myRenderFence->SetEventOnCompletion(limit, eventFence)
		, "렌더링 완료 이벤트 설정 실패!");
}

void GameFramework::SignalToFence(UINT64 count)
{
	D3DAssert(myCommandQueue->Signal(myRenderFence, count)
		, "명령 큐에 신호 보내기 실패!");
}

void GameFramework::SetBarrier(UINT type)
{
	auto& barrier = myBarriers[type];
	barrier.Transition.pResource = resSwapChainBackBuffers[indexFrameBuffer];

	myCommandList->ResourceBarrier(1, &barrier);
}
