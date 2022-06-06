#pragma once
#include "PlayableObjet.hpp"

class GraphicsPipeline : public PlayableObjet
{
public:
	GraphicsPipeline(P3DDevice device, P3DGrpCommandList cmdlist);
	virtual ~GraphicsPipeline();

	GraphicsPipeline& Attach(const D3D12_ROOT_PARAMETER& param);
	GraphicsPipeline& Attach(P3DSignature* signature);
	GraphicsPipeline& AttachVertexShader(const Shader& shader);
	GraphicsPipeline& AttachPixelShader(const Shader& shader);
	GraphicsPipeline& Attach(const D3D12_INPUT_LAYOUT_DESC& desc);
	GraphicsPipeline& Attach(D3D12_INPUT_LAYOUT_DESC&& desc);
	GraphicsPipeline& Attach(const D3D12_RASTERIZER_DESC& desc);
	GraphicsPipeline& Attach(D3D12_RASTERIZER_DESC&& desc);
	GraphicsPipeline& Attach(const D3D12_BLEND_DESC& desc);
	GraphicsPipeline& Attach(D3D12_BLEND_DESC&& desc);
	GraphicsPipeline& Attach(const D3D12_DEPTH_STENCIL_DESC& desc);
	GraphicsPipeline& Attach(D3D12_DEPTH_STENCIL_DESC&& desc);

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Update(float delta_time) override;
	virtual void PrepareRendering();
	virtual void Render();
	virtual void Release();

	bool IsModified() const;

protected:
	virtual void CreateUniforms();
	void CreateRootSignature();
	void CreatePipelineState();

	P3DDevice dxDevice;
	P3DGrpCommandList dxTaskList;

	P3DSignature mySignature;
	std::vector<D3D12_ROOT_PARAMETER> myShaderUniforms;

	//shared_ptr<GameCamera> mainCamera;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC myDescription;
	ID3D12PipelineState* myState;
	bool isModified;

	shared_ptr<Shader> myVertexShader;
	shared_ptr<Shader> myPixelShader;
};
