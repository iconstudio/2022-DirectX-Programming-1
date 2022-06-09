#include "pch.hpp"
#include "Material.hpp"
#include "Pipeline.hpp"

RawMaterial::RawMaterial(const UINT type)
	: myType(type)
{}

RawMaterial::~RawMaterial()
{}

CMaterial::CMaterial()
	: m_pShader(nullptr)
	, m_xmf4Ambient(XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f))
	, m_xmf4Diffuse(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
	, m_xmf4Specular(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
	, m_xmf4Emissive(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
{}

CMaterial::CMaterial(RawMaterial* raw_material)
	: CMaterial()
{
	m_xmf4Diffuse = raw_material->m_xmf4AlbedoColor;
	m_xmf4Specular = raw_material->m_xmf4SpecularColor; // (r,g,b,a=power)
	m_xmf4Specular.w = (raw_material->m_fGlossiness * 255.0f);
	m_xmf4Emissive = raw_material->m_xmf4EmissiveColor;
}

CMaterial& CMaterial::operator=(RawMaterial* raw_material)
{
	m_xmf4Diffuse = raw_material->m_xmf4AlbedoColor;
	m_xmf4Specular = raw_material->m_xmf4SpecularColor; // (r,g,b,a=power)
	m_xmf4Specular.w = (raw_material->m_fGlossiness * 255.0f);
	m_xmf4Emissive = raw_material->m_xmf4EmissiveColor;

	return *this;
}

CMaterial::~CMaterial()
{}

void CMaterial::SetShader(Pipeline* pShader)
{
	if (m_pShader)
	{
		m_pShader->Release();
	}

	m_pShader = pShader;

	if (m_pShader)
	{
		m_pShader->AddRef();
	}
}

void CMaterial::PrepareRendering(P3DGrpCommandList cmdlist) const
{
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Ambient), 16);
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Diffuse), 20);
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Specular), 24);
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Emissive), 28);
}

