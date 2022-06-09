#include "pch.hpp"
#include "GraphicsPipeline.hpp"

GraphicsPipeline::GraphicsPipeline(P3DDevice device, P3DGrpCommandList cmdlist)
{}

GraphicsPipeline::~GraphicsPipeline()
{}

void GraphicsPipeline::Awake()
{
	BuildShaders();
	BuildRootSignature();
	BuildPipeline();
}

P3DSignature GraphicsPipeline::GetRootSignature()
{
	return mySignature;
}

P3DSignature const GraphicsPipeline::GetRootSignature() const
{
	return mySignature;
}
