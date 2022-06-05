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
	ID3D12Resource* myInstanceInfo;
	GameCameraBlob* myMappedInstance;

protected:
	virtual void CreateUniforms();
};
