#include "pch.hpp"
#include "GraphicsCore.hpp"

GraphicsCore::GraphicsCore(long width, long height)
	: frameWidth(width), frameHeight(height)
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
#endif
{}

void GraphicsCore::Awake()
{}

void GraphicsCore::Start()
{}

void GraphicsCore::Reset()
{}

void GraphicsCore::Update(float delta_time)
{}

void GraphicsCore::PrepareRendering()
{}

void GraphicsCore::Render()
{}

void GraphicsCore::RegisterPipeline(const GraphicsPipeline& pipeline)
{
	myPipelines.emplace_back(pipeline);
}

void GraphicsCore::RegisterPipeline(GraphicsPipeline&& pipeline)
{
	myPipelines.emplace_back(std::forward<GraphicsPipeline>(pipeline));
}

shared_ptr<GraphicsPipeline> GraphicsCore::GetPipeline(const int index) const
{
	return myPipelines.at(index);
}
