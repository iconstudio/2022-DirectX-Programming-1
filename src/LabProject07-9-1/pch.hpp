#pragma once
#ifndef PCH
#define PCH
#include "stdafx.h"

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
class GameKinematicObject; // 정적임
class GameEntity; // 동적임
enum class COLLISION_TAGS;

class Rock;
class Tree;
class Car;
class Player;

#endif
