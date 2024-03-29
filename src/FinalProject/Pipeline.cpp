#include "pch.hpp"
#include "Pipeline.hpp"

Pipeline* Pipeline::diffusedShader = nullptr;
Pipeline* Pipeline::illuminatedShader = nullptr;
Pipeline* Pipeline::plainShader = nullptr;

Pipeline::Pipeline()
	: dxDevice(nullptr), dxCommandList(nullptr)
	, myVertexShader("vs_5_1"), myPixelShader("ps_5_1"), mySignature(nullptr)
	, myStateDescription()
	, myDerivedStates()
{
	myStateDescription.SampleMask = UINT_MAX;
	myStateDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	myStateDescription.NumRenderTargets = 1;
	myStateDescription.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	myStateDescription.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	myStateDescription.SampleDesc.Count = 1;
	myStateDescription.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
}

Pipeline::~Pipeline()
{
	if (0 < myDerivedStates.size())
	{
		std::for_each(myDerivedStates.begin(), myDerivedStates.end()
			, [](ID3D12PipelineState* state) {
				state->Release();
			});
	}

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

void Pipeline::AssignVertexShader(const Shader& vs)
{
	myVertexShader = vs;
}

void Pipeline::AssignVertexShader(Shader&& vs)
{
	myVertexShader = std::forward<Shader>(vs);
}

void Pipeline::AssignPixelShader(const Shader& ps)
{
	myPixelShader = ps;
}

void Pipeline::AssignPixelShader(Shader&& ps)
{
	myPixelShader = std::forward<Shader>(ps);
}

D3D12_RASTERIZER_DESC Pipeline::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC result{};
	result.FillMode = D3D12_FILL_MODE_SOLID;
	result.CullMode = D3D12_CULL_MODE_BACK;
	result.FrontCounterClockwise = FALSE;
	result.DepthBias = 0;
	result.DepthBiasClamp = 0.0f;
	result.SlopeScaledDepthBias = 0.0f;
	result.DepthClipEnable = TRUE;
	result.MultisampleEnable = TRUE;
	result.AntialiasedLineEnable = TRUE;
	result.ForcedSampleCount = 0;
	result.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return result;
}

D3D12_DEPTH_STENCIL_DESC Pipeline::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC result{};
	result.DepthEnable = TRUE;
	result.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	result.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	result.StencilEnable = FALSE;
	result.StencilReadMask = 0x00;
	result.StencilWriteMask = 0x00;
	result.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	result.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	result.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	result.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	result.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	result.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	result.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	result.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return result;
}

D3D12_BLEND_DESC Pipeline::CreateBlendState()
{
	D3D12_BLEND_DESC result{};
	result.AlphaToCoverageEnable = FALSE;
	result.IndependentBlendEnable = FALSE;
	result.RenderTarget[0].LogicOpEnable = FALSE;
	result.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;

	result.RenderTarget[0].BlendEnable = TRUE;
	result.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	result.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	result.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	result.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	result.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	result.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	result.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return result;
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
