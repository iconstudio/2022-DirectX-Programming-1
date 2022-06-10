#include "pch.hpp"
#include "DiffusedGraphicsPipeline.hpp"

DiffusedGraphicsPipeline::DiffusedGraphicsPipeline()
	: Pipeline()
{}

DiffusedGraphicsPipeline::~DiffusedGraphicsPipeline()
{}

void DiffusedGraphicsPipeline::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	AssignVertexShader(Shader("DiffusedVertexShader.hlsl.", "main", "vs_5_1"));
	AssignPixelShader(Shader("DiffusedPixelShader.hlsl.", "main", "ps_5_1"));

	Pipeline::Awake(device, cmdlist);
}

ShaderBlob DiffusedGraphicsPipeline::CreateVertexShader()
{
	return myVertexShader.myCode;
}

ShaderBlob DiffusedGraphicsPipeline::CreatePixelShader()
{
	return myPixelShader.myCode;
}

D3D12_INPUT_LAYOUT_DESC DiffusedGraphicsPipeline::CreateInputLayout()
{
	return D3D12_INPUT_LAYOUT_DESC();
}

P3DSignature DiffusedGraphicsPipeline::CreateGraphicsRootSignature()
{
	return P3DSignature();
}
