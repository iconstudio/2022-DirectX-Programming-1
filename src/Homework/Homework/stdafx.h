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

#include <vector>
#include <algorithm>
#include <memory>
#include <random>

using namespace DirectX;
using namespace DirectX::PackedVector;

#define FRAMEBUFFER_WIDTH		640
#define FRAMEBUFFER_HEIGHT		480

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#define EXPLOSION_DEBRISES		30
#define RANDOM_COLOR			(0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX))
#define DegreeToRadian(x)		float((x)*3.141592654f/180.0f)
#define EPSILON					1.0e-6f

inline bool IsZero(const float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(const float fA, const float fB) { return(::IsZero(fA - fB)); }
inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}

class CMesh;
class CPolygon;
class CVertex;
class GameFramework;
class GamePipeline;
class GameCamera;
class GameViewport;
class GameWorld;
class GameObject;
class Player;

void Draw2DLine(HDC hDCFrameBuffer, XMFLOAT3& f3PreviousProject, XMFLOAT3& f3CurrentProject);

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

	XMFLOAT4X4&& RotationYawPitchRoll(float fPitch, float fYaw, float fRoll);
	XMFLOAT4X4&& RotationAxis(XMFLOAT3 xmf3Axis, float fAngle);

	XMFLOAT4X4&& PerspectiveFovLH(float fFovAngleY, float fAspectRatio, float fNearZ, float fFarZ);

	XMFLOAT4X4&& LookAtLH(const XMFLOAT3 eye_pos, const XMFLOAT3 look_pos, const XMFLOAT3 up_dir);
	XMFLOAT4X4&& LookToLH(const XMFLOAT3 eye_pos, const XMFLOAT3 look_pos, const XMFLOAT3 up_dir);
}

namespace Triangle
{
	inline bool Intersect(XMFLOAT3 xmf3RayPosition, XMFLOAT3 xmf3RayDirection, XMFLOAT3 v0, XMFLOAT3 v1, XMFLOAT3 v2, FLOAT& fHitDistance)
	{
		auto ray = XMLoadFloat3(&xmf3RayPosition);
		auto ray_dir = XMLoadFloat3(&xmf3RayDirection);
		auto p0 = XMLoadFloat3(&v0);
		auto p1 = XMLoadFloat3(&v1);
		auto p2 = XMLoadFloat3(&v2);

		return TriangleTests::Intersects(ray, ray_dir, p0, p1, p2, fHitDistance);
	}
}

namespace Plane
{
	XMFLOAT4&& Normalize(const XMFLOAT4& xmf4Plane);
	XMFLOAT4&& Normalize(XMFLOAT4&& xmf4Plane);
}
