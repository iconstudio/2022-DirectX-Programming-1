#pragma once
#include "Object.h"
#include "Camera.h"

class Pipeline
{
public:
	Pipeline();
	virtual ~Pipeline();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual ShaderBlob CreateVertexShader() = 0;
	virtual ShaderBlob CreatePixelShader() = 0;
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual P3DSignature CreateGraphicsRootSignature() = 0;

	ShaderBlob CompileShaderFromFile(const  WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	ShaderBlob ReadCompiledShaderFromFile(const WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob = NULL);

	virtual void CreateShader(P3DDevice device, P3DGrpCommandList cmdlist);

	virtual void PrepareRendering(P3DGrpCommandList cmdlist, int index = 0);

	P3DSignature GetRootSignature();
	const P3DSignature GetRootSignature() const;

	static Pipeline* diffusedShader;
	static Pipeline* illuminatedShader;

protected:
	P3DDevice dxDevice;
	P3DGrpCommandList dxCommandList;

	P3DSignature mySignature;

	ID3DBlob* m_pd3dVertexShaderBlob = NULL;
	ID3DBlob* m_pd3dPixelShaderBlob = NULL;

	int m_nPipelineStates = 0;
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC myState;
};

class IlluminatedGraphicsPipeline : public Pipeline
{
public:
	IlluminatedGraphicsPipeline();
	virtual ~IlluminatedGraphicsPipeline();
	
	ShaderBlob CreateVertexShader() override;
	ShaderBlob CreatePixelShader() override;
	D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	virtual P3DSignature CreateGraphicsRootSignature() override;
	
	void CreateShader(P3DDevice device, P3DGrpCommandList cmdlist) override;
};

