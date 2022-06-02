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

	void WaitForGpuComplete();
	void PrepareRendering();
	void Render();

	void ToggleFullscreen();

	void RegisterPipeline(const GraphicsPipeline& pipeline);
	void RegisterPipeline(GraphicsPipeline&& pipeline);
	void SetPipeline(const int index);

	const GraphicsPipeline& GetPipeline(const int index) const;
	GraphicsPipeline& GetPipeline(const int index);

	GraphicsPipeline CreateEmptyPipeline() const;
	Shader CreateEmptyShader(const char* version) const;
	constexpr D3D12_INPUT_LAYOUT_DESC CreateEmptyInputLayout() const;
	constexpr D3D12_RASTERIZER_DESC CreateEmptyRasterizerState() const;
	constexpr D3D12_BLEND_DESC CreateEmptyBlendState() const;
	constexpr D3D12_DEPTH_STENCIL_DESC CreateEmptyDepthStencilState() const;

private:
	void CreateDirect3DDevice();
	void CreateCommanders();
	void CreateSwapChain();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	HWND myWindow;
	const long frameWidth, frameHeight;

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
