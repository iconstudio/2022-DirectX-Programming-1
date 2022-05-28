#pragma once
#include "Pipeline.hpp"

class DefaultPipeline : public Pipeline
{
public:
	void Awake(PtrDevice device, PtrGrpCommandList cmd_list) override;

	void Start(const shared_ptr<Shader>& vs, const shared_ptr<Shader>& ps) override;
	void Start(shared_ptr<Shader>&& vs, shared_ptr<Shader>&& ps) override;

	PtrSignature CreateGraphicsRootSignature() override;
	shared_ptr<Shader> CreateVertexShader(const Filepath& path) override;
	shared_ptr<Shader> CreatePixelShader(const Filepath& path) override;
};
