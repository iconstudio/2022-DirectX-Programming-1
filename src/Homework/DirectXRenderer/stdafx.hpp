#pragma once
#pragma comment(lib, "winmm.lib")
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include <Mmsystem.h>
#include <tchar.h>
#include <stdlib.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
using namespace DirectX;
using namespace DirectX::PackedVector;

#include <memory>
#include <cmath>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <iterator>
#include <random>
using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;
using std::make_shared;
using std::make_unique;
using std::static_pointer_cast;

bool operator==(const XMFLOAT3& lhs, const XMFLOAT3& rhs);
bool operator<(const XMFLOAT3& lhs, const XMFLOAT3& rhs);

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

constexpr auto PI = 3.141592654f;
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
	XYZWrapper(float& x, float& y, float& z) : x(x), y(y), z(z) {}

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

	explicit operator XMFLOAT3() const noexcept
	{
		return std::move(XMFLOAT3(x, y, z));
	}

	float& x;
	float& y;
	float& z;
};
