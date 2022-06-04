#include "pch.hpp"
#include "GraphicsPipeline.hpp"
#include "Shader.hpp"

GraphicsPipeline::GraphicsPipeline(P3DDevice device, P3DGrpCommandList cmdlist)
	: dxDevice(device), dxTaskList(cmdlist), mySignature(nullptr)
	, myDescription(), myState(nullptr), isModified(false)
	, myVertexShader(nullptr), myPixelShader(nullptr)
{
	myShaderUniforms.reserve(5);

	myDescription.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	myDescription.NodeMask = 0;
	myDescription.SampleMask = UINT_MAX;
	myDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	myDescription.NumRenderTargets = 1;
	myDescription.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	myDescription.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	myDescription.SampleDesc.Count = 1;
}

GraphicsPipeline::~GraphicsPipeline()
{}

void GraphicsPipeline::Awake()
{
	CreateUniforms();
	CreateRootSignature();
	CreatePipelineState();
}

void GraphicsPipeline::CreateUniforms()
{
	// Camera
	D3D12_ROOT_PARAMETER param0{};
	param0.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	param0.Descriptor.ShaderRegister = 1;
	param0.Descriptor.RegisterSpace = 0;
	param0.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	Attach(param0);

	// GameObject
	D3D12_ROOT_PARAMETER param1{};
	param1.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	param1.Constants.Num32BitValues = 32;
	param1.Constants.ShaderRegister = 2;
	param1.Constants.RegisterSpace = 0;
	param1.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	Attach(param1);

	// Lights
	D3D12_ROOT_PARAMETER param2{};
	param2.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	param2.Descriptor.ShaderRegister = 4;
	param2.Descriptor.RegisterSpace = 0;
	param2.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	Attach(param2);
}

void GraphicsPipeline::CreateRootSignature()
{
	P3DSignature signature = nullptr;

	// 정점 쉐이더, 픽셀 쉐이더, 입력 조립기, 출력 병합기 
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC signature_desc{};
	signature_desc.NumParameters = static_cast<UINT>(myShaderUniforms.size());
	signature_desc.pParameters = myShaderUniforms.data();
	signature_desc.NumStaticSamplers = 0;
	signature_desc.pStaticSamplers = nullptr;
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
		throw "루트 서명을 생성하지 못함!";
	}

	if (signature_blob)
	{
		signature_blob->Release();
	}

	if (error_blob)
	{
		error_blob->Release();
	}

	mySignature = (signature);
}

void GraphicsPipeline::CreatePipelineState()
{
	if (isModified)
	{
		myDescription.pRootSignature = mySignature;
		myDescription.VS = myVertexShader->myCode;
		myDescription.PS = myPixelShader->myCode;

		ID3D12PipelineState* handle = nullptr;

		auto uuid = __uuidof(ID3D12PipelineState);
		auto place = reinterpret_cast<void**>(&handle);
		auto valid = dxDevice->CreateGraphicsPipelineState(&myDescription, uuid, place);
		if (FAILED(valid))
		{
			throw "파이프라인의 설정 실패!";
		}
		else
		{
			myState = handle;
		}

		isModified = false;
	}
}

void GraphicsPipeline::Start()
{}

void GraphicsPipeline::Reset()
{}

void GraphicsPipeline::Update(float delta_time)
{}

void GraphicsPipeline::PrepareRendering()
{
	if (mySignature)
	{
		dxTaskList->SetGraphicsRootSignature(mySignature);
	}

	if (myState)
	{
		dxTaskList->SetPipelineState(myState);
	}
}

void GraphicsPipeline::Render()
{}

void GraphicsPipeline::Release()
{
	if (mySignature)
	{
		mySignature->Release();
		mySignature = nullptr;
	}

	if (myState)
	{
		myState->Release();
		myState = nullptr;
	}
}

GraphicsPipeline& GraphicsPipeline::Attach(const D3D12_ROOT_PARAMETER& param)
{
	myShaderUniforms.push_back(param);

	isModified = true;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::Attach(P3DSignature* signature)
{
	mySignature = *signature;

	isModified = true;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::AttachVertexShader(const Shader& shader)
{
	myVertexShader = make_shared<Shader>(shader);

	return *this;
}

GraphicsPipeline& GraphicsPipeline::AttachPixelShader(const Shader& shader)
{
	myPixelShader = make_shared<Shader>(shader);

	return *this;
}

GraphicsPipeline& GraphicsPipeline::Attach(const D3D12_INPUT_LAYOUT_DESC& desc)
{
	myDescription.InputLayout = desc;

	isModified = true;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::Attach(D3D12_INPUT_LAYOUT_DESC&& desc)
{
	myDescription.InputLayout = std::forward<D3D12_INPUT_LAYOUT_DESC>(desc);

	isModified = true;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::Attach(const D3D12_RASTERIZER_DESC& desc)
{
	myDescription.RasterizerState = desc;

	isModified = true;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::Attach(D3D12_RASTERIZER_DESC&& desc)
{
	myDescription.RasterizerState = std::forward<D3D12_RASTERIZER_DESC>(desc);

	isModified = true;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::Attach(const D3D12_BLEND_DESC& desc)
{
	myDescription.BlendState = desc;

	isModified = true;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::Attach(D3D12_BLEND_DESC&& desc)
{
	myDescription.BlendState = std::forward<D3D12_BLEND_DESC>(desc);

	isModified = true;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::Attach(const D3D12_DEPTH_STENCIL_DESC& desc)
{
	myDescription.DepthStencilState = desc;

	isModified = true;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::Attach(D3D12_DEPTH_STENCIL_DESC&& desc)
{
	myDescription.DepthStencilState = std::forward<D3D12_DEPTH_STENCIL_DESC>(desc);

	isModified = true;
	return *this;
}

bool GraphicsPipeline::IsModified() const
{
	return isModified;
}
