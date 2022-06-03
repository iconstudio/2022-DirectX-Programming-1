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

	ShaderBlob CompileShaderFromFile(const  WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	ShaderBlob ReadCompiledShaderFromFile(const WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob = NULL);

	virtual void CreateShader(P3DDevice device, P3DGrpCommandList cmd_list, P3DSignature signature);

	virtual void InitializeUniforms(P3DDevice device, P3DGrpCommandList cmd_list);
	virtual void UpdateUniforms(P3DGrpCommandList cmd_list);
	virtual void ReleaseUniforms();

	virtual void UpdateUniforms(P3DGrpCommandList cmd_list, XMFLOAT4X4* pxmf4x4World);
	virtual void UpdateUniforms(P3DGrpCommandList cmd_list, CMaterialColors* pMaterialColors);

	virtual void OnPrepareRender(P3DGrpCommandList cmd_list, int nPipelineState = 0);
	virtual void Render(P3DGrpCommandList cmd_list, GameCamera* pCamera, int nPipelineState = 0);

protected:
	ID3DBlob* m_pd3dVertexShaderBlob = NULL;
	ID3DBlob* m_pd3dPixelShaderBlob = NULL;

	int									m_nPipelineStates = 0;
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC	pipeline_state_desc;
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

	virtual void CreateShader(P3DDevice device, P3DGrpCommandList cmd_list, P3DSignature signature);

	virtual void Render(P3DGrpCommandList cmd_list, GameCamera* pCamera, int nPipelineState = 0);
};

