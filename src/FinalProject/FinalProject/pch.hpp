#ifndef PCH_H
#define PCH_H
#include "stdafx.h"

class Timer;
class Transformer;

template<typename>
class WeakSingleton;
class DelegateObjet;
class PlayableObjet;
class PausableObjet;

class Framework;
class Scene;

class GraphicsCore;
class GraphicsPipeline;
class Shader;

class Material;
class RawMaterial;
class Vertex;
class DiffusedVertex;
class LightenVertex;
class Mesh;
class RawMesh;

class Model;
class MeshPoint;

class GameTerrain;
class GameObject;
class GameCamera;
struct GameCameraBlob;

class GameEntity;
class GameModel;

class Helicopter;
class Player;
class Enemy;
class PlayerMissle;
class EnemyMissile;
class ParticleObjet;
class Explosion;

constexpr long FRAME_BUFFER_W = 800L;
constexpr long FRAME_BUFFER_H = 600L;
constexpr float FRAME_BUFFER_ASPECT = float(800L) / float(600L);

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

constexpr auto MAX_LIGHTS = 16UL;

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

#define VERTEXT_POSITION			0x01
#define VERTEXT_COLOR				0x02
#define VERTEXT_NORMAL				0x04

#endif //PCH_H
