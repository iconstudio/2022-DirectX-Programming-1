#pragma once
#include "Timer.hpp"

class Framework
{
public:
	Framework(unsigned int width, unsigned int height, UINT aa_level = 0);
	~Framework();

	void Awake(HINSTANCE hinstance, HWND window);
	void CreateDirect3DDevice();
	void CreateCommanders();
	void CreateSwapChain();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void Start();
	void Update(float time_elpased);
	void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();
	void Render();

	void ToggleFullscreen();

	void OnMessage(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void OnMouse(UINT msg, WPARAM btn, LPARAM state);
	void OnKeyboard(UINT msg, WPARAM key, LPARAM state);
	LRESULT CALLBACK OnHWND(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	bool TryPause();
	void Resume();

	inline bool D3DAssert(HRESULT valid, const char* error);

	HRESULT ResetCmdList(ID3D12PipelineState* pipeline = nullptr);
	HRESULT CloseCmdList();
	void ExecuteCmdList(ID3D12CommandList* list[], UINT count);

	HRESULT SetFenceEvent(HANDLE signal, UINT64 limit);
	HRESULT SignalToFence(UINT64 count);

	HINSTANCE myHInstance;
	HWND myWindow;

	const unsigned int frameWidth;
	const unsigned int frameHeight;

	bool isAntiAliasingEnabled;
	UINT levelAntiAliasing;
	UINT indexFrameBuffer;

	IDXGIFactory4* myFactory;
	ID3D12Device* myDevice;

	ID3D12Fence* myRenderFence;
	UINT64 myFences[numberFrameBuffers];
	HANDLE eventFence;

	ID3D12GraphicsCommandList* myCommandList;
	ID3D12CommandQueue* myCommandQueue;
	ID3D12CommandAllocator* myCommandAlloc;

	IDXGISwapChain3* mySwapChain;
	ID3D12Resource* resSwapChainBackBuffers[numberFrameBuffers];

	ID3D12DescriptorHeap* heapRtvDesc;
	UINT szRtvDescIncrements;

	ID3D12Resource* myDepthStencilBuffer;
	ID3D12DescriptorHeap* heapDsvDesc;
	UINT szDsvDescIncrements;

#if defined(_DEBUG)
	ID3D12Debug* myDebugController = nullptr;
#endif

	Timer myGameTimer;

	// 월드의 월드 변환 행렬
	XMFLOAT4X4 globalMatrix;

	std::vector<shared_ptr<Scene>> myScenes;
	std::vector<shared_ptr<Scene>> myScenePipeline;
	shared_ptr<Camera> myCamera;

	std::unordered_map<const char*, shared_ptr<Mesh>> myMeshes;

	_TCHAR m_pszFrameRate[70];
};
