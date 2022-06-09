#pragma once
#include "Object.h"
#include "Camera.h"

class Pipeline
{
public:
	Pipeline();
	virtual ~Pipeline();

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist);
	virtual void BuildState(int index);
	virtual void PrepareRendering(P3DGrpCommandList cmdlist, int index = 0);

	ShaderBlob CompileShaderFromFile(const  WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	ShaderBlob ReadCompiledShaderFromFile(const WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob = NULL);

	P3DSignature GetRootSignature();
	const P3DSignature GetRootSignature() const;

	static Pipeline* diffusedShader;
	static Pipeline* illuminatedShader;

protected:
	virtual void BuildShaders();
	virtual void BuildRootSignature();
	virtual void BuildInputLayout();
	virtual void BuildRasterizerState();
	virtual void BuildBlendState();
	virtual void BuildDepthStencilState();

	virtual ShaderBlob CreateVertexShader() = 0;
	virtual ShaderBlob CreatePixelShader() = 0;
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual P3DSignature CreateGraphicsRootSignature() = 0;

	P3DDevice dxDevice;
	P3DGrpCommandList dxCommandList;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC myStateDescription;
	P3DSignature mySignature;
	ID3DBlob* myVertexShaderBlob;
	ID3DBlob* myPixelShaderBlob;

	std::vector<ID3D12PipelineState*> myDerivedStates;

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
};
