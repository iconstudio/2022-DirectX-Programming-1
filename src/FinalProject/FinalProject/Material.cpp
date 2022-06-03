#include "pch.hpp"
#include "Material.hpp"

Material::Material(RawMaterial* info)
{
	m_xmf4Diffuse = info->m_xmf4AlbedoColor;
	m_xmf4Specular = info->m_xmf4SpecularColor; // (r,g,b,a=power)
	m_xmf4Specular.w = (info->m_fGlossiness * 255.0f);
	m_xmf4Emissive = info->m_xmf4EmissiveColor;
}

Material::Material(const int id)
	: myShaderID(id)
{}

Material::~Material()
{}

void Material::SetShader(const int id)
{
	myShaderID = id;
}

void Material::PrepareRendering(P3DGrpCommandList cmdlist)
{
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Ambient), 16);
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Diffuse), 20);
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Specular), 24);
	cmdlist->SetGraphicsRoot32BitConstants(1, 4, &(m_xmf4Emissive), 28);
}
