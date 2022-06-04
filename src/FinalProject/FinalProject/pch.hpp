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
class PreMesh;
class MeshPoint;

class GameTerrain;
class GameObject;
class GameCamera;
class GameCameraBlob;

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

#endif //PCH_H
