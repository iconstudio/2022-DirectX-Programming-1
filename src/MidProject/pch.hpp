#pragma once
#ifndef __PCH__
#define __PCH__
#include "stdafx.h"

class GameFramework;
class GraphicsPipeline;

class Scene;
class IlluminatedScene;

class CMesh;
class CMaterialMesh;
class CLightenMesh;
class CMaterial;
class Pipeline;
class CLight;

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

#endif
