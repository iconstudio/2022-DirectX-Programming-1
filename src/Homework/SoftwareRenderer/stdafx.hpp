#pragma once
#include "pch.hpp"

constexpr FLOAT FPS_LIMIT = 100.0f;
constexpr COLORREF C_WHITE = RGB(0xff, 0xff, 0xff);
constexpr COLORREF C_GREEN = RGB(0x00, 0x80, 0x00);
constexpr COLORREF FRAMEBUFFER_CLS_COLOR = C_GREEN;

// 게임 월드의 가로 너비 (x)
constexpr float WORLD_H = 600.0f;
// 게임 월드의 높이 (y)
constexpr float WORLD_V = 20.0f;
// 게임 월드의 세로 너비 (z)
constexpr float WORLD_U = 2000.0f;

#define FRAMEBUFFER_WIDTH		800
#define FRAMEBUFFER_HEIGHT		600

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#define EXPLOSION_DEBRISES		100
#define RANDOM_COLOR			(0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX))
#define EPSILON					1.0e-7f

inline bool CheckProjection(const float prj_x, const float prj_y)
{
	return -1.0f <= prj_x && prj_x <= 1.0f && -1.0f <= prj_y && prj_y <= 1.0f;
}

inline bool CheckDepth(const float prj_z)
{
	return 0.0f <= prj_z && prj_z <= 1.0f;
}

class CMesh;
class CPolygon;
class CVertex;
class CFragment;

class GameTimer;
class GameFramework;
class GameScene;

class GamePipeline;
class GameCamera;
class GameViewport;

class GameTransform;
class GameMesh;
class GameEntity;
class GameStaticObject;
class GameObject;

class Player;
class RidingPlayer;
class PlayerBullet;
class Enemy;
class EnemyCube;
class EnemyManta;
class EnemyBullet;

class Pillar;
class Rail;
class RailBorder;

enum class ENEMY_TYPES
{
	CUBE = 0,
	MANTA
};

struct CLocalFragment
{
	size_t from = -1, to = -1;
};

template <>
struct std::equal_to<CLocalFragment>
{
	bool operator()(const CLocalFragment& lhs, const CLocalFragment& rhs) const;
};

template <>
struct std::less<CLocalFragment>
{
	bool operator()(const CLocalFragment& lhs, const CLocalFragment& rhs) const;
};

class XYZWrapper
{
public:
	XYZWrapper(float& x, float& y, float& z) : x(x), y(y), z(z) {}

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

	explicit operator XMFLOAT3() const noexcept
	{
		return std::move(XMFLOAT3(x, y, z));
	}

	float& x;
	float& y;
	float& z;
};
