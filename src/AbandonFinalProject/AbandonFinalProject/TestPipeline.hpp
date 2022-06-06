#pragma once
#include "GraphicsPipeline.hpp"

struct TestInstanceBlob
{
	XMFLOAT3 myPosition;
	XMFLOAT4 myColour;
};

class TestPipeline : public GraphicsPipeline
{
public:
	virtual void Release() override;

	ID3D12Resource* myInstanceInfo;
	TestInstanceBlob* myMappedInstance;

protected:
	virtual void CreateUniforms();
};
