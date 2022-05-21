#pragma once
#include "targetver.h"

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
using Filepath = std::filesystem::path
