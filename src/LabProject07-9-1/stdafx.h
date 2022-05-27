#pragma once
#include "targetver.h"

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
using ShaderBlob = D3D12_SHADER_BYTECODE;
using PtrDevice = ID3D12Device*;
using PtrCommandList = ID3D12CommandList*;
using PtrGrpCommandList = ID3D12GraphicsCommandList*;
using PtrCommandQueue = ID3D12CommandQueue*;

#include <wrl.h>
#include <shellapi.h>
#include <Mmsystem.h>
using Microsoft::WRL::ComPtr;

#include <memory>
#include <concepts>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <filesystem>
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;
using std::make_pair;
using Filepath = std::filesystem::path;

#include "Arithmetics.hpp"

int ReadIntegerFromFile(std::ifstream& stream);
float ReadFloatFromFile(std::ifstream& stream);
BYTE ReadStringFromFile(std::ifstream& stream, char* token);

int ReadIntegerFromFile(FILE* pInFile);
float ReadFloatFromFile(FILE* pInFile);
BYTE ReadStringFromFile(FILE* pInFile, char* token);

DESC_HANDLE operator+(const DESC_HANDLE& handle, const size_t increment);
DESC_HANDLE operator+(DESC_HANDLE&& handle, const size_t increment);
DESC_HANDLE& operator+=(DESC_HANDLE& handle, const size_t increment);

#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

#define EPSILON 1.0e-10f

inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(float* pfS, float* pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }
