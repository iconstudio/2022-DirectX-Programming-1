﻿#pragma once
#include "targetver.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#define WIN32_LEAN_AND_MEAN 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define NOMINMAX
#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using P3DDevice = ID3D12Device*;
using P3DCommandList = ID3D12CommandList*;
using P3DGrpCommandList = ID3D12GraphicsCommandList*;
using P3DSignature = ID3D12RootSignature*;
using D3DHandle = D3D12_CPU_DESCRIPTOR_HANDLE;
using D3DByteCode = D3D12_SHADER_BYTECODE;

#include <memory>
#include <filesystem>
#include <string>
#include <initializer_list>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <numbers>
#include <algorithm>
using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;
using std::make_shared;
using std::make_unique;
using std::make_pair;
using Filepath = std::filesystem::path;
constexpr auto PI = std::numbers::pi;

class XYZWrapper
{
public:
	XYZWrapper(float& x, float& y, float& z)
		: x(x), y(y), z(z)
	{}

	XYZWrapper(XMFLOAT3& position)
		: XYZWrapper(position.x, position.y, position.z)
	{}

	XYZWrapper(XMFLOAT3&& position) = delete;

	XYZWrapper& operator=(float list[3])
	{
		x = list[0];
		y = list[1];
		z = list[2];
		return *this;
	}

	XYZWrapper& operator=(const XMFLOAT3& vector)
	{
		x = vector.x;
		y = vector.y;
		z = vector.z;
		return *this;
	}

	XYZWrapper& operator=(XMFLOAT3&& vector)
	{
		x = std::forward<float>(vector.x);
		y = std::forward<float>(vector.y);
		z = std::forward<float>(vector.z);
		return *this;
	}

	explicit operator XMFLOAT3() const noexcept
	{
		return XMFLOAT3(x, y, z);
	}

	float& x;
	float& y;
	float& z;
};

template <>
struct std::default_delete<IUnknown>
{ // default deleter for unique_ptr
	constexpr default_delete() noexcept = default;

	template <class _Ty2, enable_if_t<is_convertible_v<_Ty2*, IUnknown*>, int> = 0>
	default_delete(const default_delete<_Ty2>&) noexcept {}

	void operator()(IUnknown* _Ptr) const noexcept /* strengthened */
	{ // delete a pointer
		static_assert(0 < sizeof(IUnknown), "can't delete an incomplete type");
		_Ptr->Release();
		delete _Ptr;
	}
};

struct signature_deleter
{
	constexpr signature_deleter() noexcept = default;

	void operator()(P3DSignature _Ptr) const noexcept
	{
		_Ptr->Release();
		delete _Ptr;
	}
};

struct pipelinestate_deleter
{
	constexpr pipelinestate_deleter() noexcept = default;

	void operator()(ID3D12PipelineState* _Ptr) const noexcept
	{
		_Ptr->Release();
		delete _Ptr;
	}
};
