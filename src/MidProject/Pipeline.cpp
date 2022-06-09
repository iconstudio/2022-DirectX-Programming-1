#include "pch.hpp"
#include "Pipeline.hpp"

Pipeline* Pipeline::diffusedShader = nullptr;
Pipeline* Pipeline::illuminatedShader = nullptr;

Pipeline::Pipeline()
	: dxDevice(nullptr), dxCommandList(nullptr)
	, mySignature(nullptr)
	, myState()
{}

Pipeline::~Pipeline()
{
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

	if (mySignature)
	{
		mySignature->Release();
		mySignature = nullptr;
	}
}

ShaderBlob Pipeline::CreateVertexShader()
{
	ShaderBlob d3dShaderByteCode{};
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

ShaderBlob Pipeline::CreatePixelShader()
{
	ShaderBlob d3dShaderByteCode{};
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

ShaderBlob Pipeline::CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
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

	ShaderBlob d3dShaderByteCode{};
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

ShaderBlob Pipeline::ReadCompiledShaderFromFile(const WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN

	FILE* pFile = NULL;
	_wfopen_s(&pFile, pszFileName, L"rb");
	if (NULL == pFile)
	{
		throw "쉐이더 파일을 찾을 수 없음!";
	}

	fseek(pFile, 0, SEEK_END);
	int nFileSize = ftell(pFile);
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

	ShaderBlob d3dShaderByteCode;
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

P3DSignature Pipeline::CreateGraphicsRootSignature()
{
	return nullptr;
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

void Pipeline::CreateShader(P3DDevice device, P3DGrpCommandList cmdlist)
{
	dxDevice = device;
	dxCommandList = cmdlist;

	ZeroMemory(&myState, sizeof(myState));

	mySignature = CreateGraphicsRootSignature();
	myState.pRootSignature = mySignature;
	myState.VS = CreateVertexShader();
	myState.PS = CreatePixelShader();
	myState.RasterizerState = CreateRasterizerState();
	myState.BlendState = CreateBlendState();
	myState.DepthStencilState = CreateDepthStencilState();
	myState.InputLayout = CreateInputLayout();

	myState.SampleMask = UINT_MAX;
	myState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	myState.NumRenderTargets = 1;
	myState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	myState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	myState.SampleDesc.Count = 1;
	myState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	auto uuid = __uuidof(ID3D12PipelineState);
	auto place = reinterpret_cast<void**>(&m_ppd3dPipelineStates[0]);
	auto valid = device->CreateGraphicsPipelineState(&myState, uuid, place);
	if (FAILED(valid))
	{
		throw "파이프라인 상태의 설정 실패!";
	}
}

void Pipeline::PrepareRendering(P3DGrpCommandList cmdlist, int nPipelineState)
{
	if (m_ppd3dPipelineStates)
	{
		cmdlist->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);
	}
}

P3DSignature Pipeline::GetRootSignature()
{
	return mySignature;
}

const P3DSignature Pipeline::GetRootSignature() const
{
	return mySignature;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
IlluminatedGraphicsPipeline::IlluminatedGraphicsPipeline()
	: Pipeline()
{}

IlluminatedGraphicsPipeline::~IlluminatedGraphicsPipeline()
{}

D3D12_INPUT_LAYOUT_DESC IlluminatedGraphicsPipeline::CreateInputLayout()
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

ShaderBlob IlluminatedGraphicsPipeline::CreateVertexShader()
{
	return(Pipeline::CompileShaderFromFile(L"VertexShader.hlsl", "main", "vs_5_1", &m_pd3dVertexShaderBlob));
}

ShaderBlob IlluminatedGraphicsPipeline::CreatePixelShader()
{
	return(Pipeline::CompileShaderFromFile(L"PixelShader.hlsl", "main", "ps_5_1", &m_pd3dPixelShaderBlob));
}

P3DSignature IlluminatedGraphicsPipeline::CreateGraphicsRootSignature()
{
	P3DSignature signature = nullptr;

	D3D12_ROOT_PARAMETER shader_params[3]{};
	ZeroMemory(&shader_params, sizeof(shader_params));

	shader_params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	shader_params[0].Descriptor.ShaderRegister = 0; // Camera
	shader_params[0].Descriptor.RegisterSpace = 0;
	shader_params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	shader_params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	shader_params[1].Constants.Num32BitValues = 32;
	shader_params[1].Constants.ShaderRegister = 1; // GameObject
	shader_params[1].Constants.RegisterSpace = 0;
	shader_params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	shader_params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	shader_params[2].Descriptor.ShaderRegister = 0; // Lights
	shader_params[2].Descriptor.RegisterSpace = 1;
	shader_params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// 정점 쉐이더, 픽셀 쉐이더, 입력 조립기, 출력 병합기 
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC signature_desc{};

	signature_desc.NumParameters = std::size(shader_params);
	signature_desc.pParameters = shader_params;
	signature_desc.NumStaticSamplers = 0; // 텍스처
	signature_desc.pStaticSamplers = NULL;
	signature_desc.Flags = flags;

	ID3DBlob* signature_blob = nullptr;
	ID3DBlob* error_blob = nullptr;

	auto valid = D3D12SerializeRootSignature(&signature_desc
		, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob
		, &error_blob);
	if (FAILED(valid))
	{
		throw "루트 서명의 메모리를 할당하지 못함!";
	}

	UINT gpu_mask = 0;
	auto uuid = __uuidof(ID3D12RootSignature);
	auto place = reinterpret_cast<void**>(&signature);
	valid = dxDevice->CreateRootSignature(gpu_mask
		, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize()
		, uuid, place);
	if (FAILED(valid))
	{
		throw "루트 서명을 셍성하지 못함!";
	}

	if (signature_blob)
	{
		signature_blob->Release();
	}

	if (error_blob)
	{
		error_blob->Release();
	}

	return signature;
}

void IlluminatedGraphicsPipeline::CreateShader(P3DDevice device, P3DGrpCommandList cmdlist)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	Pipeline::CreateShader(device, cmdlist);

	myState.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	HRESULT hResult = device->CreateGraphicsPipelineState(&myState
		, __uuidof(ID3D12PipelineState)
		, (void**)&m_ppd3dPipelineStates[1]);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (myState.InputLayout.pInputElementDescs)
	{
		delete[] myState.InputLayout.pInputElementDescs;
	}
}
