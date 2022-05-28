#pragma once
#include "Shader.h"
#include "Pipeline.hpp"

class RawMaterial
{
public:
	RawMaterial();
	virtual ~RawMaterial();

	UINT myType;

	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4AlbedoColor;
	XMFLOAT4 m_xmf4EmissiveColor;
	XMFLOAT4 m_xmf4SpecularColor;

	float m_fGlossiness = 0.0f;
	float m_fSmoothness = 0.0f;
	float m_fSpecularHighlight = 0.0f;
	float m_fMetallic = 0.0f;
	float m_fGlossyReflection = 0.0f;
};

class CMaterial
{
public:
	CMaterial(Pipeline& pipeline);
	CMaterial(Pipeline& pipeline, const RawMaterial& raw);
	CMaterial(Pipeline& pipeline, RawMaterial&& raw);
	virtual ~CMaterial();

	void UpdateShaderVariable(PtrGrpCommandList pd3dCommandList);

	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT4 m_xmf4Emissive;
	
	Pipeline& myPipeline;
};
