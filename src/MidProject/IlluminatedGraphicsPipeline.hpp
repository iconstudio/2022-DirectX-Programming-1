#pragma once
#include "Pipeline.hpp"

class IlluminatedGraphicsPipeline : public Pipeline
{
public:
	IlluminatedGraphicsPipeline();
	virtual ~IlluminatedGraphicsPipeline();

	void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;

	ShaderBlob CreateVertexShader() override;
	ShaderBlob CreatePixelShader() override;
	D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	P3DSignature CreateGraphicsRootSignature() override;
};
