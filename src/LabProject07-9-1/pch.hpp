#pragma once

#ifndef PCH
#define PCH
#include "stdafx.h"

class GameFramework;

class TaggedObject;
class CScene;

class CMesh;
class CMeshLoadInfo;
class CMaterial;
class RawMaterial;
class CShader;
class CLight;

class GameCamera;
class GameObject;

class Car;
class Player;

//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE
#define FRAME_BUFFER_WIDTH		800
#define FRAME_BUFFER_HEIGHT		600

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

constexpr XMFLOAT4 DefaultAmient = { 0.2f, 0.2f, 0.2f, 1.0f };

//#define _WITH_CB_GAMEOBJECT_32BIT_CONSTANTS
//#define _WITH_CB_GAMEOBJECT_ROOT_DESCRIPTOR
#define _WITH_CB_WORLD_MATRIX_DESCRIPTOR_TABLE
#define _WITH_DEBUG_FRAME_HIERARCHY

extern UINT gnCbvSrvDescriptorIncrementSize;

extern CMeshLoadInfo* LoadMeshInfoFromFile(FILE* stream);

extern std::vector<RawMaterial> LoadRawMaterials(FILE* stream);

extern ID3D12Resource* CreateBufferResource(PtrDevice device
	, PtrGrpCommandList  pd3dCommandList
	, void* pData, UINT nBytes
	, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD
	, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
	, ID3D12Resource** ppd3dUploadBuffer = NULL);

extern ID3D12Resource* CreateTextureResourceFromDDSFile(PtrDevice device
	, PtrGrpCommandList  cmd_list
	, wchar_t* pszFileName
	, ID3D12Resource** ppd3dUploadBuffer
	, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

#endif
