#pragma once
#ifndef __PCH_H__
#define __PCH_H__
#include "stdafx.hpp"

constexpr FLOAT FPS_LIMIT = 100.0f;
constexpr COLORREF C_WHITE = RGB(0xff, 0xff, 0xff);
constexpr COLORREF C_SMOG = RGB(0xe6, 0xe6, 0xe6);
constexpr COLORREF C_GREEN = RGB(0x00, 0x80, 0x00);

// 게임 월드의 가로 너비 (x)
constexpr float WORLD_H = 100.0f;
// 게임 월드의 높이 (y)
constexpr float WORLD_V = 20.0f;
// 게임 월드의 세로 너비 (z)
constexpr float WORLD_U = 1450.0f;

constexpr int FRAMEBUFFER_WIDTH = 800;
constexpr int FRAMEBUFFER_HEIGHT = 600;

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#define EXPLOSION_DEBRISES 100
#define RANDOM_COLOR (0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX))
#define EPSILON 1.0e-7f

class CMesh;
class CPolygon;
class CVertex;
class CDiffusedVertex;
class CLightenVertex;
class CFragment;

class Timer;
class GameFramework;
class GameScene;
class GameRenderer;

class GameTransform;
class Transformer;
class GameCamera;
class GameViewport;
class GameObject;
class GameStaticObject;
class GameEntity;
class GameMesh;

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

enum class GOBJECT_TAGS
{
	NONE = 0,
	NORMAL,
	STATIC,
	PLAYER,
	PLAYER_BULLET,
	ENEMY,
	ENEMY_BULLET
};

enum class ENEMY_TYPES
{
	CUBE = 0,
	MANTA
};

#endif
