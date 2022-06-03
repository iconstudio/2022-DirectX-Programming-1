#include "pch.hpp"
#include "Material.hpp"

Material::Material(const int id)
	: myShaderID(id)
{}

Material::~Material()
{}

void Material::SetShader(const int id)
{
	myShaderID = id;
}
