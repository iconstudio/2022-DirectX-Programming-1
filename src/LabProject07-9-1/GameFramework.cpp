
#include "stdafx.h"
#include "GameFramework.h"

GameFramework::GameFramework(unsigned int width, unsigned int height)
	: frameWidth(width), frameHeight(height)
	, isAntiAliasingEnabled(false), levelAntiAliasing(0)
	, indexFrameBuffer(0)
	, myFactory(nullptr), myDevice(nullptr)
	, myRenderFence(nullptr), myFences(), eventFence(NULL)
	, myCommandList(nullptr), myCommandQueue(nullptr), myCommandAlloc(nullptr)
	, mySwapChain(nullptr), resSwapChainBackBuffers()
	, heapRtvDesc(nullptr), szRtvDescIncrements(0)
	, myDepthStencilBuffer(nullptr)
	, heapDsvDesc(nullptr), szDsvDescIncrements(0)
{
	m_pScene = NULL;
	m_pPlayer = NULL;

	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));
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
	myHInstance = hInstance;
	myWindow = hMainWnd;

	CreateDirect3DDevice();
	CreateCommanders();
	CreateSwapChain();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	BuildObjects();
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

void GameFramework::ToggleFullscreen()
{
	HRESULT valid = 0;
	IID uiid{};
	void** place = nullptr;
	ZeroMemory(&uiid, sizeof(uiid));

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

void GameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			::ReleaseCapture();
			break;
		case WM_MOUSEMOVE:
			break;
		default:
			break;
	}
}

void GameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
		case WM_KEYUP:
			switch (wParam)
			{
				case VK_ESCAPE:
					::PostQuitMessage(0);
					break;
				case VK_RETURN:
					break;
				case VK_F1:
				case VK_F2:
				case VK_F3:
					m_pCamera = m_pPlayer->ChangeCamera((DWORD)(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
					break;
				case VK_F9:
					ToggleFullscreen();
					break;
				case VK_F5:
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

LRESULT CALLBACK GameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
				m_GameTimer.Stop();
			else
				m_GameTimer.Start();
			break;
		}
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
	}
	return(0);
}

void GameFramework::BuildObjects()
{
	myCommandList->Reset(myCommandAlloc, NULL);

	m_pScene = new CScene();
	if (m_pScene) m_pScene->BuildObjects(myDevice, myCommandList);

	CAirplanePlayer *pAirplanePlayer = new CAirplanePlayer(myDevice, myCommandList, m_pScene->GetGraphicsRootSignature());
	pAirplanePlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pScene->m_pPlayer = m_pPlayer = pAirplanePlayer;
	m_pCamera = m_pPlayer->GetCamera();

	myCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { myCommandList };
	myCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

	if (m_pScene) m_pScene->ReleaseUploadBuffers();
	if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();

	m_GameTimer.Reset();
}

void GameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
}

void GameFramework::ProcessInput()
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
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void GameFramework::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();

	if (m_pScene) m_pScene->AnimateObjects(fTimeElapsed);

	m_pPlayer->Animate(fTimeElapsed, NULL);
}

void GameFramework::WaitForGpuComplete()
{
	const UINT64 nFenceValue = ++myFences[indexFrameBuffer];
	HRESULT hResult = myCommandQueue->Signal(myRenderFence, nFenceValue);

	if (myRenderFence->GetCompletedValue() < nFenceValue)
	{
		hResult = myRenderFence->SetEventOnCompletion(nFenceValue, eventFence);
		::WaitForSingleObject(eventFence, INFINITE);
	}
}

void GameFramework::MoveToNextFrame()
{
	indexFrameBuffer = mySwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++myFences[indexFrameBuffer];
	HRESULT hResult = myCommandQueue->Signal(myRenderFence, nFenceValue);

	if (myRenderFence->GetCompletedValue() < nFenceValue)
	{
		hResult = myRenderFence->SetEventOnCompletion(nFenceValue, eventFence);
		::WaitForSingleObject(eventFence, INFINITE);
	}
}

//#define _WITH_PLAYER_TOP

void GameFramework::FrameAdvance()
{    
	m_GameTimer.Tick(0.0f);
	
	ProcessInput();

    AnimateObjects();

	HRESULT hResult = myCommandAlloc->Reset();
	hResult = myCommandList->Reset(myCommandAlloc, NULL);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = resSwapChainBackBuffers[indexFrameBuffer];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	myCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = heapRtvDesc->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (indexFrameBuffer * szRtvDescIncrements);

	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	myCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = heapDsvDesc->GetCPUDescriptorHandleForHeapStart();
	myCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	myCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);

	if (m_pScene) m_pScene->Render(myCommandList, m_pCamera);

#ifdef _WITH_PLAYER_TOP
	myCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	if (m_pPlayer) m_pPlayer->Render(myCommandList, m_pCamera);

	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	myCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	hResult = myCommandList->Close();
	
	ID3D12CommandList *ppd3dCommandLists[] = { myCommandList };
	myCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

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

	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	size_t nLength = _tcslen(m_pszFrameRate);
	XMFLOAT3 xmf3Position = m_pPlayer->GetPosition();
	_stprintf_s(m_pszFrameRate + nLength, 70 - nLength, _T("(%4f, %4f, %4f)"), xmf3Position.x, xmf3Position.y, xmf3Position.z);
	::SetWindowText(myWindow, m_pszFrameRate);
}

