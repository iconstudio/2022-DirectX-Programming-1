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
using std::shared_ptr;
using std::unique_ptr;
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

namespace Vector3
{
	XMFLOAT3&& XMVectorToFloat3(const XMVECTOR& vector);
	XMFLOAT3&& XMVectorToFloat3(XMVECTOR&& vector);

	XMFLOAT3&& ScalarProduct(const XMFLOAT3& vector, float scalar, bool normalize = true);
	XMFLOAT3&& ScalarProduct(XMFLOAT3&& vector, float scalar, bool normalize = true);

	XMFLOAT3&& Add(const XMFLOAT3& vector1, const XMFLOAT3& vector2, float scalar = 1.0f);
	XMFLOAT3&& Add(XMFLOAT3&& vector1, XMFLOAT3&& vector2, float scalar = 1.0f);

	XMFLOAT3&& Subtract(const XMFLOAT3& vector1, const XMFLOAT3& vector2);
	XMFLOAT3&& Subtract(XMFLOAT3&& vector1, XMFLOAT3&& vector2);

	FLOAT DotProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2);
	FLOAT DotProduct(XMFLOAT3&& vector1, XMFLOAT3&& vector2);

	XMFLOAT3&& CrossProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2, bool normalize = true);
	XMFLOAT3&& CrossProduct(XMFLOAT3&& vector1, XMFLOAT3&& vector2, bool normalize = true);

	XMFLOAT3&& Normalize(const XMFLOAT3& vector);
	XMFLOAT3&& Normalize(XMFLOAT3&& vector);

	FLOAT Length(XMFLOAT3 vector);
	FLOAT Distance(XMFLOAT3 vector1, XMFLOAT3 vector2);
	FLOAT Angle(XMVECTOR vector1, XMVECTOR vector2);
	FLOAT Angle(XMFLOAT3 vector1, XMFLOAT3 vector2);

	XMFLOAT3&& TransformNormal(const XMFLOAT3& vector, const XMMATRIX& transform);
	XMFLOAT3&& TransformNormal(XMFLOAT3&& vector, XMMATRIX&& transform);
	XMFLOAT3&& TransformNormal(const XMFLOAT3& vector, const XMFLOAT4X4& transform);
	XMFLOAT3&& TransformNormal(XMFLOAT3&& vector, XMFLOAT4X4&& transform);

	XMFLOAT3&& TransformCoord(const XMFLOAT3& vector, const XMMATRIX& transform);
	XMFLOAT3&& TransformCoord(XMFLOAT3&& vector, XMMATRIX&& transform);
	XMFLOAT3&& TransformCoord(const XMFLOAT3& vector, const XMFLOAT4X4& transform);
	XMFLOAT3&& TransformCoord(XMFLOAT3&& vector, XMFLOAT4X4&& transform);
}

namespace Vector4
{
	XMFLOAT4&& Add(const XMFLOAT4& vector1, const XMFLOAT4& vector2);
	XMFLOAT4&& Add(XMFLOAT4&& vector1, XMFLOAT4&& vector2);
}

namespace Matrix4x4
{
	XMFLOAT4X4&& Identity();
	XMFLOAT4X4&& Translate(float x, float y, float z);

	XMFLOAT4X4&& Inverse(const XMFLOAT4X4& matrix);
	XMFLOAT4X4&& Inverse(XMFLOAT4X4&& matrix);

	XMFLOAT4X4&& Transpose(const XMFLOAT4X4& matrix);
	XMFLOAT4X4&& Transpose(XMFLOAT4X4&& matrix);

	XMFLOAT4X4&& Multiply(const XMFLOAT4X4& matrix1, const XMFLOAT4X4& matrix2);
	XMFLOAT4X4&& Multiply(XMFLOAT4X4&& matrix1, XMFLOAT4X4&& matrix2);

	XMFLOAT4X4&& Multiply(const XMFLOAT4X4& matrix1, const XMMATRIX& matrix2);
	XMFLOAT4X4&& Multiply(XMFLOAT4X4&& matrix1, XMMATRIX&& matrix2);

	XMFLOAT4X4&& Multiply(const XMMATRIX& matrix1, const XMFLOAT4X4& matrix2);
	XMFLOAT4X4&& Multiply(XMMATRIX&& matrix1, XMFLOAT4X4&& matrix2);

	XMFLOAT4X4&& Multiply(const XMMATRIX& matrix1, const XMMATRIX& matrix2);
	XMFLOAT4X4&& Multiply(XMMATRIX&& matrix1, XMMATRIX&& matrix2);

	XMFLOAT4X4&& RotationYawPitchRoll(float pitch, float yaw, float roll);
	XMFLOAT4X4&& RotationAxis(XMFLOAT3 axis, float angle);

	XMFLOAT4X4&& PerspectiveFovLH(float fov, float aspect_ratio, float near, float far);

	XMFLOAT4X4&& LookAtLH(const XMFLOAT3 eye_pos, const XMFLOAT3 look_pos, const XMFLOAT3 up_dir);
	XMFLOAT4X4&& LookToLH(const XMFLOAT3 eye_pos, const XMFLOAT3 look_pos, const XMFLOAT3 up_dir);
}

namespace Triangle
{
	bool Intersect(XMFLOAT3 ray_pos, XMFLOAT3 ray_dir, XMFLOAT3 v0, XMFLOAT3 v1, XMFLOAT3 v2, FLOAT& out_distance);
}

namespace Plane
{
	XMFLOAT4&& Normalize(const XMFLOAT4& xmf4Plane);
	XMFLOAT4&& Normalize(XMFLOAT4&& xmf4Plane);
}
