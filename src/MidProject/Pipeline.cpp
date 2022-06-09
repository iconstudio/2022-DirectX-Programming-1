#include "pch.hpp"
#include "Pipeline.hpp"

Pipeline* Pipeline::diffusedShader = nullptr;
Pipeline* Pipeline::illuminatedShader = nullptr;

Pipeline::Pipeline()
	: dxDevice(nullptr), dxCommandList(nullptr)
	, myStateDescription()
	, mySignature(nullptr), myVertexShaderBlob(nullptr), myPixelShaderBlob(nullptr)
	, myDerivedStates()
{}

Pipeline::~Pipeline()
{
	if (0 < myDerivedStates.size())
	{
		std::for_each(myDerivedStates.begin(), myDerivedStates.end()
			, [](ID3D12PipelineState* state) {
				state->Release();
			});
	}

	if (myVertexShaderBlob) myVertexShaderBlob->Release();
	if (myPixelShaderBlob) myPixelShaderBlob->Release();

	if (myStateDescription.InputLayout.pInputElementDescs)
	{
		delete[] myStateDescription.InputLayout.pInputElementDescs;
	}

	if (mySignature)
	{
		mySignature->Release();
		mySignature = nullptr;
	}
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

void Pipeline::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	dxDevice = device;
	dxCommandList = cmdlist;

	BuildShaders();
	BuildRootSignature();
	BuildInputLayout();
	BuildRasterizerState();
	BuildBlendState();
	BuildDepthStencilState();

	BuildState(0);
}

void Pipeline::BuildShaders()
{
	myStateDescription.VS = CreateVertexShader();
	myStateDescription.PS = CreatePixelShader();
}

void Pipeline::BuildRootSignature()
{
	if (mySignature)
	{
		mySignature->Release();
	}

	mySignature = CreateGraphicsRootSignature();
	myStateDescription.pRootSignature = mySignature;
}

void Pipeline::BuildInputLayout()
{
	myStateDescription.InputLayout = CreateInputLayout();
}

void Pipeline::BuildRasterizerState()
{
	myStateDescription.RasterizerState = CreateRasterizerState();
}

void Pipeline::BuildBlendState()
{
	myStateDescription.BlendState = CreateBlendState();
}

void Pipeline::BuildDepthStencilState()
{
	myStateDescription.DepthStencilState = CreateDepthStencilState();
}

void Pipeline::BuildState(int index)
{
	myStateDescription.SampleMask = UINT_MAX;
	myStateDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	myStateDescription.NumRenderTargets = 1;
	myStateDescription.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	myStateDescription.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	myStateDescription.SampleDesc.Count = 1;
	myStateDescription.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	ID3D12PipelineState* handle = nullptr;

	auto uuid = __uuidof(ID3D12PipelineState);
	auto place = reinterpret_cast<void**>(&handle);
	auto valid = dxDevice->CreateGraphicsPipelineState(&myStateDescription, uuid, place);
	if (FAILED(valid))
	{
		throw "파이프라인 상태의 설정 실패!";
	}

	if (myDerivedStates.size() <= index)
	{
		myDerivedStates.push_back(handle);
	}
	else
	{
		auto& it = myDerivedStates.at(index);
		if (it)
		{
			it->Release();
		}

		it = handle;
	}
}

void Pipeline::PrepareRendering(P3DGrpCommandList cmdlist, int index)
{
	if (index <= myDerivedStates.size())
	{
		cmdlist->SetPipelineState(myDerivedStates.at(index));
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
