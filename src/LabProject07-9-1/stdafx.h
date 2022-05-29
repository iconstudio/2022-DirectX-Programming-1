#pragma once
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#define WIN32_LEAN_AND_MEAN 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include <tchar.h>
#include <math.h>

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
using DESC_HANDLE = D3D12_CPU_DESCRIPTOR_HANDLE;

#include <wrl.h>
#include <shellapi.h>
#include <Mmsystem.h>
using Microsoft::WRL::ComPtr;

#include <memory>
#include <filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <array>
#include <algorithm>
using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;
using std::make_shared;
using std::make_unique;
using std::make_pair;

class GameFramework;
class Scene;
class IlluminatedScene;

class CMaterial;
class Pipeline;
class CLight;
using ShaderBlob = D3D12_SHADER_BYTECODE;
using P3DDevice = ID3D12Device*;
using P3DCommandList = ID3D12CommandList*;
using P3DGrpCommandList = ID3D12GraphicsCommandList*;
using P3DSignature = ID3D12RootSignature*;

class Transformer;
class GameCamera;
class GameObject;
enum class COLLISION_TAGS;

class Rock;
class Tree;
class Car;
class Player;

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

constexpr UINT FRAME_BUFFER_WIDTH = 800;
constexpr UINT FRAME_BUFFER_HEIGHT = 600;
constexpr float ASPECT_RATIO = (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT));

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

#define MAX_LIGHTS			16 
#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

#define VERTEXT_POSITION			0x01
#define VERTEXT_COLOR				0x02
#define VERTEXT_NORMAL				0x04

#include "Arithmetics.hpp"

//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE
//#define _WITH_CB_GAMEOBJECT_32BIT_CONSTANTS
//#define _WITH_CB_GAMEOBJECT_ROOT_DESCRIPTOR
//#define _WITH_PLAYER_TOP
#define _WITH_CB_WORLD_MATRIX_DESCRIPTOR_TABLE
#define _WITH_DEBUG_FRAME_HIERARCHY

extern UINT gnCbvSrvDescriptorIncrementSize;

int ReadIntegerFromFile(FILE* file);
float ReadFloatFromFile(FILE* file);
BYTE ReadStringFromFile(FILE* file, char* token);

extern ID3D12Resource* CreateBufferResource(P3DDevice device
	, P3DGrpCommandList cmd_list
	, void* origin_data, UINT origin_size
	, D3D12_HEAP_TYPE heap_type = D3D12_HEAP_TYPE_UPLOAD
	, D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
	, ID3D12Resource** buffer = NULL);

//extern ID3D12Resource* CreateTextureResourceFromDDSFile(P3DDevice device, P3DGrpCommandList cmd_list, wchar_t* pszFileName, ID3D12Resource** ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

DESC_HANDLE operator+(const DESC_HANDLE& handle, const size_t increment);
DESC_HANDLE operator+(DESC_HANDLE&& handle, const size_t increment);
DESC_HANDLE& operator+=(DESC_HANDLE& handle, const size_t increment);

#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

#define EPSILON 1.0e-10f

inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(float* pfS, float* pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }
