#include "pch.hpp"
#include "GraphicsPipeline.hpp"
#include "Shader.hpp"

GraphicsPipeline::GraphicsPipeline(P3DDevice device, P3DGrpCommandList cmdlist)
	: dxDevice(device), dxCmdList(cmdlist), mySignature(nullptr)
	, myDescription(), myState(nullptr), isModified(false)
{
#ifdef _DEBUG
	myDescription.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
#else
	myDescription.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
#endif

	myDescription.NodeMask = 0;
	myDescription.SampleMask = UINT_MAX;
	myDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	myDescription.NumRenderTargets = 1;
	myDescription.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	myDescription.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	myDescription.SampleDesc.Count = 1;
}

GraphicsPipeline::~GraphicsPipeline()
{

}

void GraphicsPipeline::Awake()
{
	P3DSignature signature = nullptr;

	D3D12_ROOT_PARAMETER shader_params[3]{};
	ZeroMemory(&shader_params, sizeof(shader_params));

	shader_params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	shader_params[0].Descriptor.ShaderRegister = 1; // Camera
	shader_params[0].Descriptor.RegisterSpace = 0;
	shader_params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	shader_params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	shader_params[1].Constants.Num32BitValues = 32;
	shader_params[1].Constants.ShaderRegister = 2; // GameObject

	shader_params[1].Constants.RegisterSpace = 0;
	shader_params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	shader_params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	shader_params[2].Descriptor.ShaderRegister = 4; // Lights
	shader_params[2].Descriptor.RegisterSpace = 0;
	shader_params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// 정점 쉐이더, 픽셀 쉐이더, 입력 조립기, 출력 병합기 
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC signature_desc;
	ZeroMemory(&signature_desc, sizeof(signature_desc));

	signature_desc.NumParameters = _countof(shader_params);
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

	//mySignature = shared_ptr<P3DSignature>(&signature);

	if (isModified)
	{
		ID3D12PipelineState** state = nullptr;

		auto uuid = __uuidof(ID3D12PipelineState);
		auto place = reinterpret_cast<void**>(&state);
		auto valid = dxDevice->CreateGraphicsPipelineState(&myDescription, uuid, place);
		if (FAILED(valid))
		{
			throw "파이프라인의 설정 실패!";
		}
		else
		{
			myState = unique_ptr<ID3D12PipelineState*>(state);
		}
	}
}

void GraphicsPipeline::Start()
{
}

void GraphicsPipeline::Reset()
{}

void GraphicsPipeline::Update(float delta_time)
{}

GraphicsPipeline& GraphicsPipeline::Attach(const D3D12_ROOT_PARAMETER& param)
{
	myShaderUniforms.push_back(param);
}

GraphicsPipeline& GraphicsPipeline::Attach(P3DSignature* signature)
{
	mySignature = unique_ptr<P3DSignature>(signature);

	isModified = true;
	return *this;
}

GraphicsPipeline& GraphicsPipeline::Attach(const Shader& shader)
{
	myShaders.emplace_back(new Shader(shader));

	return *this;
}

GraphicsPipeline& GraphicsPipeline::Attach(Shader&& shader)
{
	myShaders.emplace_back(new Shader(std::forward<Shader>(shader)));

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
	myDescription.RasterizerState = desc;

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
	myDescription.BlendState = desc;

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
	myDescription.DepthStencilState = desc;

	isModified = true;
	return *this;
}

bool GraphicsPipeline::IsModified() const
{
	return isModified;
}
