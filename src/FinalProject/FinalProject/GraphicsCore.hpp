#pragma once
#include "PlayableObjet.hpp"

class GraphicsCore : PlayableObjet
{
public:
	GraphicsCore(long width, long height);
	~GraphicsCore();

	GraphicsCore& SetHWND(HWND window);

	void Awake() override;
	void Start() override;
	void Reset() override;
	void Update(float delta_time) override;
	void Release();

	void WaitForGpuComplete();
	void PrepareRendering();
	void Render();

	void ToggleFullscreen();

	void RegisterPipeline(const GraphicsPipeline& pipeline);
	void RegisterPipeline(GraphicsPipeline&& pipeline);

	void SetPipeline(const int index);
	const GraphicsPipeline& GetPipeline(const int index) const;
	GraphicsPipeline& GetPipeline(const int index);
	P3DGrpCommandList GetCommandList();

	GraphicsPipeline CreateEmptyPipeline() const;
	Shader CreateEmptyShader(const char* version) const;
	D3D12_INPUT_LAYOUT_DESC CreateEmptyInputLayout() const;
	D3D12_RASTERIZER_DESC CreateEmptyRasterizerState() const;
	D3D12_BLEND_DESC CreateEmptyBlendState() const;
	D3D12_DEPTH_STENCIL_DESC CreateEmptyDepthStencilState() const;

private:
	void CreateDirect3DDevice();
	void CreateCommanders();
	void CreateSwapChain();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	bool D3DAssert(HRESULT valid, const char* error);

	//
	void ResetCmdAllocator();
	void ResetCmdList(ID3D12PipelineState* pipeline = nullptr);
	void CloseCmdList();
	void ExecuteCmdList(P3DCommandList list[], size_t count);

	//
	D3DHandle& AddtoDescriptor(D3DHandle& handle, const size_t increment);
	D3DHandle GetRTVHandle() const;
	D3DHandle GetDSVHandle() const;
	void ClearRenderTargetView(D3DHandle& handle, D3D12_RECT* erase_rects = nullptr, size_t erase_count = 0);
	void ClearDepthStencilView(D3DHandle& handle, float depth = 1.0f, UINT8 stencil = 0, D3D12_RECT* erase_rects = nullptr, size_t erase_count = 0);
	void ReadyOutputMerger(D3DHandle& rtv, D3DHandle& dsv);

	void SetFenceEvent(HANDLE signal, UINT64 limit);
	void SignalToFence(UINT64 count);

	const UINT barrierRender = 0;
	const UINT barrierSwap = 1;
	void SetBarrier(UINT type);

	HWND myWindow;
	const long frameWidth, frameHeight;
	float frameColor[4];

	IDXGIFactory4* myFactory;
	IDXGISwapChain3* mySwapChain;
	P3DDevice myDevice;

	P3DGrpCommandList myCommandList;
	ID3D12CommandQueue* myCommandQueue;
	ID3D12CommandAllocator* myCommandAlloc;
	P3DSignature myRootSignature;

	static const UINT numberFrameBuffers = 2;
	bool isAntiAliasingEnabled;
	UINT levelAntiAliasing;
	UINT indexFrameBuffer;

	ID3D12Resource* resSwapChainBackBuffers[numberFrameBuffers];
	D3D12_RESOURCE_BARRIER myBarriers[numberFrameBuffers];
	ID3D12DescriptorHeap* heapRtvDesc;
	UINT szRtvDescIncrements;

	ID3D12Resource* myDepthStencilBuffer;
	ID3D12DescriptorHeap* heapDsvDesc;
	UINT szDsvDescIncrements;

	ID3D12Fence* myRenderFence;
	UINT64 myFences[numberFrameBuffers];
	HANDLE eventFence;

#if defined(_DEBUG)
	ID3D12Debug* myDebugController;
#endif

	std::vector<shared_ptr<GraphicsPipeline>> myPipelines;
	shared_ptr<GraphicsPipeline> currentPipeline;
};
