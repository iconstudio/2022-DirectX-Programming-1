#include "pch.hpp"
#include "stdafx.h"
#include "Pipeline.hpp"
#include "Shader.h"

Pipeline::Pipeline()
	: myPipelineStates()
{
	myPipelineStates.reserve(2);
}

Pipeline::Pipeline(const PipelineDesc& desc)
	: Pipeline(std::move(PipelineDesc(desc)))
{}

Pipeline::Pipeline(PipelineDesc&& desc)
	: m_d3dPipelineStateDesc(std::forward<PipelineDesc>(desc))
{
	myPipelineStates.reserve(2);
}

Pipeline::~Pipeline()
{
	if (myRootSignature)
	{
		myRootSignature->Release();
	}

	if (0 < myPipelineStates.size())
	{
		for (auto& state : myPipelineStates)
		{
			state->Release();
		}
	}

}

void Pipeline::Awake(PtrDevice device, PtrGrpCommandList cmd_list)
{
	d3dDevice = device;
	d3dTaskList = cmd_list;
	
	myRootSignature = CreateGraphicsRootSignature();
}

void Pipeline::Attach(const D3D12_INPUT_LAYOUT_DESC& desc)
{
	myShaderAttributes = desc;
}

void Pipeline::Attach(const D3D12_RASTERIZER_DESC& desc)
{
	myRasterizerOptions = desc;
}

void Pipeline::Attach(const D3D12_BLEND_DESC& desc)
{
	myBlendingOptions = desc;
}

void Pipeline::Attach(const D3D12_DEPTH_STENCIL_DESC& desc)
{
	myDepthStencilOptions = desc;
}

void Pipeline::Start(const shared_ptr<Shader>& vs, const shared_ptr<Shader>& ps)
{
	Start(shared_ptr<Shader>(vs), shared_ptr<Shader>(ps));
}

void Pipeline::Start(shared_ptr<Shader>&& vs, shared_ptr<Shader>&& ps)
{
	ZeroMemory(&m_d3dPipelineStateDesc, sizeof(m_d3dPipelineStateDesc));

	m_d3dPipelineStateDesc.VS = std::forward<ShaderHandle>(vs->myHandle);
	m_d3dPipelineStateDesc.PS = std::forward<ShaderHandle>(ps->myHandle);

	m_d3dPipelineStateDesc.pRootSignature = myRootSignature;
	m_d3dPipelineStateDesc.InputLayout = myShaderAttributes;
	m_d3dPipelineStateDesc.RasterizerState = myRasterizerOptions;
	m_d3dPipelineStateDesc.BlendState = myBlendingOptions;
	m_d3dPipelineStateDesc.DepthStencilState = myDepthStencilOptions;
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// 광원이 없는 파이프라인
	CreateStateForNow();
}

void Pipeline::PrepareRendering(PtrGrpCommandList pd3dCommandList, int pipeline_index)
{
	if (pipeline_index <= myPipelineStates.size())
	{
		pd3dCommandList->SetPipelineState(myPipelineStates.at(pipeline_index).get());
	}
	else
	{
		throw "잘못된 파이프라인 상태를 참조함!";
	}
}

shared_ptr<Shader> Pipeline::CreateShader(const Filepath& path
	, const char* version
	, const char* entry)
{
	const auto shader_path = path.c_str();

	UINT flags = 0;
#if defined(_DEBUG)
	flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* shader_blob = nullptr;
	ID3DBlob* error_blob = nullptr;
	HRESULT valid = D3DCompileFromFile(shader_path, nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, entry, version, flags, 0
		, &shader_blob
		, &error_blob);
	if (FAILED(valid))
	{
		throw "Pipeline: 쉐이더 컴파일 오류!";
	}

	if (error_blob)
	{
		volatile char* error_info = nullptr;
		error_info = reinterpret_cast<char*>(error_blob->GetBufferPointer());

		throw error_info;
	}

	auto& blob = *shader_blob;
	auto handle = CreateEmptyShaderHandle();
	handle.pShaderBytecode = blob.GetBufferPointer();
	handle.BytecodeLength = blob.GetBufferSize();

	auto ptr = make_shared<Shader>(version, entry);
	ptr->myHandle = std::move(handle);
	ptr->myBlob = unique_ptr<ID3DBlob>(std::move(shader_blob));

	return ptr;
}

shared_ptr<Shader> Pipeline::LoadShader(const Filepath& path)
{
	std::ifstream ifsFile;
	ifsFile.open(path, std::ios::in | std::ios::ate | std::ios::binary);

	const auto shader_size = ifsFile.tellg();
	auto shader_code = new BYTE[static_cast<size_t>(shader_size)];

	ifsFile.seekg(0);
	ifsFile.read(reinterpret_cast<char*>(shader_code), shader_size);
	ifsFile.close();

	auto handle = CreateEmptyShaderHandle();
	handle.pShaderBytecode = shader_code;
	handle.BytecodeLength = shader_size;

	return make_shared<Shader>(std::move(handle));
}

void Pipeline::Attach(const shared_ptr<Shader>& ptr)
{
	myShaders.push_back(ptr);
}

void Pipeline::Attach(shared_ptr<Shader>&& ptr)
{
	myShaders.push_back(std::forward<shared_ptr<Shader>>(ptr));
}

void Pipeline::CreateStateForNow()
{
	ID3D12PipelineState* ptr = nullptr;

	auto uuid = __uuidof(ID3D12PipelineState);
	auto place = reinterpret_cast<void**>(&ptr);
	auto valid = d3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, uuid, place);
	if (FAILED(valid))
	{
		throw "파이프라인 상태 객체의 생성 실패! (1)";
	}

	if (!ptr)
	{
		throw "파이프라인 상태 객체의 생성 실패! (2)";
	}

	myPipelineStates.emplace_back(ptr);
}

D3D12_INPUT_LAYOUT_DESC Pipeline::CreateEmptyInputLayout() const
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc{};
	ZeroMemory(&d3dInputLayoutDesc, sizeof(d3dInputLayoutDesc));

	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC Pipeline::CreateEmptyRasterizerState() const
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc{};
	ZeroMemory(&d3dRasterizerDesc, sizeof(d3dRasterizerDesc));

	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;

	return d3dRasterizerDesc;
}

D3D12_BLEND_DESC Pipeline::CreateEmptyBlendState() const
{
	D3D12_BLEND_DESC d3dBlendDesc{};
	ZeroMemory(&d3dBlendDesc, sizeof(d3dBlendDesc));

	return d3dBlendDesc;
}

D3D12_DEPTH_STENCIL_DESC Pipeline::CreateEmptyDepthStencilState() const
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc{};
	ZeroMemory(&d3dDepthStencilDesc, sizeof(d3dDepthStencilDesc));

	return d3dDepthStencilDesc;
}

shared_ptr<Shader> Pipeline::GetShader(const UINT index) const
{
	if (0 < myShaders.size())
	{
		return myShaders.at(index);
	}
	else
	{
		return nullptr;
	}
}

PtrSignature Pipeline::GetGraphicsRootSignature()
{
	return myRootSignature;
}

ID3D12RootSignature const* Pipeline::GetGraphicsRootSignature() const
{
	return myRootSignature;
}
