#pragma once
#include "Object.h"
#include "Camera.h"

class Shader
{
public:
	Shader(const char* version, const char* entry);
	Shader(const ShaderHandle& handle);
	Shader(ShaderHandle&& handle);
	virtual ~Shader();

	std::string myVersion;
	std::string myEntry;
	ShaderHandle myHandle;
	unique_ptr<ID3DBlob> myBlob;
};
