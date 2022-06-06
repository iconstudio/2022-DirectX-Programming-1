#pragma once
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#define WIN32_LEAN_AND_MEAN 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define NOMINMAX
#define NOATOM
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOOPENFILE
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
#include <cmath>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>
#include <iterator>
#include <numbers>
#include <random>
#include <string>
#include <algorithm>
using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;
using std::make_shared;
using std::make_unique;
using std::make_pair;
using Filepath = std::filesystem::path;
constexpr auto PI = std::numbers::pi;

bool operator==(const XMFLOAT3& lhs, const XMFLOAT3& rhs);
bool operator<(const XMFLOAT3& lhs, const XMFLOAT3& rhs);

extern ID3D12Resource* CreateBufferResource(P3DDevice device
	, P3DGrpCommandList cmdlist
	, const void* origin_data, UINT origin_size
	, D3D12_HEAP_TYPE heap_type = D3D12_HEAP_TYPE_UPLOAD
	, D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
	, ID3D12Resource** upload_buffer = NULL);

template <>
struct std::hash<XMFLOAT3>
{
	size_t operator()(const XMFLOAT3& xmf3) const noexcept
	{
		auto key = xmf3.x + abs(xmf3.y * 1000.0f) + xmf3.y * 10000.0f + xmf3.z * 100000000.0f;
		return _Hash_representation<float>(key);
	}
};

template <>
struct std::equal_to<XMFLOAT3>
{
	bool operator()(const XMFLOAT3& lhs, const XMFLOAT3& rhs) const;
};

template <>
struct std::less<XMFLOAT3>
{
	bool operator()(const XMFLOAT3& lhs, const XMFLOAT3& rhs) const;
};

template <typename T> constexpr auto DegreeToRadian(T x)
{
	return float(x * PI / 180.0f);
}

inline float Clamp(float value, float min, float max)
{
	if (value < min)
	{
		return min;
	}
	else if (max < value)
	{
		return min;
	}
	return value;
}

inline float Distance(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return float(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2) + std::pow(z1 - z2, 2));
}

inline void DrawLine(HDC surface, const int x1, const int y1, const int x2, const int y2)
{
	MoveToEx(surface, x1, y1, NULL);
	LineTo(surface, x2, y2);
}

inline bool CheckProjection(const float prj_x, const float prj_y)
{
	return -1.0f <= prj_x && prj_x <= 1.0f && -1.0f <= prj_y && prj_y <= 1.0f;
}

inline bool CheckDepth(const float prj_z)
{
	return 0.0f <= prj_z && prj_z <= 1.0f;
}

struct CLocalFragment
{
	size_t from = -1, to = -1;
};

template <>
struct std::equal_to<CLocalFragment>
{
	bool operator()(const CLocalFragment& lhs, const CLocalFragment& rhs) const;
};

template <>
struct std::less<CLocalFragment>
{
	bool operator()(const CLocalFragment& lhs, const CLocalFragment& rhs) const;
};

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
