#include "pch.hpp"
#include "Shader.h"

Pipeline::Pipeline()
{}

Pipeline::~Pipeline()
{
	ReleaseUniforms();

	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++)
		{
			if (m_ppd3dPipelineStates[i])
			{
				m_ppd3dPipelineStates[i]->Release();
			}
		}
		delete[] m_ppd3dPipelineStates;
	}
}

D3DByteCode Pipeline::CreateVertexShader()
{
	D3DByteCode d3dShaderByteCode{};
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3DByteCode Pipeline::CreatePixelShader()
{
	D3DByteCode d3dShaderByteCode{};
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3DByteCode Pipeline::CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pd3dErrorBlob = NULL;
	auto valid = D3DCompileFromFile(pszFileName
		, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, pszShaderName, pszShaderProfile
		, nCompileFlags, 0
		, ppd3dShaderBlob
		, &pd3dErrorBlob);
	if (FAILED(valid))
	{
		throw "쉐이더 컴파일 실패!";
	}

	volatile char* pErrorString = nullptr;
	if (pd3dErrorBlob)
	{
		pErrorString = (char*)pd3dErrorBlob->GetBufferPointer();
	}

	D3DByteCode d3dShaderByteCode{};
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return d3dShaderByteCode;
}

#define _WITH_WFOPEN
//#define _WITH_STD_STREAM

#ifdef _WITH_STD_STREAM
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#endif

D3DByteCode Pipeline::ReadCompiledShaderFromFile(const WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN
	FILE* pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE* pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	nReadBytes = (UINT)::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
#endif
#ifdef _WITH_STD_STREAM
	std::ifstream ifsFile;
	ifsFile.open(pszFileName, std::ios::in | std::ios::ate | std::ios::binary);
	nReadBytes = (int)ifsFile.tellg();
	BYTE* pByteCode = new BYTE[*pnReadBytes];
	ifsFile.seekg(0);
	ifsFile.read((char*)pByteCode, nReadBytes);
	ifsFile.close();
#endif

	D3DByteCode d3dShaderByteCode;
	if (ppd3dShaderBlob)
	{
		*ppd3dShaderBlob = NULL;
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dShaderBlob);
		memcpy((*ppd3dShaderBlob)->GetBufferPointer(), pByteCode, nReadBytes);
		d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
		d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	}
	else
	{
		d3dShaderByteCode.BytecodeLength = nReadBytes;
		d3dShaderByteCode.pShaderBytecode = pByteCode;
	}

	return(d3dShaderByteCode);
}

D3D12_INPUT_LAYOUT_DESC Pipeline::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC Pipeline::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	//	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC Pipeline::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC Pipeline::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void Pipeline::CreateShader(P3DDevice device, P3DGrpCommandList cmd_list, P3DSignature signature)
{
	ZeroMemory(&pipeline_state_desc, sizeof(pipeline_state_desc));

	pipeline_state_desc.pRootSignature = signature;
	pipeline_state_desc.VS = CreateVertexShader();
	pipeline_state_desc.PS = CreatePixelShader();
	pipeline_state_desc.RasterizerState = CreateRasterizerState();
	pipeline_state_desc.BlendState = CreateBlendState();
	pipeline_state_desc.DepthStencilState = CreateDepthStencilState();
	pipeline_state_desc.InputLayout = CreateInputLayout();

	pipeline_state_desc.SampleMask = UINT_MAX;
	pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipeline_state_desc.NumRenderTargets = 1;
	pipeline_state_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipeline_state_desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipeline_state_desc.SampleDesc.Count = 1;
	pipeline_state_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	auto uuid = __uuidof(ID3D12PipelineState);
	auto place = reinterpret_cast<void**>(&m_ppd3dPipelineStates[0]);
	auto valid = device->CreateGraphicsPipelineState(&pipeline_state_desc, uuid, place);
	if (FAILED(valid))
	{
		throw "파이프라인 상태의 설정 실패!";
	}
}

void Pipeline::InitializeUniforms(P3DDevice device, P3DGrpCommandList cmd_list)
{}

void Pipeline::UpdateUniforms(P3DGrpCommandList cmd_list)
{}

void Pipeline::UpdateUniforms(P3DGrpCommandList cmd_list, XMFLOAT4X4* pxmf4x4World)
{}

void Pipeline::UpdateUniforms(P3DGrpCommandList cmd_list, CMaterialColors* pMaterialColors)
{}

void Pipeline::ReleaseUniforms()
{}

void Pipeline::OnPrepareRender(P3DGrpCommandList cmd_list, int nPipelineState)
{
	if (m_ppd3dPipelineStates)
	{
		cmd_list->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);
	}
}

void Pipeline::Render(P3DGrpCommandList cmd_list, GameCamera* pCamera, int nPipelineState)
{
	OnPrepareRender(cmd_list, nPipelineState);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CIlluminatedShader::CIlluminatedShader()
{}

CIlluminatedShader::~CIlluminatedShader()
{}

D3D12_INPUT_LAYOUT_DESC CIlluminatedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3DByteCode CIlluminatedShader::CreateVertexShader()
{
	return(Pipeline::CompileShaderFromFile(L"VertexShader.hlsl", "main", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3DByteCode CIlluminatedShader::CreatePixelShader()
{
	return(Pipeline::CompileShaderFromFile(L"PixelShader.hlsl", "main", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CIlluminatedShader::CreateShader(P3DDevice device, P3DGrpCommandList cmd_list, P3DSignature signature)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	Pipeline::CreateShader(device, cmd_list, signature);

	pipeline_state_desc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	HRESULT hResult = device->CreateGraphicsPipelineState(&pipeline_state_desc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[1]);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (pipeline_state_desc.InputLayout.pInputElementDescs) delete[] pipeline_state_desc.InputLayout.pInputElementDescs;
}

void CIlluminatedShader::Render(P3DGrpCommandList cmd_list, GameCamera* pCamera, int nPipelineState)
{
	OnPrepareRender(cmd_list, nPipelineState);
}
