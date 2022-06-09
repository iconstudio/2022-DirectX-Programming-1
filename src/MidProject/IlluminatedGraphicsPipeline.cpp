#include "pch.hpp"
#include "IlluminatedGraphicsPipeline.hpp"

IlluminatedGraphicsPipeline::IlluminatedGraphicsPipeline()
	: Pipeline()
{}

IlluminatedGraphicsPipeline::~IlluminatedGraphicsPipeline()
{}

ShaderBlob IlluminatedGraphicsPipeline::CreateVertexShader()
{
	return(Pipeline::CompileShaderFromFile(L"VertexShader.hlsl", "main", "vs_5_1", &myVertexShaderBlob));
}

ShaderBlob IlluminatedGraphicsPipeline::CreatePixelShader()
{
	return(Pipeline::CompileShaderFromFile(L"PixelShader.hlsl", "main", "ps_5_1", &myPixelShaderBlob));
}

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

	return signature;
}

void IlluminatedGraphicsPipeline::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	myDerivedStates.clear();
	myDerivedStates.reserve(2);

	Pipeline::Awake(device, cmdlist);

	myStateDescription.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	
	BuildState(1);
}
