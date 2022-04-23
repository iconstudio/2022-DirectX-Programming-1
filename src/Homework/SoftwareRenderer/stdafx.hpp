#pragma once
#pragma comment(lib, "winmm.lib")
#define WIN32_LEAN_AND_MEAN 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include <Mmsystem.h>
#include <tchar.h>
#include <stdlib.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <cmath>
#include <vector>
#include <algorithm>
#include <memory>

using namespace DirectX;
using namespace DirectX::PackedVector;

constexpr FLOAT FPS_LIMIT = 100.0f;
constexpr COLORREF C_WHITE = RGB(0xff, 0xff, 0xff);
constexpr COLORREF C_GREEN = RGB(0x00, 0x80, 0x00);
constexpr COLORREF FRAMEBUFFER_CLS_COLOR = C_GREEN;

// 게임 월드의 크기
constexpr UINT WORLD_H = 1000, WORLD_V = 3000, WORLD_U = 1000;

// 충돌 영역의 크기
constexpr UINT COLLIDE_AREA_H = 1000, COLLIDE_AREA_V = 3000, COLLIDE_AREA_U = 1000;

#define FRAMEBUFFER_WIDTH		800
#define FRAMEBUFFER_HEIGHT		600

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#define EXPLOSION_DEBRISES		100
#define RANDOM_COLOR			(0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX))
#define DegreeToRadian(x)		float((x)*3.141592654f/180.0f)
#define EPSILON					1.0e-6f

class CMesh;
class CPolygon;
class CVertex;
class GameFramework;
class GamePipeline;
class GameTransform;
class GameCamera;
class GameViewport;
class GameScene;
class GameCollsionGroup;
class GameObject;
class Player;

class XYZWrapper
{
public:
	XYZWrapper(float& x, float& y, float& z) : X(x), Y(y), Z(z) {}

	XYZWrapper& operator=(float list[3])
	{
		X = list[0];
		Y = list[1];
		Z = list[2];
		return *this;
	}
	XYZWrapper& operator=(const XMFLOAT3& vector)
	{
		X = vector.x;
		Y = vector.y;
		Z = vector.z;
		return *this;
	}

	explicit operator XMFLOAT3() const noexcept
	{
		return std::move(XMFLOAT3(X, Y, Z));
	}

	float& X;
	float& Y;
	float& Z;
};

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
