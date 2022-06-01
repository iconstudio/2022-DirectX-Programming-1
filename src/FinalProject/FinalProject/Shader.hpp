#pragma once
class Shader
{
public:
	Shader(const Shader& other);
	Shader(Shader&& other);
	Shader(const char* version);

	bool Load(const char* filepath, const char* entry);

	bool isCompiled;

	const std::string myVersion;

	D3DByteCode myCode;
	ID3DBlob* myBlob;
};
