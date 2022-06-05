#include "pch.hpp"
#include "TestPipeline.hpp"

void TestPipeline::CreateUniforms()
{
	// Camera
	D3D12_ROOT_PARAMETER param0{};
	param0.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	param0.Descriptor.ShaderRegister = 0;
	param0.Descriptor.RegisterSpace = 0;
	param0.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	Attach(param0);

	// GameObject
	D3D12_ROOT_PARAMETER param1{};
	param1.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	param1.Constants.Num32BitValues = 32;
	param1.Constants.ShaderRegister = 1;
	param1.Constants.RegisterSpace = 0;
	param1.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	Attach(param1);

	UINT aligned_sz = ((sizeof(TestInstanceBlob) + 255) & ~255);
	myInstanceInfo = ::CreateBufferResource(dxDevice, dxTaskList
		, NULL, aligned_sz
		, D3D12_HEAP_TYPE_UPLOAD
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, NULL);

	auto place = reinterpret_cast<void**>(&myMappedInstance);
	auto valid = myInstanceInfo->Map(0, NULL, place);
	if (FAILED(valid))
	{
		throw "파이프라인 초기화 실패!";
	}
}
