#pragma once
#include "PlayableObjet.hpp"

class GraphicsCore : PlayableObjet
{
public:
	GraphicsCore(long width, long height);

	void Awake() override;
	void Start() override;
	void Reset() override;
	void Update(float delta_time) override;
	void PrepareRendering();
	void Render();

	void RegisterPipeline(const GraphicsPipeline& pipeline);
	void RegisterPipeline(GraphicsPipeline&& pipeline);

	shared_ptr<GraphicsPipeline> GetPipeline(const int index) const;

private:
	const long frameWidth, frameHeight;

	IDXGIFactory4* myFactory;
	IDXGISwapChain3* mySwapChain;
	P3DDevice myDevice;

	P3DGrpCommandList myCommandList;
	ID3D12CommandQueue* myCommandQueue;
	ID3D12CommandAllocator* myCommandAlloc;
	P3DSignature myRootSignature;

	static const UINT numberFrameBuffers = 2;
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
};
