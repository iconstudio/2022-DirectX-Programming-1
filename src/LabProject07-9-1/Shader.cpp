#include "pch.hpp"
#include "stdafx.h"
#include "Shader.h"

Shader::Shader(const char* version, const char* entry)
	: myVersion(version), myEntry(entry)
	, myHandle()
	, myBlob(nullptr)
{}

Shader::Shader(const ShaderHandle& handle)
	: myVersion(""), myEntry("main")
	, myHandle(handle)
	, myBlob(nullptr)
{}

Shader::Shader(ShaderHandle&& handle)
	: myVersion(""), myEntry("main")
	, myHandle(std::forward<ShaderHandle>(handle))
	, myBlob(nullptr)
{}

Shader::~Shader()
{}
