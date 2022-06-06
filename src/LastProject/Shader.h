#pragma once
#include "GameObject.hpp"
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

	virtual D3DByteCode CreateVertexShader();
	virtual D3DByteCode CreatePixelShader();

	D3DByteCode CompileShaderFromFile(const  WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	D3DByteCode ReadCompiledShaderFromFile(const WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob = NULL);

	virtual void CreateShader(P3DDevice device, P3DGrpCommandList cmdlist, P3DSignature signature);

	virtual void InitializeUniforms(P3DDevice device, P3DGrpCommandList cmdlist);
	virtual void UpdateUniforms(P3DGrpCommandList cmdlist);
	virtual void ReleaseUniforms();

	virtual void UpdateUniforms(P3DGrpCommandList cmdlist, XMFLOAT4X4* pxmf4x4World);
	virtual void UpdateUniforms(P3DGrpCommandList cmdlist, CMaterialColors* pMaterialColors);

	virtual void PrepareRendering(P3DGrpCommandList cmdlist, int index);

protected:
	ID3DBlob* m_pd3dVertexShaderBlob = NULL;
	ID3DBlob* m_pd3dPixelShaderBlob = NULL;

	int m_nPipelineStates = 0;
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
	virtual D3DByteCode CreateVertexShader();
	virtual D3DByteCode CreatePixelShader();

	virtual void CreateShader(P3DDevice device, P3DGrpCommandList cmdlist, P3DSignature signature);

	virtual void PrepareRendering(P3DGrpCommandList cmdlist, int index);
};

