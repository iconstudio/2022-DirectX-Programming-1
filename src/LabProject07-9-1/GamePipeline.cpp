#include "pch.hpp"
#include "stdafx.h"
#include "GamePipeline.hpp"
#include "Shader.h"

void DefaultPipeline::Awake(PtrDevice device, PtrGrpCommandList cmd_list)
{
	Pipeline::Awake(device, cmd_list);

	auto attributes_count = 2U;
	auto vs_attributes = new D3D12_INPUT_ELEMENT_DESC[attributes_count];
	vs_attributes[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT
		, 0
		, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	vs_attributes[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT
		, 1
		, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	auto input_layout = CreateEmptyInputLayout();
	input_layout.pInputElementDescs = vs_attributes;
	input_layout.NumElements = attributes_count;
	Attach(input_layout);

	auto d3dRasterizerDesc = CreateEmptyRasterizerState();
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
	Attach(d3dRasterizerDesc);

	auto d3dBlendDesc = CreateEmptyBlendState();
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
	Attach(d3dBlendDesc);

	auto d3dDepthStencilDesc = CreateEmptyDepthStencilState();
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
	Attach(d3dDepthStencilDesc);
}

void DefaultPipeline::Start(const shared_ptr<Shader>& vs, const shared_ptr<Shader>& ps)
{
	Start(shared_ptr<Shader>(vs), shared_ptr<Shader>(ps));
}

void DefaultPipeline::Start(shared_ptr<Shader>&& vs, shared_ptr<Shader>&& ps)
{
	Pipeline::Start(std::forward<shared_ptr<Shader>>(vs), std::forward<shared_ptr<Shader>>(ps));

	m_d3dPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	CreateStateForNow();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
	}

	//if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	//if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();
}

PtrSignature DefaultPipeline::CreateGraphicsRootSignature()
{
	D3D12_ROOT_PARAMETER root_parameters[3]{};

	// register(b1)
	root_parameters[0].Descriptor.ShaderRegister = 1; // Camera
	root_parameters[0].Descriptor.RegisterSpace = 0;
	root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	root_parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// register(b2)
	root_parameters[1].Constants.ShaderRegister = 2; // GameObject
	root_parameters[1].Constants.RegisterSpace = 0;
	root_parameters[1].Constants.Num32BitValues = 64; // 최대 64개
	root_parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	root_parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// register(b4)
	root_parameters[2].Descriptor.ShaderRegister = 4; // Lights
	root_parameters[2].Descriptor.RegisterSpace = 0;
	root_parameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	root_parameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS root_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC rootsignature_desc{};
	::ZeroMemory(&rootsignature_desc, sizeof(rootsignature_desc));

	rootsignature_desc.NumParameters = std::size(root_parameters);
	rootsignature_desc.pParameters = root_parameters;
	rootsignature_desc.NumStaticSamplers = 0;
	rootsignature_desc.pStaticSamplers = nullptr;
	rootsignature_desc.Flags = root_flags;

	ID3DBlob* signature_blob = nullptr;
	ID3DBlob* error_blob = nullptr;
	auto valid = D3D12SerializeRootSignature(&rootsignature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &error_blob);
	if (FAILED(valid))
	{
		throw "루트 서명 서술자의 직렬화 실패!";
	}

	if (error_blob)
	{
		volatile char* error_info = nullptr;
		error_info = reinterpret_cast<char*>(error_blob->GetBufferPointer());

		throw error_info;
	}

	UINT gpu_mask = 0;
	void* blob = signature_blob->GetBufferPointer();
	size_t blob_size = signature_blob->GetBufferSize();

	PtrSignature root_signature = nullptr;
	auto uuid = __uuidof(ID3D12RootSignature);
	void** place = reinterpret_cast<void**>(&root_signature);
	valid = d3dDevice->CreateRootSignature(gpu_mask, blob, blob_size, uuid, place);
	if (FAILED(valid))
	{
		throw "루트 서명 생성 실패!";
	}

	if (signature_blob)
	{
		signature_blob->Release();
	}

	if (error_blob)
	{
		error_blob->Release();
	}
}

shared_ptr<Shader> DefaultPipeline::CreateVertexShader(const Filepath& path)
{
	return CreateShader(path, "vs_5_1", "main");
}

shared_ptr<Shader> DefaultPipeline::CreatePixelShader(const Filepath& path)
{
	return CreateShader(path, "ps_5_1", "main");
}
