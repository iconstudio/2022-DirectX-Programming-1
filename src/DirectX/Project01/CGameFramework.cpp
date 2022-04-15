#include "pch.h"
#include "CGameFramework.hpp"
#include "CommandController.hpp"
#include "CGameTimer.h"

CGameFramework::CGameFramework()
	: m_nWndClientWidth(::FRAME_BUFFER_WIDTH), m_nWndClientHeight(::FRAME_BUFFER_HEIGHT)
	, m_pdxgiFactory(nullptr), m_pd3dDevice(nullptr)
	, m_pd3dPipelineState(nullptr)
	, m_pdxgiSwapChain(nullptr), m_nSwapChainBufferIndex(0)
	, m_pd3dRtvDescriptorHeap(nullptr), m_nRtvDescriptorIncrementSize(0)
	, m_pd3dDsvDescriptorHeap(nullptr), m_nDsvDescriptorIncrementSize(0)
	, m_pd3dDepthStencilBuffer(nullptr)
	, controlCommands()
	, Timer()
{
	ZeroMemory(m_ppd3dRenderTargetBuffers, sizeof(m_ppd3dRenderTargetBuffers));

	dxgiRenderTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiRenderTargetParameters.RefreshRate.Numerator = 60;
	dxgiRenderTargetParameters.RefreshRate.Denominator = 1;
	dxgiRenderTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiRenderTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	wcscpy_s(m_pszFrameRate, L"LapProject (");
}

CGameFramework::~CGameFramework()
{}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D 디바이스, 명령 큐와 명령 리스트, 스왑 체인 등을 생성하는 함수를 호출한다.
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();

	//렌더링할 게임 객체를 생성한다.
	BuildObjects();

	return true;
}

void CGameFramework::OnDestroy()
{
	controlCommands.WaitForGpuComplete(); //GPU가 모든 명령 리스트를 실행할 때 까지 기다린다.

	ReleaseObjects(); //게임 객체(게임 월드 객체)를 소멸한다.

	controlCommands.OnDestroy();
	for (int i = 0; i < m_nSwapChainBuffers; i++)
	{
		if (m_ppd3dRenderTargetBuffers[i])
		{
			m_ppd3dRenderTargetBuffers[i]->Release();
		}
	}
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();

	if (m_pdxgiSwapChain)
	{
		m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
		m_pdxgiSwapChain->Release();
	}
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void CGameFramework::CreateDirect3DDevice()
{
	UINT flags = 0;
#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = NULL;
	auto hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}

	flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	void** place = reinterpret_cast<void**>(&m_pdxgiFactory);
	IID id = _uuidof(IDXGIFactory2);

	auto result = CreateDXGIFactory2(flags, id, place);
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateDXGIFactory2()");
		return;
	}

	// 모든 하드웨어 어댑터에 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.
	auto feature = D3D_FEATURE_LEVEL_12_0;
	IDXGIAdapter1* pd3dAdapter = nullptr;
	DXGI_ADAPTER_DESC1 dxgiAdapterDesc{};

	place = reinterpret_cast<void**>(&m_pd3dDevice);
	id = _uuidof(ID3D12Device);

	UINT index = 0;
	while (true)
	{
		if (DXGI_ERROR_NOT_FOUND == m_pdxgiFactory->EnumAdapters1(index, &pd3dAdapter))
		{
			break;
		}

		ZeroMemory(&dxgiAdapterDesc, sizeof(dxgiAdapterDesc));
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

		result = D3D12CreateDevice(pd3dAdapter, feature, id, place);
		if (SUCCEEDED(result)) break;

		index++;
	}

	//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.
	if (!pd3dAdapter)
	{
		auto warp_place = reinterpret_cast<void**>(&pd3dAdapter);
		result = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), warp_place);
		if (FAILED(result))
		{
			ErrorDisplay(L"EnumWarpAdapter()");
			return;
		}

		result = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, id, place);
		if (FAILED(result))
		{
			ErrorDisplay(L"EnumWarpAdapter → D3D12CreateDevice()");
			return;
		}
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels{};
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;

	// 디바이스가 지원하는 다중 샘플의 품질 수준을 확인한다.
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;

	// 다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화한다.
	m_bMsaa4xEnable = (1 < m_nMsaa4xQualityLevels) ? true : false;

	//뷰포트를 주 윈도우의 클라이언트 영역 전체로 설정한다.
	m_d3dViewport.TopLeftX = 0;
	m_d3dViewport.TopLeftY = 0;
	m_d3dViewport.Width = static_cast<float>(m_nWndClientWidth);
	m_d3dViewport.Height = static_cast<float>(m_nWndClientHeight);
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.MaxDepth = 1.0f;

	//씨저 사각형을 주 윈도우의 클라이언트 영역 전체로 설정한다.
	m_d3dScissorRect = { 0, 0, m_nWndClientWidth, m_nWndClientHeight };

	if (pd3dAdapter) (pd3dAdapter)->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	controlCommands.SetDevice(m_pd3dDevice);
	controlCommands.OnCreate();
}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);

	auto width = static_cast<int>(rcClient.right - rcClient.left);
	auto height = static_cast<int>(rcClient.bottom - rcClient.top);
	SetRenderingSize(width, height);

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferDesc = dxgiRenderTargetParameters;
	//dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	//dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	//dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	//dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;

	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.Windowed = TRUE;

	//dxgiSwapChainDesc.BufferDesc.Scaling = DXGI_SCALING_NONE;
	//dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	//전체화면 모드에서 바탕화면의 해상도를 바꾸지 않고 후면버퍼의 크기를 바탕화면 크기로 변경한다.
	dxgiSwapChainDesc.Flags = 0;
#else
	//전체화면 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경한다.
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	auto queue = controlCommands.m_pd3dCommandQueue;

	// 스왑체인을 생성한다.
	HRESULT result = m_pdxgiFactory->CreateSwapChain(queue, &dxgiSwapChainDesc
		, (IDXGISwapChain**)&m_pdxgiSwapChain);
	
	/*
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	HRESULT result = m_pdxgiFactory->CreateSwapChainForHwnd(queue, m_hWnd
		, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc
		, NULL, (IDXGISwapChain1**)&m_pdxgiSwapChain);
	*/
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateSwapChain()");
		return;
	}

	// “Alt+Enter” 키의 동작을 비활성화한다
	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

	// 스왑체인의 현재 후면버퍼 인덱스를 저장한다.
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);
	// 렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성한다. 

	// 렌더 타겟 서술자 힙의 원소의 크기를 저장한다.
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	// 깊이-스텐실 서술자 힙(서술자의 개수는 1)을 생성한다.
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);

	// 깊이-스텐실 서술자 힙의 원소의 크기를 저장한다.
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

// 스왑체인의 각 후면 버퍼에 대한 렌더 타겟 뷰를 생성한다.
void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void
			**)&m_ppd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL,
			d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	// 깊이-스텐실 버퍼를 생성한다.
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// 깊이-스텐실 버퍼 뷰를 생성한다.
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
}

void CGameFramework::BuildObjects()
{}

void CGameFramework::ReleaseObjects()
{}

void CGameFramework::SetRenderingSize(const int width, const int height)
{
	m_nWndClientWidth = width;
	m_nWndClientHeight = height;

	dxgiRenderTargetParameters.Width = width;
	dxgiRenderTargetParameters.Height = height;
}

void CGameFramework::OnResizeBackBuffers()
{
	controlCommands.WaitForGpuComplete();

	HRESULT result = controlCommands.TryResetList();
	if (FAILED(result))
	{
		ErrorDisplay(L"OnResizeBackBuffers → ID3D12GraphicsCommandList.Reset()");
		return;
	}

	for (int i = 0; i < m_nSwapChainBuffers; i++)
	{
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();
	}
	if (m_pd3dDepthStencilBuffer)
	{
		m_pd3dDepthStencilBuffer->Release();
	}

	m_nSwapChainBufferIndex = 0;
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	result = m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
#else
	result = m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers
		, m_nWndClientWidth, m_nWndClientHeight
		, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
#endif
	if (FAILED(result))
	{
		ErrorDisplay(L"OnResizeBackBuffers → ResizeBuffers()");
		return;
	}

	CreateRenderTargetViews();
	CreateDepthStencilView();

	result = controlCommands.TryCloseList();
	if (FAILED(result))
	{
		ErrorDisplay(L"OnResizeBackBuffers → ID3D12GraphicsCommandList.Close()");
		return;
	}

	controlCommands.Execute();
	controlCommands.WaitForGpuComplete();
}

void CGameFramework::ProcessInput()
{}

void CGameFramework::AnimateObjects()
{}

void CGameFramework::FrameAdvance()
{
	Timer.Tick(0.0f);

	ProcessInput();
	AnimateObjects();

	HRESULT result = controlCommands.TryResetAllocator();
	if (FAILED(result))
	{
		ErrorDisplay(L"FrameAdvance → ID3DCommandAllocator.Reset()");
		return;
	}

	result = controlCommands.TryResetList();
	if (FAILED(result))
	{
		ErrorDisplay(L"FrameAdvance → ID3DGraphicsCommandList.Reset()");
		return;
	}

	D3D12_RESOURCE_BARRIER barrier;
	::ZeroMemory(&barrier, sizeof(D3D12_RESOURCE_BARRIER));
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	/*
		현재 렌더 타겟에 대한 프리젠트가 끝나기를 기다린다.
		프리젠트가 끝나면 렌더 타겟 버퍼의 상태는
		프리젠트 상태 (D3D12_RESOURCE_STATE_PRESENT)
		에서 렌더 타겟 상태(D3D12_RESOURCE_STATE_RENDER_TARGET)로 바뀔 것이다.
	*/
	controlCommands.WaitForPresent(barrier);

	// 뷰포트와 씨저 사각형을 설정한다.
	controlCommands.RSSetViewports(m_d3dViewport);
	controlCommands.RSSetScissorRects(m_d3dScissorRect);

	// 현재의 렌더 타겟에 해당하는 서술자의 CPU 주소(핸들)를 계산한다.
	auto handleDescriptorRT = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleDescriptorRT.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	float rcolor[] = { 0.0f, 0.125f, 0.3f, 1.0f };

	// 원하는 색상으로 렌더 타겟(뷰)을 지운다.
	controlCommands.ClearRenderTargetView(handleDescriptorRT, rcolor);

	// 깊이-스텐실 서술자의 CPU 주소를 계산한다.
	auto handleDescriptorDS = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// 원하는 값으로 깊이-스텐실(뷰)을 지운다.
	controlCommands.ClearDepthStencilView(handleDescriptorDS);

	// 렌더 타겟 뷰(서술자)와 깊이-스텐실 뷰(서술자)를 출력-병합 단계(OM)에 연결한다.
	controlCommands.OMSetRenderTargets(handleDescriptorRT, handleDescriptorDS);

	// 렌더링 코드는 여기에 추가될 것이다.
	//

	/*
		현재 렌더 타겟에 대한 렌더링이 끝나기를 기다린다.
		GPU가 렌더 타겟(버퍼)을 더 이상 사용하지 않으면
		렌더 타겟의 상태는 프리젠트 상태(D3D12_RESOURCE_STATE_PRESENT)로 바뀔 것이다.
	*/
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	controlCommands.WaitForPresent(barrier);

	// 명령 리스트를 닫힌 상태로 만든다.
	result = controlCommands.TryCloseList();
	if (FAILED(result))
	{
		ErrorDisplay(L"FrameAdvance → ID3DGraphicsCommandList.Close()");
		return;
	}

	// 명령 리스트를 명령 큐에 추가하여 실행한다.
	controlCommands.Execute();

	// GPU가 모든 명령 리스트를 실행할 때 까지 기다린다.
	controlCommands.WaitForGpuComplete();

	DXGI_PRESENT_PARAMETERS presentOptions{};
	presentOptions.DirtyRectsCount = 0;
	presentOptions.pDirtyRects = NULL;
	presentOptions.pScrollRect = NULL;
	presentOptions.pScrollOffset = NULL;

	/*
		스왑체인을 프리젠트한다.
		프리젠트를 하면 현재 렌더 타겟(후면버퍼)의 내용이 전면버퍼로 옮겨지고 렌더 타겟 인덱스가 바뀔 것이다.
	*/
	result = m_pdxgiSwapChain->Present1(0, 0, &presentOptions);
	if (FAILED(result))
	{
		ErrorDisplay(L"FrameAdvance → Present1()");
		return;
	}

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	Timer.GetFrameRate(m_pszFrameRate + 12, 37);

	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		break;
		case WM_MOUSEMOVE:
		break;
		default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
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

			case VK_F8:
			break;

			// “F9” 키가 눌려지면 윈도우 모드와 전체화면 모드의 전환을 처리한다.
			case VK_F9:
			{
				BOOL bFullScreenState = FALSE;
				m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
				m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

				dxgiRenderTargetParameters.Width = m_nWndClientWidth;
				dxgiRenderTargetParameters.Height = m_nWndClientHeight;

				m_pdxgiSwapChain->ResizeTarget(&dxgiRenderTargetParameters);
				OnResizeBackBuffers();
			}
			break;

			default:
			break;
		}
		break;
		default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
				Timer.Stop();
			else
				Timer.Start();
			break;
		}

		/*
			WM_SIZE 메시지는 윈도우가 생성될 때 한번 호출되거나 윈도우의 크기가 변경될 때 호출된다.
			주 윈도우의 크기 를 사용자가 변경할 수 없으므로 윈도우의 크기가 변경되는 경우는
			윈도우 모드와 전체화면 모드의 전환이 발생할 때이다.
		*/
		case WM_SIZE:
		{
			m_nWndClientWidth = LOWORD(lParam);
			m_nWndClientHeight = HIWORD(lParam);

			// 윈도우의 크기가 변경되면 후면버퍼의 크기를 변경한다.
			OnResizeBackBuffers();
		}
		break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		}
		break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		}
		break;
	}

	return(0);
}
