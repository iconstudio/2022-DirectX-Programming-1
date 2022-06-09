#pragma once

class Shader
{
public:
	Shader(const Filepath& filepath, const char* entry, const char* version);
	Shader(const char* version);
	~Shader();

	void Load(const Filepath& filepath);
	void Complile(const Filepath& filepath, const char* entry);

	bool isCompiled;

	const std::string myVersion;

	ShaderBlob myCode;
	ID3DBlob* myBlob;
};
