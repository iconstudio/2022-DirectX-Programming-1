#include "pch.hpp"
#include "stdafx.h"
#include "Material.hpp"
#include "Shader.h"

RawMaterial::RawMaterial()
	: myType(0)
	, m_xmf4Ambient(DefaultAmient)
	, m_xmf4AlbedoColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
	, m_xmf4EmissiveColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
	, m_xmf4SpecularColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
{}

RawMaterial::~RawMaterial()
{}

CMaterial::CMaterial(const RawMaterial& raw)
	: CMaterial(RawMaterial(raw))
{}

CMaterial::CMaterial(RawMaterial&& raw)
{
	m_xmf4Ambient = std::forward<XMFLOAT4>(raw.m_xmf4Ambient);
	m_xmf4Diffuse = std::forward<XMFLOAT4>(raw.m_xmf4AlbedoColor);
	m_xmf4Specular = std::forward<XMFLOAT4>(raw.m_xmf4SpecularColor);
	m_xmf4Specular.w = std::forward<float>(raw.m_fGlossiness) * 255.0f;
	m_xmf4Emissive = std::forward<XMFLOAT4>(raw.m_xmf4EmissiveColor);
}

CMaterial::~CMaterial()
{
}

void CMaterial::SetShader(CShader* pShader)
{
	m_pShader = pShader;
}

void CMaterial::UpdateShaderVariable(PtrGrpCommandList  cmd_list)
{
	// register(b1)
	cmd_list->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Ambient), 16);
	cmd_list->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Diffuse), 20);
	cmd_list->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Specular), 24);
	cmd_list->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Emissive), 28);
}
