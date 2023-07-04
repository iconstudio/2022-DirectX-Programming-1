#include "pch.hpp"
#include "PlainGraphicsPipeline.hpp"

PlainGraphicsPipeline::PlainGraphicsPipeline()
	: Pipeline()
{}

PlainGraphicsPipeline::~PlainGraphicsPipeline()
{}

void PlainGraphicsPipeline::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	//AssignVertexShader(Shader("PlainVertexShader.hlsl.", "main", "vs_5_1"));
	//AssignPixelShader(Shader("PlainPixelShader.hlsl.", "main", "ps_5_1"));
	AssignVertexShader(Shader("PlainVertexShader.cso", "vs_5_1"));
	AssignPixelShader(Shader("PlainPixelShader.cso", "ps_5_1"));


	Pipeline::Awake(device, cmdlist);
}

ShaderBlob PlainGraphicsPipeline::CreateVertexShader()
{
	return myVertexShader.myCode;
}

ShaderBlob PlainGraphicsPipeline::CreatePixelShader()
{
	return myPixelShader.myCode;
}

D3D12_INPUT_LAYOUT_DESC PlainGraphicsPipeline::CreateInputLayout()
{
	constexpr UINT params_count = 2;
	auto vs_params = new D3D12_INPUT_ELEMENT_DESC[params_count]{};

	vs_params[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	vs_params[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc{};
	d3dInputLayoutDesc.pInputElementDescs = vs_params;
	d3dInputLayoutDesc.NumElements = params_count;

	return d3dInputLayoutDesc;
}

P3DSignature PlainGraphicsPipeline::CreateGraphicsRootSignature()
{
	P3DSignature signature = nullptr;

	D3D12_ROOT_PARAMETER shader_params[1]{};

	shader_params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	shader_params[0].Constants.ShaderRegister = 0; // Resolution
	shader_params[0].Constants.RegisterSpace = 2;
	shader_params[0].Constants.Num32BitValues = 2; // Resolution
	shader_params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// ���� ���̴�, �ȼ� ���̴�, �Է� ������, ��� ���ձ� 
	auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC signature_desc{};
	signature_desc.NumParameters = static_cast<UINT>(std::size(shader_params));
	signature_desc.pParameters = shader_params;
	signature_desc.NumStaticSamplers = 0; // �ؽ�ó
	signature_desc.pStaticSamplers = NULL;
	signature_desc.Flags = flags;

	ID3DBlob* signature_blob = nullptr;
	ID3DBlob* error_blob = nullptr;

	auto valid = D3D12SerializeRootSignature(&signature_desc
		, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob
		, &error_blob);
	if (FAILED(valid))
	{
		throw "��Ʈ ������ �޸𸮸� �Ҵ����� ����!";
	}

	UINT gpu_mask = 0;
	auto uuid = __uuidof(ID3D12RootSignature);
	auto place = reinterpret_cast<void**>(&signature);
	valid = dxDevice->CreateRootSignature(gpu_mask
		, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize()
		, uuid, place);
	if (FAILED(valid))
	{
		throw "��Ʈ ������ �������� ����!";
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
