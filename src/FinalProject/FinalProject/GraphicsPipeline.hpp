#pragma once
#include "PlayableObjet.hpp"

class GraphicsPipeline : public PlayableObjet
{
public:
	GraphicsPipeline();
	virtual ~GraphicsPipeline();

	GraphicsPipeline& Attach(P3DSignature* signature);
	GraphicsPipeline& Attach(const Shader& shader);
	GraphicsPipeline& Attach(Shader&& shader);
	GraphicsPipeline& Attach(const D3D12_INPUT_LAYOUT_DESC& desc);
	GraphicsPipeline& Attach(D3D12_INPUT_LAYOUT_DESC&& desc);
	GraphicsPipeline& Attach(const D3D12_RASTERIZER_DESC& desc);
	GraphicsPipeline& Attach(D3D12_RASTERIZER_DESC&& desc);
	GraphicsPipeline& Attach(const D3D12_DEPTH_STENCIL_DESC& desc);
	GraphicsPipeline& Attach(D3D12_DEPTH_STENCIL_DESC&& desc);

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Update(float delta_time) override;

	Shader CreateEmptyShader(const char* version) const;
	D3D12_INPUT_LAYOUT_DESC CreateEmptyInputLayout() const;
	D3D12_RASTERIZER_DESC CreateEmptyRasterizerState() const;
	D3D12_BLEND_DESC CreateEmptyBlendState() const;
	D3D12_DEPTH_STENCIL_DESC CreateEmptyDepthStencilState() const;

private:
	unique_ptr<ID3D12PipelineState*> myState;

	P3DSignature* mySignature;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC myDescription;

	std::vector<shared_ptr<Shader>> myShaders;
};
