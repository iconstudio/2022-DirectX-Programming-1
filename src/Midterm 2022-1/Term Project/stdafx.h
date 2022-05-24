#pragma once
#include "targetver.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#define WIN32_LEAN_AND_MEAN 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <D3Dcompiler.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#include <wrl.h>
#include <shellapi.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;

#include <memory>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>

using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;
using std::string;
using Filepath = std::filesystem::path;

constexpr auto PI = 3.141592654f;
template <typename T> constexpr auto DegreeToRadian(T x)
{
	return float(x * PI / 180.0f);
}

//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

#define FRAME_BUFFER_WIDTH		640
#define FRAME_BUFFER_HEIGHT		480

//#define _WITH_CB_GAMEOBJECT_32BIT_CONSTANTS
//#define _WITH_CB_GAMEOBJECT_ROOT_DESCRIPTOR
#define _WITH_CB_WORLD_MATRIX_DESCRIPTOR_TABLE


extern UINT gnCbvSrvDescriptorIncrementSize;

extern ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource** ppd3dUploadBuffer = NULL);
extern ID3D12Resource* CreateTextureResourceFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, ID3D12Resource** ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

#define RANDOM_COLOR			XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

#define EPSILON					1.0e-10f

inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(float* pfS, float* pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }
class XYZWrapper
{
public:
	XYZWrapper(float& x, float& y, float& z) : x(x), y(y), z(z) {}
	XYZWrapper(XMFLOAT3& vector) : x(vector.x), y(vector.y), z(vector.z) {}

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

namespace Vector3
{
	XMFLOAT3 XMVectorToFloat3(const XMVECTOR& vector);
	XMFLOAT3 XMVectorToFloat3(XMVECTOR&& vector);

	XMFLOAT3 ScalarProduct(const XMFLOAT3& vector, float scalar, bool normalize = true);
	XMFLOAT3 ScalarProduct(XMFLOAT3&& vector, float scalar, bool normalize = true);

	XMFLOAT3 Add(const XMFLOAT3& vector1, const XMFLOAT3& vector2, float scalar = 1.0f);
	XMFLOAT3 Add(XMFLOAT3&& vector1, XMFLOAT3&& vector2, float scalar = 1.0f);

	XMFLOAT3 Subtract(const XMFLOAT3& vector1, const XMFLOAT3& vector2);
	XMFLOAT3 Subtract(XMFLOAT3&& vector1, XMFLOAT3&& vector2);

	FLOAT DotProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2);
	FLOAT DotProduct(XMFLOAT3&& vector1, XMFLOAT3&& vector2);

	XMFLOAT3 CrossProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2, bool normalize = true);
	XMFLOAT3 CrossProduct(XMFLOAT3&& vector1, XMFLOAT3&& vector2, bool normalize = true);

	XMFLOAT3 Normalize(const XMFLOAT3& vector);
	XMFLOAT3 Normalize(XMFLOAT3&& vector);

	FLOAT Length(XMFLOAT3 vector);
	FLOAT Distance(XMFLOAT3 vector1, XMFLOAT3 vector2);
	FLOAT Angle(XMVECTOR vector1, XMVECTOR vector2);
	FLOAT Angle(XMFLOAT3 vector1, XMFLOAT3 vector2);

	XMFLOAT3 TransformNormal(const XMFLOAT3& vector, const XMMATRIX& transform);
	XMFLOAT3 TransformNormal(XMFLOAT3&& vector, XMMATRIX&& transform);
	XMFLOAT3 TransformNormal(const XMFLOAT3& vector, const XMFLOAT4X4& transform);
	XMFLOAT3 TransformNormal(XMFLOAT3&& vector, XMFLOAT4X4&& transform);

	XMFLOAT3 TransformCoord(const XMFLOAT3& vector, const XMMATRIX& transform);
	XMFLOAT3 TransformCoord(XMFLOAT3&& vector, XMMATRIX&& transform);
	XMFLOAT3 TransformCoord(const XMFLOAT3& vector, const XMFLOAT4X4& transform);
	XMFLOAT3 TransformCoord(XMFLOAT3&& vector, XMFLOAT4X4&& transform);
}

namespace Vector4
{
	XMFLOAT4 Add(const XMFLOAT4& vector1, const XMFLOAT4& vector2);
	XMFLOAT4 Add(XMFLOAT4&& vector1, XMFLOAT4&& vector2);
}

namespace Matrix4x4
{
	XMFLOAT4X4 Identity();
	XMFLOAT4X4 Translate(float x, float y, float z);

	XMFLOAT4X4 Inverse(const XMFLOAT4X4& matrix);
	XMFLOAT4X4 Inverse(XMFLOAT4X4&& matrix);

	XMFLOAT4X4 Transpose(const XMFLOAT4X4& matrix);
	XMFLOAT4X4 Transpose(XMFLOAT4X4&& matrix);

	XMFLOAT4X4 Multiply(const XMFLOAT4X4& matrix1, const XMFLOAT4X4& matrix2);
	XMFLOAT4X4 Multiply(XMFLOAT4X4&& matrix1, XMFLOAT4X4&& matrix2);

	XMFLOAT4X4 Multiply(const XMFLOAT4X4& matrix1, const XMMATRIX& matrix2);
	XMFLOAT4X4 Multiply(XMFLOAT4X4&& matrix1, XMMATRIX&& matrix2);

	XMFLOAT4X4 Multiply(const XMMATRIX& matrix1, const XMFLOAT4X4& matrix2);
	XMFLOAT4X4 Multiply(XMMATRIX&& matrix1, XMFLOAT4X4&& matrix2);

	XMFLOAT4X4 Multiply(const XMMATRIX& matrix1, const XMMATRIX& matrix2);
	XMFLOAT4X4 Multiply(XMMATRIX&& matrix1, XMMATRIX&& matrix2);

	XMFLOAT4X4 RotationYawPitchRoll(float pitch, float yaw, float roll);
	XMFLOAT4X4 RotationAxis(XMFLOAT3 axis, float angle);

	XMFLOAT4X4 PerspectiveFovLH(float fov, float aspect_ratio, float near, float far);

	XMFLOAT4X4 LookAtLH(const XMFLOAT3 eye_pos, const XMFLOAT3 look_pos, const XMFLOAT3 up_dir);
	XMFLOAT4X4 LookToLH(const XMFLOAT3 eye_pos, const XMFLOAT3 look_pos, const XMFLOAT3 up_dir);
}

namespace Triangle
{
	bool Intersect(XMFLOAT3 ray_pos, XMFLOAT3 ray_dir, XMFLOAT3 v0, XMFLOAT3 v1, XMFLOAT3 v2, FLOAT& out_distance);
}

namespace Plane
{
	XMFLOAT4 Normalize(const XMFLOAT4& xmf4Plane);
	XMFLOAT4 Normalize(XMFLOAT4&& xmf4Plane);
}
