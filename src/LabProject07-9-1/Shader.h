#pragma once
#include "Object.h"
#include "Camera.h"

class CShader
{
public:
	CShader();
	virtual ~CShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual ShaderBlob CreateVertexShader() = 0;
	virtual ShaderBlob CreatePixelShader() = 0;

	ShaderBlob CompileShaderFromFile(const Filepath& path, const char* name, const char* version, ID3DBlob** blob);
	ShaderBlob ReadCompiledShaderFromFile(const Filepath& path, ID3DBlob** blob);

	virtual void CreateShader(PtrDevice device, PtrGrpCommandList cmd_list, ID3D12RootSignature* signature);

	virtual void CreateShaderVariables(PtrDevice device, PtrGrpCommandList cmd_list);
	virtual void UpdateShaderVariables(PtrGrpCommandList cmd_list);
	virtual void UpdateShaderVariable(PtrGrpCommandList cmd_list, XMFLOAT4X4* pxmf4x4World);
	virtual void ReleaseShaderVariables();

	virtual void OnPrepareRender(PtrGrpCommandList cmd_list, int nPipelineState = 0);
	virtual void Render(PtrGrpCommandList cmd_list, GameCamera* pCamera, int nPipelineState = 0);

protected:
	ShaderBlob CreateEmptyShader() const;

	ID3DBlob* m_pd3dVertexShaderBlob = NULL;
	ID3DBlob* m_pd3dPixelShaderBlob = NULL;

	int m_nPipelineStates = 0;
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_d3dPipelineStateDesc;
};

class CIlluminatedShader : public CShader
{
public:
	CIlluminatedShader();
	virtual ~CIlluminatedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual ShaderBlob CreateVertexShader();
	virtual ShaderBlob CreatePixelShader();

	virtual void CreateShader(PtrDevice device, PtrGrpCommandList cmd_list, ID3D12RootSignature* signature);

	virtual void Render(PtrGrpCommandList cmd_list, GameCamera* pCamera, int nPipelineState = 0);
};

