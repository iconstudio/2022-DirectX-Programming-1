#pragma once
class Shader
{
public:
	Shader(const Shader& other) = default;
	Shader(Shader&& other) = default;
	Shader(const char* version);
	~Shader();

	void Load(const Filepath& filepath);
	void Complile(const Filepath& filepath, const char* entry);

	bool isCompiled;

	const std::string myVersion;

	D3DByteCode myCode;
	unique_ptr<ID3DBlob*> myBlob;
};
