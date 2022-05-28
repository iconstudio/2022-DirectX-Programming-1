#pragma once
#include "Object.h"
#include "Camera.h"

class Pipeline
{
public:
	Pipeline();
	virtual ~Pipeline();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual ShaderBlob CreateVertexShader();
	virtual ShaderBlob CreatePixelShader();

	ShaderBlob CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);
	ShaderBlob ReadCompiledShaderFromFile(WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob=NULL);

	virtual void CreateShader(ID3D12Device *device, ID3D12GraphicsCommandList *cmd_list, ID3D12RootSignature *signature);

	virtual void CreateShaderVariables(ID3D12Device *device, ID3D12GraphicsCommandList *cmd_list);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *cmd_list);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *cmd_list, XMFLOAT4X4 *pxmf4x4World);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *cmd_list, CMaterialColors *pMaterialColors);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *cmd_list, int nPipelineState=0);
	virtual void Render(ID3D12GraphicsCommandList *cmd_list, GameCamera *pCamera, int nPipelineState=0);

protected:
	ID3DBlob							*m_pd3dVertexShaderBlob = NULL;
	ID3DBlob							*m_pd3dPixelShaderBlob = NULL;

	int									m_nPipelineStates = 0;
	ID3D12PipelineState					**m_ppd3dPipelineStates = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_d3dPipelineStateDesc;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CIlluminatedShader : public Pipeline
{
public:
	CIlluminatedShader();
	virtual ~CIlluminatedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual ShaderBlob CreateVertexShader();
	virtual ShaderBlob CreatePixelShader();

	virtual void CreateShader(ID3D12Device *device, ID3D12GraphicsCommandList *cmd_list, ID3D12RootSignature *signature);

	virtual void Render(ID3D12GraphicsCommandList *cmd_list, GameCamera *pCamera, int nPipelineState = 0);
};

