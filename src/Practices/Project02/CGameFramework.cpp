#include "pch.h"
#include "CGameFramework.hpp"
#include "CommandController.hpp"
#include "CScene.hpp"
#include "CGameTimer.h"

CGameFramework::CGameFramework()
	: m_nWndClientWidth(::FRAME_BUFFER_WIDTH), m_nWndClientHeight(::FRAME_BUFFER_HEIGHT)
	, m_pdxgiFactory(nullptr), m_pd3dDevice(nullptr)
	, m_pdxgiSwapChain(nullptr), m_nSwapChainBufferIndex(0)
	, m_hFenceEvent(NULL), m_pd3dFence(nullptr), m_nFenceValues(), m_nFenceValue(0)
	, m_pd3dRtvDescriptorHeap(nullptr), m_nRtvDescriptorIncrementSize(0)
	, m_pd3dDsvDescriptorHeap(nullptr), m_nDsvDescriptorIncrementSize(0)
	, m_pd3dDepthStencilBuffer(nullptr)
	, controlCommands()
	, m_pScene(nullptr)
	, Timer()
{
	ZeroMemory(m_ppd3dRenderTargetBuffers, sizeof(m_ppd3dRenderTargetBuffers));
	ZeroMemory(m_nFenceValues, sizeof(m_nFenceValues));

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

	// Direct3D ����̽�, ���� ť�� ���� ����Ʈ, ���� ü�� ���� �����ϴ� �Լ��� ȣ���Ѵ�.
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();

	// �������� ���� ��ü�� �����Ѵ�.
	BuildObjects();

	return true;
}

void CGameFramework::OnDestroy()
{
	// GPU�� ��� ���� ����Ʈ�� ������ �� ���� ��ٸ���.
	WaitForGpuComplete();

	// ���� ��ü(���� ���� ��ü)�� �Ҹ��Ѵ�.
	ReleaseObjects();

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

	CloseHandle(m_hFenceEvent);
	if (m_pd3dFence) m_pd3dFence->Release();
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

	// ��� �ϵ���� ����Ϳ� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� �����Ѵ�.
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

	// Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� �����Ѵ�.
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
			ErrorDisplay(L"EnumWarpAdapter �� D3D12CreateDevice()");
			return;
		}
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels{};
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x ���� ���ø�
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;

	// ����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ���Ѵ�.
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;

	// ���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ�Ѵ�.
	m_bMsaa4xEnable = (1 < m_nMsaa4xQualityLevels) ? true : false;

	//����Ʈ�� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�.
	m_d3dViewport.TopLeftX = 0;
	m_d3dViewport.TopLeftY = 0;
	m_d3dViewport.Width = static_cast<float>(m_nWndClientWidth);
	m_d3dViewport.Height = static_cast<float>(m_nWndClientHeight);
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.MaxDepth = 1.0f;

	//���� �簢���� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�.
	m_d3dScissorRect = { 0, 0, m_nWndClientWidth, m_nWndClientHeight };

	// �潺�� �����ϰ� �潺 ���� 0���� �����Ѵ�.
	place = reinterpret_cast<void**>(&m_pd3dFence);
	id = _uuidof(ID3D12Fence);

	result = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, id, place);
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateFence()");
		return;
	}

	/*
		�潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� �����Ѵ� (�̺�Ʈ ��ü�� �ʱⰪ�� FALSE�̴�).
		�̺�Ʈ�� ����Ǹ�(Signal) �̺�Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.
	*/
	m_nFenceValue = 0;
	m_hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == m_hFenceEvent)
	{
		ErrorDisplay(L"CreateEvent()");
		return;
	}

	if (pd3dAdapter)
	{
		(pd3dAdapter)->Release();
	}
}

void CGameFramework::CreateCommandQueueAndList()
{
	controlCommands.SetDevice(m_pd3dDevice);
	controlCommands.OnCreate();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	// ���� Ÿ�� ������ ��(�������� ������ ����ü�� ������ ����)�� �����Ѵ�.
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);

	// ���� Ÿ�� ������ ���� ������ ũ�⸦ �����Ѵ�.
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	// ����-���ٽ� ������ ��(�������� ������ 1)�� �����Ѵ�.
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);

	// ����-���ٽ� ������ ���� ������ ũ�⸦ �����Ѵ�.
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);

	auto width = static_cast<int>(rcClient.right - rcClient.left);
	auto height = static_cast<int>(rcClient.bottom - rcClient.top);
	SetRenderingSize(width, height);

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
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
	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� �ٲ��� �ʰ� �ĸ������ ũ�⸦ ����ȭ�� ũ��� �����Ѵ�.
	dxgiSwapChainDesc.Flags = 0;
#else
	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� ����ü��(�ĸ����)�� ũ�⿡ �°� �����Ѵ�.
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	auto queue = controlCommands.m_pd3dCommandQueue;

	// ����ü���� �����Ѵ�.
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

	// ��Alt+Enter�� Ű�� ������ ��Ȱ��ȭ�Ѵ�
	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

	// ����ü���� ���� �ĸ���� �ε����� �����Ѵ�.
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

// ����ü���� �� �ĸ� ���ۿ� ���� ���� Ÿ�� �並 �����Ѵ�.
void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource)
			, (void**)&m_ppd3dRenderTargetBuffers[i]);

		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL,
			d3dRtvCPUDescriptorHandle);

		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(d3dResourceDesc));
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
	::ZeroMemory(&d3dHeapProperties, sizeof(d3dHeapProperties));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	::ZeroMemory(&d3dClearValue, sizeof(d3dClearValue));
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	// ����-���ٽ� ���۸� �����Ѵ�.
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// ����-���ٽ� ���� �並 �����Ѵ�.
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
}

void CGameFramework::BuildObjects()
{
	m_pScene = std::make_unique<CScene>();
	if (m_pScene)
	{
		m_pScene->BuildObjects(m_pd3dDevice);
	}

	Timer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		m_pScene.release();
	}
}

void CGameFramework::SetRenderingSize(const int width, const int height)
{
	m_nWndClientWidth = width;
	m_nWndClientHeight = height;

	dxgiRenderTargetParameters.Width = width;
	dxgiRenderTargetParameters.Height = height;
}

void CGameFramework::OnResizeBackBuffers()
{
	WaitForGpuComplete();

	HRESULT result = controlCommands.TryResetList();
	if (FAILED(result))
	{
		ErrorDisplay(L"OnResizeBackBuffers �� ID3D12GraphicsCommandList.Reset()");
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
		ErrorDisplay(L"OnResizeBackBuffers �� ResizeBuffers()");
		return;
	}

	CreateRenderTargetViews();
	CreateDepthStencilView();

	result = controlCommands.TryCloseList();
	if (FAILED(result))
	{
		ErrorDisplay(L"OnResizeBackBuffers �� ID3D12GraphicsCommandList.Close()");
		return;
	}

	controlCommands.Execute();

	WaitForGpuComplete();
}

void CGameFramework::ProcessInput()
{}

void CGameFramework::AnimateObjects()
{
	if (m_pScene)
	{
		m_pScene->AnimateObjects(Timer.GetTimeElapsed());
	}
}

void CGameFramework::WaitForGpuComplete()
{
	// CPU �潺�� ���� �����Ѵ�.
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];

	// GPU�� �潺�� ���� �����ϴ� ������ ���� ť�� �߰��Ѵ�.
	HRESULT result = controlCommands.TrySignal(m_pd3dFence, nFenceValue);
	if (FAILED(result))
	{
		ErrorDisplay(L"WaitForGpuComplete �� ID3D12GraphicsCommandList.Signal()");
		return;
	}

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		// �潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
		result = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		if (FAILED(result))
		{
			ErrorDisplay(L"WaitForGpuComplete �� ID3D12Fence.SetEventOnCompletion()");
			return;
		}

		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::FrameAdvance()
{
	Timer.Tick(0.0f);

	ProcessInput();
	AnimateObjects();

	HRESULT result = controlCommands.TryResetAllocator();
	if (FAILED(result))
	{
		ErrorDisplay(L"FrameAdvance �� ID3DCommandAllocator.Reset()");
		return;
	}

	result = controlCommands.TryResetList();
	if (FAILED(result))
	{
		ErrorDisplay(L"FrameAdvance �� ID3DGraphicsCommandList.Reset()");
		return;
	}

	D3D12_RESOURCE_BARRIER barrier;
	::ZeroMemory(&barrier, sizeof(barrier));
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	/*
		���� ���� Ÿ�ٿ� ���� ������Ʈ�� �����⸦ ��ٸ���.
		������Ʈ�� ������ ���� Ÿ�� ������ ���´�
		������Ʈ ���� (D3D12_RESOURCE_STATE_PRESENT)
		���� ���� Ÿ�� ����(D3D12_RESOURCE_STATE_RENDER_TARGET)�� �ٲ� ���̴�.
	*/
	controlCommands.WaitForPresent(barrier);

	// ����Ʈ�� ���� �簢���� �����Ѵ�.
	controlCommands.RSSetViewports(m_d3dViewport);
	controlCommands.RSSetScissorRects(m_d3dScissorRect);

	// ������ ���� Ÿ�ٿ� �ش��ϴ� �������� CPU �ּ�(�ڵ�)�� ����Ѵ�.
	auto handleDescriptorRT = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleDescriptorRT.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	float rcolor[] = { 0.0f, 0.125f, 0.3f, 1.0f };

	// ���ϴ� �������� ���� Ÿ��(��)�� �����.
	controlCommands.ClearRenderTargetView(handleDescriptorRT, rcolor);

	// ����-���ٽ� �������� CPU �ּҸ� ����Ѵ�.
	auto handleDescriptorDS = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// ���ϴ� ������ ����-���ٽ�(��)�� �����.
	controlCommands.ClearDepthStencilView(handleDescriptorDS);

	// ���� Ÿ�� ��(������)�� ����-���ٽ� ��(������)�� ���-���� �ܰ�(OM)�� �����Ѵ�.
	controlCommands.OMSetRenderTargets(handleDescriptorRT, handleDescriptorDS);

	// ������ �ڵ�� ���⿡ �߰��� ���̴�.
	if (m_pScene)
	{
		m_pScene->Render(controlCommands.m_pd3dCommandList);
	}

	/*
		���� ���� Ÿ�ٿ� ���� �������� �����⸦ ��ٸ���.
		GPU�� ���� Ÿ��(����)�� �� �̻� ������� ������
		���� Ÿ���� ���´� ������Ʈ ����(D3D12_RESOURCE_STATE_PRESENT)�� �ٲ� ���̴�.
	*/
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	controlCommands.WaitForPresent(barrier);

	// ���� ����Ʈ�� ���� ���·� �����.
	result = controlCommands.TryCloseList();
	if (FAILED(result))
	{
		ErrorDisplay(L"FrameAdvance �� ID3DGraphicsCommandList.Close()");
		return;
	}

	// ���� ����Ʈ�� ���� ť�� �߰��Ͽ� �����Ѵ�.
	controlCommands.Execute();

	// GPU�� ��� ���� ����Ʈ�� ������ �� ���� ��ٸ���.
	WaitForGpuComplete();

	DXGI_PRESENT_PARAMETERS presentOptions{};
	presentOptions.DirtyRectsCount = 0;
	presentOptions.pDirtyRects = NULL;
	presentOptions.pScrollRect = NULL;
	presentOptions.pScrollOffset = NULL;

	/*
		����ü���� ������Ʈ�Ѵ�.
		������Ʈ�� �ϸ� ���� ���� Ÿ��(�ĸ����)�� ������ ������۷� �Ű����� ���� Ÿ�� �ε����� �ٲ� ���̴�.
	*/
	result = m_pdxgiSwapChain->Present1(0, 0, &presentOptions);
	if (FAILED(result))
	{
		ErrorDisplay(L"FrameAdvance �� Present1()");
		return;
	}

	MoveToNextFrame();

	Timer.GetFrameRate(m_pszFrameRate + 12, 37);

	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];

	HRESULT result = controlCommands.TrySignal(m_pd3dFence, nFenceValue);
	if (FAILED(result))
	{
		ErrorDisplay(L"MoveToNextFrame �� ID3D12GraphicsCommandList.Signal()");
		return;
	}

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		result = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		if (FAILED(result))
		{
			ErrorDisplay(L"WaitForGpuComplete �� ID3D12Fence.SetEventOnCompletion()");
			return;
		}

		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
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

			// ��F9�� Ű�� �������� ������ ���� ��üȭ�� ����� ��ȯ�� ó���Ѵ�.
			case VK_F9:
			{
				BOOL bFullScreenState = FALSE;
				m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
				m_pdxgiSwapChain->SetFullscreenState(~bFullScreenState, NULL);

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
			WM_SIZE �޽����� �����찡 ������ �� �ѹ� ȣ��ǰų� �������� ũ�Ⱑ ����� �� ȣ��ȴ�.
			�� �������� ũ�� �� ����ڰ� ������ �� �����Ƿ� �������� ũ�Ⱑ ����Ǵ� ����
			������ ���� ��üȭ�� ����� ��ȯ�� �߻��� ���̴�.
		*/
		case WM_SIZE:
		{
			m_nWndClientWidth = LOWORD(lParam);
			m_nWndClientHeight = HIWORD(lParam);

			// �������� ũ�Ⱑ ����Ǹ� �ĸ������ ũ�⸦ �����Ѵ�.
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