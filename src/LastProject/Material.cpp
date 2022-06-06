#include "pch.hpp"
#include "Material.hpp"
#include "Shader.h"

RawMaterial::RawMaterial(const UINT type)
	: myType(type)
{}

RawMaterial::~RawMaterial()
{}

CMaterialColors::CMaterialColors(RawMaterial* pMaterialInfo)
{
	m_xmf4Diffuse = pMaterialInfo->m_xmf4AlbedoColor;
	m_xmf4Specular = pMaterialInfo->m_xmf4SpecularColor; //(r,g,b,a=power)
	m_xmf4Specular.w = (pMaterialInfo->m_fGlossiness * 255.0f);
	m_xmf4Emissive = pMaterialInfo->m_xmf4EmissiveColor;
}

CMaterial::CMaterial()
{}

CMaterial::~CMaterial()
{
	if (m_pShader) m_pShader->Release();
	if (m_pMaterialColors) m_pMaterialColors->Release();
}

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

void CMaterial::SetMaterialColors(CMaterialColors* pMaterialColors)
{
	if (m_pMaterialColors)
	{
		m_pMaterialColors->Release();
	}

	m_pMaterialColors = pMaterialColors;

	if (m_pMaterialColors)
	{
		m_pMaterialColors->AddRef();
	}
}

void CMaterial::PrepareRendering(P3DGrpCommandList cmdlist)
{
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_pMaterialColors->m_xmf4Ambient), 16);
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_pMaterialColors->m_xmf4Diffuse), 20);
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_pMaterialColors->m_xmf4Specular), 24);
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_pMaterialColors->m_xmf4Emissive), 28);
}

