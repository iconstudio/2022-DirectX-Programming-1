#include "stdafx.h"
#include "GameFramework.h"

GameFramework::GameFramework(unsigned int width, unsigned int height)
	: frameWidth(width), frameHeight(height)
	, frameBasisColour{ 0.0f, 0.125f, 0.3f, 1.0f }
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
#endif //  _DEBUG
	, m_pScene(nullptr), m_pPlayer(nullptr)
{
	ZeroMemory(resSwapChainBackBuffers, sizeof(resSwapChainBackBuffers));
	ZeroMemory(myBarriers, sizeof(myBarriers));

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
		throw "IDXGIFactory4 ���� ����!";
	}

	IDXGIAdapter1* adapter = nullptr;
	DXGI_ADAPTER_DESC1 adapter_desc;
	D3D_FEATURE_LEVEL d3d_level = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != myFactory->EnumAdapters1(i, &adapter); i++)
	{
		valid = adapter->GetDesc1(&adapter_desc);
		if (FAILED(valid))
		{
			throw "IDXGIAdapter1 ������� ������ ��� ����!";
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
			throw "���丮���� ��ü ����� ���� ����!";
		}

		uiid = __uuidof(ID3D12Device);
		place = reinterpret_cast<void**>(&myDevice);
		valid = D3D12CreateDevice(adapter, d3d_level, uiid, place);
		if (FAILED(valid))
		{
			throw "����̽� ���� & ���� ����!";
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
		throw "��Ƽ �ٸ���� ���� Ȯ�� ����!";
	}
	levelAntiAliasing = aa_desc.NumQualityLevels;
	isAntiAliasingEnabled = 1 < levelAntiAliasing;

	const auto fence_flag = D3D12_FENCE_FLAG_NONE;
	uiid = __uuidof(ID3D12Fence);
	place = reinterpret_cast<void**>(&myRenderFence);
	valid = myDevice->CreateFence(0, fence_flag, uiid, place);
	if (FAILED(valid))
	{
		throw "GPU �޸� �庮 ���� ����!";
	}

	// CreateEvent(attr, reset, init, name);
	eventFence = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == eventFence)
	{
		throw "�庮 �̺�Ʈ ��ü ���� ����!";
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
		throw "��� ť ���� ����!";
	}

	const auto list_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	uiid = __uuidof(ID3D12CommandAllocator);
	place = reinterpret_cast<void**>(&myCommandAlloc);
	valid = myDevice->CreateCommandAllocator(list_type, uiid, place);
	if (FAILED(valid))
	{
		throw "��� �Ҵ��� ���� ����!";
	}

	uiid = __uuidof(ID3D12GraphicsCommandList);
	place = reinterpret_cast<void**>(&myCommandList);
	valid = myDevice->CreateCommandList(0, list_type, myCommandAlloc, NULL, uiid, place);
	if (FAILED(valid))
	{
		throw "��� ����Ʈ ���� ����!";
	}

	valid = myCommandList->Close();
	if (FAILED(valid))
	{
		throw "��� ����Ʈ�� �ݴ� �߿� ���� �߻�!";
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
		throw "���� ü�� ���� ����!";
	}

	indexFrameBuffer = mySwapChain->GetCurrentBackBufferIndex();

	valid = myFactory->MakeWindowAssociation(myWindow, DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(valid))
	{
		throw "���� ü���� ������ ���� ����!";
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
	rtv_heap_desc.NodeMask = 0; // GPU ����ũ

	uiid = __uuidof(ID3D12DescriptorHeap);
	place = reinterpret_cast<void**>(&heapRtvDesc);
	valid = myDevice->CreateDescriptorHeap(&rtv_heap_desc, uiid, place);
	if (FAILED(valid))
	{
		throw "���� Ÿ�� ���� ������ ���� �����ϴµ� ����!";
	}
	szRtvDescIncrements = myDevice->GetDescriptorHandleIncrementSize(buffer_type);

	// CPU �޸𸮿� ������ ���� �Ҵ�
	auto cpu_ptr_handle = heapRtvDesc->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < numberFrameBuffers; i++)
	{
		uiid = __uuidof(ID3D12Resource);
		place = reinterpret_cast<void**>(&resSwapChainBackBuffers[i]);
		valid = mySwapChain->GetBuffer(i, uiid, place);
		if (FAILED(valid))
		{
			throw "�ĸ� ������ ������ �޸� �Ҵ� ����!";
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
		throw "���� ���ٽ� ���� ������ ���� �����ϴµ� ����!";
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

	// �޸� �Ҵ� ��� ����
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
		throw "���� ���ٽ� ���� ���۸� �����ϴµ� ����!";
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
	BuildStages();
	BuildWorld();
	BuildParticles();
	BuildPlayer();
	BuildTerrains();
	BuildObjects();

	CloseCmdList();
	ID3D12CommandList* cmd_lists[] = { myCommandList };
	ExecuteCmdList(cmd_lists, std::size(cmd_lists));

	WaitForGpuComplete();

	if (m_pScene) m_pScene->ReleaseUploadBuffers();
	if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();
}

void GameFramework::BuildStages()
{
	m_pScene = new CScene("Scene");

	if (m_pScene)
	{
		m_pScene->Awake(myDevice, myCommandList);
		m_pScene->BuildObjects();
	}
}

void GameFramework::BuildWorld()
{
}

void GameFramework::BuildParticles()
{}

void GameFramework::BuildPlayer()
{
	auto pAirplanePlayer = new CAirplanePlayer(myDevice, myCommandList, m_pScene->GetGraphicsRootSignature());
	pAirplanePlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	m_pScene->m_pPlayer = m_pPlayer = pAirplanePlayer;
	m_pCamera = m_pPlayer->GetCamera();
}

void GameFramework::BuildTerrains()
{}

void GameFramework::BuildObjects()
{
}

void GameFramework::Update(float elapsed_time)
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);

	if (!bProcessedByScene)
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
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
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
			if (dwDirection) m_pPlayer->Move(dwDirection, 1.5f, true);
		}
	}

	if (m_pScene)
	{
		m_pScene->Update(elapsed_time);
	}

	m_pPlayer->Animate(elapsed_time, NULL);
	m_pPlayer->Update(elapsed_time);
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

	if (m_pScene) m_pScene->Render(m_pCamera);

#ifdef _WITH_PLAYER_TOP
	myCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	if (m_pPlayer) m_pPlayer->Render(myCommandList, m_pCamera);

	SetBarrier(barrierRender);

	CloseCmdList();
	ID3D12CommandList* cmd_lists[] = { myCommandList };
	ExecuteCmdList(cmd_lists, std::size(cmd_lists));

	WaitForGpuComplete();

#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	mySwapChain->Present1(1, 0, &dxgiPresentParameters);
#else
#ifdef _WITH_SYNCH_SWAPCHAIN
	mySwapChain->Present(1, 0);
#else
	mySwapChain->Present(0, 0);
#endif
#endif

	AfterRendering();
}

void GameFramework::AfterRendering()
{
	// ������ ����
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

void GameFramework::ToggleFullscreen()
{
	HRESULT valid = 0;

	BOOL fullscreen = FALSE;
	valid = mySwapChain->GetFullscreenState(&fullscreen, NULL);
	if (FAILED(valid))
	{
		throw "���� ü�ο��� ��ü ȭ�� ���¸� �����µ��� ����!";
	}

	valid = mySwapChain->SetFullscreenState(!fullscreen, NULL);
	if (FAILED(valid))
	{
		throw "���� ü���� ��ü ȭ�� ���¸� �����ϴµ��� ����!";
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
		throw "���� ü���� ���¸� �����ϴµ� ����!";
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
		throw "���� ü���� �����ڸ� �������µ� ����!";
	}

	valid = mySwapChain->ResizeBuffers(numberFrameBuffers, frameWidth, frameHeight, frame_desc.BufferDesc.Format, frame_desc.Flags);
	if (FAILED(valid))
	{
		throw "�ĸ� ������ ������ ũ�⸦ ������ �ϴµ��� ����!";
	}

	indexFrameBuffer = mySwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

void GameFramework::OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{
	if (m_pScene)
	{
		m_pScene->OnMouseEvent(hwnd, msg, btn, info);
	}

	switch (msg)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		{
			SetCapture(hwnd);
			GetCursorPos(&m_ptOldCursorPos);
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

void GameFramework::OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
	if (m_pScene)
	{
		m_pScene->OnKeyboardEvent(hwnd, msg, key, state);
	}

	switch (msg)
	{
		case WM_KEYUP:
		{
			switch (key)
			{
				case VK_ESCAPE:
				{
					PostQuitMessage(0);
				}
				break;

				case VK_RETURN:
				break;

				case VK_F1:
				case VK_F2:
				case VK_F3:
				{
					m_pCamera = m_pPlayer->ChangeCamera((DWORD)(key - VK_F1 + 1), 0);
				}

				break;

				case VK_F9:
				{
					ToggleFullscreen();
				}
				break;

				case VK_F5:
				break;

				default:
				break;
			}
		}
		break;

		default:
		{}
		break;
	}
}

LRESULT CALLBACK GameFramework::OnWindowsEvent(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_SIZE:
		break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		OnMouseEvent(hWnd, nMessageID, wParam, lParam);
		break;
		case WM_KEYDOWN:
		case WM_KEYUP:
		OnKeyboardEvent(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
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
		, "��ɾ� �Ҵ����� �ʱ�ȭ ����!");
}

void GameFramework::ResetCmdList(ID3D12PipelineState* pipeline)
{
	D3DAssert(myCommandList->Reset(myCommandAlloc, pipeline)
		, "��ɾ� ����Ʈ�� �ʱ�ȭ ����!");
}

void GameFramework::CloseCmdList()
{
	D3DAssert(myCommandList->Close()
		, "��ɾ� ����Ʈ�� �ݱ� ����");
}

void GameFramework::ExecuteCmdList(ID3D12CommandList* list[], UINT count)
{
	myCommandQueue->ExecuteCommandLists(count, list);
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
		, frameBasisColour
		, erase_count, erase_rects);
}

void GameFramework::ClearDepthStencilView(DESC_HANDLE& handle
	, float depth, UINT8 stencil
	, D3D12_RECT* erase_rects, size_t erase_count)
{
	myCommandList->ClearDepthStencilView(handle
		, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL
		, depth, stencil
		, erase_count, erase_rects);
}

void GameFramework::ReadyOutputMerger(DESC_HANDLE& rtv, DESC_HANDLE& dsv)
{
	constexpr BOOL is_single = TRUE;
	myCommandList->OMSetRenderTargets(1, &rtv, is_single, &dsv);
}

void GameFramework::SetFenceEvent(HANDLE signal, UINT64 limit)
{
	D3DAssert(myRenderFence->SetEventOnCompletion(limit, eventFence)
		, "������ �Ϸ� �̺�Ʈ ���� ����!");
}

void GameFramework::SignalToFence(UINT64 count)
{
	D3DAssert(myCommandQueue->Signal(myRenderFence, count)
		, "��� ť�� ��ȣ ������ ����!");
}

void GameFramework::SetBarrier(UINT type)
{
	auto& barrier = myBarriers[type];
	barrier.Transition.pResource = resSwapChainBackBuffers[indexFrameBuffer];

	myCommandList->ResourceBarrier(1, &barrier);
}
