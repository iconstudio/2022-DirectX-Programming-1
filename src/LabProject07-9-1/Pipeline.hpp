#pragma once
#include "Shader.h"

class Pipeline
{
public:
	Pipeline();
	Pipeline(const PipelineDesc& desc);
	Pipeline(PipelineDesc&& desc);
	virtual ~Pipeline();

	virtual void Awake(PtrDevice device, PtrGrpCommandList cmd_list);
	void Attach(const D3D12_INPUT_LAYOUT_DESC& desc);
	void Attach(const D3D12_RASTERIZER_DESC& desc);
	void Attach(const D3D12_BLEND_DESC& desc);
	void Attach(const D3D12_DEPTH_STENCIL_DESC& desc);
	void Attach(const shared_ptr<Shader>& ptr);
	void Attach(shared_ptr<Shader>&& ptr);

	// ���� ���������� ���� �������� ������ ���� ����
	void CreateStateForNow();
	// ���̴��� �ҷ����� �������ϱ�
	shared_ptr<Shader> CreateShader(const Filepath& path, const char* version, const char* entry);
	// ������ �� ���̴� ���̳ʸ� �ҷ�����
	shared_ptr<Shader> LoadShader(const Filepath& path);

	// ����
	virtual void Start(const shared_ptr<Shader>& vs, const shared_ptr<Shader>& ps);
	virtual void Start(shared_ptr<Shader>&& vs, shared_ptr<Shader>&& ps);

	// ������
	void PrepareRendering(PtrGrpCommandList pd3dCommandList, int pipeline_index);

	// ���̴� ���
	shared_ptr<Shader> GetShader(const UINT index) const;
	PtrSignature GetGraphicsRootSignature();
	ID3D12RootSignature const* GetGraphicsRootSignature() const;

	virtual PtrSignature CreateGraphicsRootSignature() = 0;
	virtual shared_ptr<Shader> CreateVertexShader(const Filepath& path) = 0;
	virtual shared_ptr<Shader> CreatePixelShader(const Filepath& path) = 0;

protected:
	D3D12_INPUT_LAYOUT_DESC CreateEmptyInputLayout() const;
	D3D12_RASTERIZER_DESC CreateEmptyRasterizerState() const;
	D3D12_BLEND_DESC CreateEmptyBlendState() const;
	D3D12_DEPTH_STENCIL_DESC CreateEmptyDepthStencilState() const;

	PtrDevice d3dDevice;
	PtrGrpCommandList d3dTaskList;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_d3dPipelineStateDesc;
	PtrSignature myRootSignature;
	D3D12_INPUT_LAYOUT_DESC myShaderAttributes;
	D3D12_RASTERIZER_DESC myRasterizerOptions;
	D3D12_BLEND_DESC myBlendingOptions;
	D3D12_DEPTH_STENCIL_DESC myDepthStencilOptions;

	std::vector<shared_ptr<Shader>> myShaders;

	std::vector<unique_ptr<ID3D12PipelineState>> myPipelineStates;
}; 
