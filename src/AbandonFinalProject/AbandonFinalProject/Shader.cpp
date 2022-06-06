#include "pch.hpp"
#include "Shader.hpp"

Shader::Shader(const Filepath& filepath, const char* entry, const char* version)
	: isCompiled(false)
	, myVersion(version)
	, myCode(), myBlob(nullptr)
{
	Complile(filepath, entry);
}

Shader::Shader(const char* version)
	: isCompiled(false)
	, myVersion(version)
	, myCode(), myBlob(nullptr)
{}

Shader::~Shader()
{
	if (myBlob)
	{
		//myBlob->Release();
	}
}

void Shader::Load(const Filepath& filepath)
{
	std::ifstream shader_file{};
	shader_file.open(filepath, std::ios::in | std::ios::ate | std::ios::binary);

	auto code_size = shader_file.tellg();
	BYTE* bytecode = new BYTE[code_size];
	shader_file.seekg(0);

	auto place = reinterpret_cast<char*>(bytecode);
	shader_file.read(place, code_size);
	shader_file.close();

	D3DByteCode result{};
	if (myBlob)
	{
		myBlob->Release();
		//myBlob.reset();

		auto valid = D3DCreateBlob(code_size, &myBlob);
		if (FAILED(valid))
		{
			throw "기존 쉐이더의 바이트코드를 불러올 수 없음!";
		}

		memcpy(myBlob->GetBufferPointer(), bytecode, code_size);

		result.BytecodeLength = myBlob->GetBufferSize();
		result.pShaderBytecode = myBlob->GetBufferPointer();
	}
	else
	{
		result.BytecodeLength = code_size;
		result.pShaderBytecode = bytecode;
	}

	myCode = result;
}

void Shader::Complile(const Filepath& filepath, const char* entry)
{
	UINT compile_options = 0;
#if defined(_DEBUG)
	compile_options = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	if (!std::filesystem::exists(filepath))
	{
		throw "쉐이더 파일을 찾을 수 없음!";
	}

	const auto shader_path = filepath.c_str();

	ID3DBlob* shader_blob = nullptr;
	ID3DBlob* error_blob = nullptr;
	auto valid = D3DCompileFromFile(shader_path
		, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, entry, myVersion.c_str()
		, compile_options, 0
		, &shader_blob
		, &error_blob);
	if (FAILED(valid))
	{
		throw "쉐이더 컴파일 실패!";
	}

	volatile char* error_contents = nullptr;
	if (error_blob)
	{
		error_contents = reinterpret_cast<char*>(error_blob->GetBufferPointer());
		//throw error_contents; // 경고도 포함

		CHAR pstrDebug[256]{};
		snprintf(pstrDebug, 256, "%s", error_contents);

		OutputDebugStringA(pstrDebug);
	}

	D3DByteCode bytecode{};
	bytecode.BytecodeLength = shader_blob->GetBufferSize();
	bytecode.pShaderBytecode = shader_blob->GetBufferPointer();

	myCode = bytecode;
	myBlob = shader_blob;

	if (shader_blob)
	{
		isCompiled = true;
	}
}
