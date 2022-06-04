#include "pch.hpp"
#include "GraphicsCore.hpp"
#include "GraphicsPipeline.hpp"
#include "Shader.hpp"

GraphicsCore::GraphicsCore(long width, long height)
	: myWindow(NULL), frameWidth(width), frameHeight(height)
	, frameColor{ 0.0f, 0.0f, 0.0f, 0.0f }
	, isAntiAliasingEnabled(false), levelAntiAliasing(0)
	, indexFrameBuffer(0)
	, myFactory(nullptr), myDevice(nullptr)
	, myRenderFence(nullptr), myFences(), eventFence(NULL)
	, myCommandList(nullptr), myCommandQueue(nullptr), myCommandAlloc(nullptr)
	, mySwapChain(nullptr), resSwapChainBackBuffers(), myBarriers()
	, heapRtvDesc(nullptr), szRtvDescIncrements(0)
	, myDepthStencilBuffer(nullptr)
	, heapDsvDesc(nullptr), szDsvDescIncrements(0)
#ifdef _DEBUG
	, myDebugController(nullptr)
#endif
	, myPipelines(), currentPipeline(nullptr)
{
	myPipelines.reserve(10);

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

GraphicsCore::~GraphicsCore()
{}

GraphicsCore& GraphicsCore::SetHWND(HWND window)
{
	myWindow = window;
	return *this;
}

void GraphicsCore::Awake()
{
	CreateDirect3DDevice();
	CreateCommanders();
	CreateSwapChain();
	CreateRenderTargetViews();
	CreateDepthStencilView();
}

void GraphicsCore::Start()
{
	ResetCmdAllocator();
	ResetCmdList();
	for (auto& pipeline : myPipelines)
	{
		pipeline->Awake();
	}
	CloseCmdList();
	P3DCommandList cmd_lists[] = { myCommandList };
	ExecuteCmdList(cmd_lists, std::size(cmd_lists));

	WaitForGpuComplete();

	SetPipeline(0);
}

void GraphicsCore::Reset()
{
	for (auto& pipeline : myPipelines)
	{
		pipeline->Reset();
	}
}

void GraphicsCore::Update(float delta_time)
{}

void GraphicsCore::Release()
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

	for (auto& pipeline : myPipelines)
	{
		pipeline->Release();
	}

	if (myCommandAlloc) myCommandAlloc->Release();
	if (myCommandQueue) myCommandQueue->Release();
	if (myCommandList) myCommandList->Release();

	if (myRenderFence) myRenderFence->Release();

	if (myDevice) myDevice->Release();
	if (myFactory) myFactory->Release();

#if defined(_DEBUG)
	if (myDebugController) myDebugController->Release();

	IDXGIDebug1* debugger = NULL;

	auto uid = __uuidof(IDXGIDebug1);
	auto place = reinterpret_cast<void**>(&debugger);
	auto valid = DXGIGetDebugInterface1(0, uid, place);
	if (FAILED(valid))
	{
		throw "디버그 정보 출력기의 생성 실패!";
	}

	valid = debugger->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	if (FAILED(valid))
	{
		throw "디버그 정보 출력 실패!";
	}

	debugger->Release();
#endif
}

void GraphicsCore::WaitForGpuComplete()
{
	const auto signal = ++myFences[indexFrameBuffer];
	SignalToFence(signal);

	if (myRenderFence->GetCompletedValue() < signal)
	{
		SetFenceEvent(eventFence, signal);

		WaitForSingleObject(eventFence, INFINITE);
	}
}

void GraphicsCore::PrepareRendering()
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

	currentPipeline->PrepareRendering();
	currentPipeline->Render();
}

void GraphicsCore::Render()
{
	SetBarrier(barrierRender);

	CloseCmdList();
	P3DCommandList cmd_lists[] = { myCommandList };
	ExecuteCmdList(cmd_lists, std::size(cmd_lists));

	WaitForGpuComplete();

	mySwapChain->Present(0, 0);

	indexFrameBuffer = mySwapChain->GetCurrentBackBufferIndex();

	WaitForGpuComplete();
}

void GraphicsCore::ToggleFullscreen()
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

void GraphicsCore::CreateDirect3DDevice()
{
	HRESULT valid = 0;
	IID uiid{};
	void** place = nullptr;

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

void GraphicsCore::CreateCommanders()
{
	HRESULT valid = 0;
	IID uiid{};
	void** place = nullptr;

	D3D12_COMMAND_QUEUE_DESC queue_desc{};
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

void GraphicsCore::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sw_desc{};
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
	auto valid = myFactory->CreateSwapChain(myCommandQueue, &sw_desc, place);
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

void GraphicsCore::CreateRenderTargetViews()
{
	HRESULT valid = 0;
	IID uiid{};
	void** place = nullptr;

	const auto buffer_type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc{};
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

void GraphicsCore::CreateDepthStencilView()
{
	HRESULT valid = 0;
	IID uiid{};
	void** place = nullptr;

	const auto buffer_type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc{};
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
	heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_properties.CreationNodeMask = 1;
	heap_properties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue{};
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
	dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

	auto cpu_ptr_handle = heapDsvDesc->GetCPUDescriptorHandleForHeapStart();

	myDevice->CreateDepthStencilView(myDepthStencilBuffer, &dsv_desc, cpu_ptr_handle);
}

bool GraphicsCore::D3DAssert(HRESULT valid, const char* error)
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

void GraphicsCore::ResetCmdAllocator()
{
	D3DAssert(myCommandAlloc->Reset()
		, "명령어 할당자의 초기화 실패!");
}

void GraphicsCore::ResetCmdList(ID3D12PipelineState* pipeline)
{
	D3DAssert(myCommandList->Reset(myCommandAlloc, pipeline)
		, "명령어 리스트의 초기화 실패!");
}

void GraphicsCore::CloseCmdList()
{
	D3DAssert(myCommandList->Close()
		, "명령어 리스트의 닫기 실패");
}

void GraphicsCore::ExecuteCmdList(P3DCommandList list[], size_t count)
{
	myCommandQueue->ExecuteCommandLists(static_cast<UINT>(count), list);
}

D3DHandle& GraphicsCore::AddtoDescriptor(D3DHandle& handle, const size_t increment)
{
	handle.ptr += increment;
	return handle;
}

D3DHandle GraphicsCore::GetRTVHandle() const
{
	return heapRtvDesc->GetCPUDescriptorHandleForHeapStart();
}

D3DHandle GraphicsCore::GetDSVHandle() const
{
	return heapDsvDesc->GetCPUDescriptorHandleForHeapStart();
}

inline void GraphicsCore::ClearRenderTargetView(D3DHandle& handle
	, D3D12_RECT* erase_rects, size_t erase_count)
{
	myCommandList->ClearRenderTargetView(handle
		, frameColor
		, static_cast<UINT>(erase_count), erase_rects);
}

void GraphicsCore::ClearDepthStencilView(D3DHandle& handle
	, float depth, UINT8 stencil
	, D3D12_RECT* erase_rects, size_t erase_count)
{
	myCommandList->ClearDepthStencilView(handle
		, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL
		, depth, stencil
		, static_cast<UINT>(erase_count), erase_rects);
}

void GraphicsCore::ReadyOutputMerger(D3DHandle& rtv, D3DHandle& dsv)
{
	constexpr BOOL is_single = TRUE;
	myCommandList->OMSetRenderTargets(1, &rtv, is_single, &dsv);
}

void GraphicsCore::SetFenceEvent(HANDLE signal, UINT64 limit)
{
	D3DAssert(myRenderFence->SetEventOnCompletion(limit, eventFence)
		, "렌더링 완료 이벤트 설정 실패!");
}

void GraphicsCore::SignalToFence(UINT64 count)
{
	D3DAssert(myCommandQueue->Signal(myRenderFence, count)
		, "명령 큐에 신호 보내기 실패!");
}

void GraphicsCore::SetBarrier(UINT type)
{
	auto& barrier = myBarriers[type];
	barrier.Transition.pResource = resSwapChainBackBuffers[indexFrameBuffer];

	myCommandList->ResourceBarrier(1, &barrier);
}

void GraphicsCore::RegisterPipeline(const GraphicsPipeline& pipeline)
{
	myPipelines.emplace_back(new GraphicsPipeline(pipeline));
}

void GraphicsCore::RegisterPipeline(GraphicsPipeline&& pipeline)
{
	myPipelines.emplace_back(new GraphicsPipeline(std::forward<GraphicsPipeline>(pipeline)));
}

GraphicsPipeline GraphicsCore::CreateEmptyPipeline() const
{
	return GraphicsPipeline(myDevice, myCommandList);
}

Shader GraphicsCore::CreateEmptyShader(const char* version) const
{
	return Shader(version);
}

D3D12_INPUT_LAYOUT_DESC GraphicsCore::CreateEmptyInputLayout() const
{
	return D3D12_INPUT_LAYOUT_DESC{ nullptr, 0 };
}

D3D12_RASTERIZER_DESC GraphicsCore::CreateEmptyRasterizerState() const
{
	return D3D12_RASTERIZER_DESC{ D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK
		, FALSE, 0, 0.0f, 0.0f, TRUE
		, FALSE, FALSE, 0
		, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF };
}

D3D12_BLEND_DESC GraphicsCore::CreateEmptyBlendState() const
{
	auto blend_dest = D3D12_BLEND_DESC{ FALSE, FALSE };
	auto& target_desc = blend_dest.RenderTarget[0];
	target_desc.BlendEnable = FALSE;
	target_desc.LogicOpEnable = FALSE;
	target_desc.SrcBlend = D3D12_BLEND_ONE;
	target_desc.DestBlend = D3D12_BLEND_ZERO;
	target_desc.BlendOp = D3D12_BLEND_OP_ADD;
	target_desc.SrcBlendAlpha = D3D12_BLEND_ONE;
	target_desc.DestBlendAlpha = D3D12_BLEND_ZERO;
	target_desc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	target_desc.LogicOp = D3D12_LOGIC_OP_NOOP;
	target_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return blend_dest;
}

D3D12_DEPTH_STENCIL_DESC GraphicsCore::CreateEmptyDepthStencilState() const
{
	return D3D12_DEPTH_STENCIL_DESC{ TRUE
		, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_LESS
		, FALSE, 0x00, 0x00
		, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP
		, D3D12_COMPARISON_FUNC_NEVER
		, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP
		, D3D12_COMPARISON_FUNC_NEVER };
}

void GraphicsCore::SetPipeline(const int index)
{
	currentPipeline = myPipelines[index];
}

const GraphicsPipeline& GraphicsCore::GetPipeline(const int index) const
{
	return *myPipelines.at(index);
}

GraphicsPipeline& GraphicsCore::GetPipeline(const int index)
{
	return *myPipelines.at(index);
}

P3DGrpCommandList GraphicsCore::GetCommandList()
{
	return myCommandList;
}
