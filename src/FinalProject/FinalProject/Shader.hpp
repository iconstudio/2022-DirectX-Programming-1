#pragma once
class Shader
{
public:
	Shader(const char* version);
	~Shader();

	void Load(const Filepath& filepath);
	void Complile(const Filepath& filepath, const char* entry);

	bool isCompiled;

	const std::string myVersion;

	D3DByteCode myCode;
	shared_ptr<ID3DBlob*> myBlob;
};
