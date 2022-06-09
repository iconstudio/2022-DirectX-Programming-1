#pragma once

class GraphicsPipeline
{
public:
	GraphicsPipeline(P3DDevice device, P3DGrpCommandList cmdlist);
	virtual ~GraphicsPipeline();

	virtual void Awake();
	virtual void BuildShaders() = 0;
	virtual void BuildRootSignature() = 0;
	virtual void BuildPipeline() = 0;
	virtual void PrepareRendering() = 0;
	virtual void Render() = 0;

	P3DSignature GetRootSignature();
	P3DSignature const GetRootSignature() const;

protected:
	virtual ShaderBlob CreateVertexShader() = 0;
	virtual ShaderBlob CreatePixelShader() = 0;
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() = 0;
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState() = 0;
	virtual D3D12_BLEND_DESC CreateBlendState() = 0;
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState() = 0;

	P3DDevice dxDevice;
	P3DGrpCommandList dxTaskList;

	P3DSignature mySignature;
};

