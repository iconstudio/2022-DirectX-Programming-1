#pragma once
#include "PlayableObjet.hpp"

class GraphicsPipeline : public PlayableObjet
{
public:
	GraphicsPipeline(P3DDevice device, P3DGrpCommandList cmdlist);
	virtual ~GraphicsPipeline();

	GraphicsPipeline& Attach(const D3D12_ROOT_PARAMETER& param);
	GraphicsPipeline& Attach(P3DSignature* signature);
	GraphicsPipeline& Attach(const Shader& shader);
	GraphicsPipeline& Attach(Shader&& shader);
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

	bool IsModified() const;

private:
	P3DDevice dxDevice;
	P3DGrpCommandList dxCmdList;

	shared_ptr<P3DSignature> mySignature;
	std::vector<D3D12_ROOT_PARAMETER> myShaderUniforms;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC myDescription;
	shared_ptr<ID3D12PipelineState*> myState;
	bool isModified;

	std::vector<shared_ptr<Shader>> myShaders;
};
