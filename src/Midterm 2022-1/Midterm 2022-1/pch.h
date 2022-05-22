﻿// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H
#include "stdafx.hpp"

class Framework;
class Timer;
class Scene;
class Camera;
class GameObject;
class GameEntity;

class Shader;
class Material;
class Model;
class Mesh;

class Car;
class Player;
class Enemy;

class Obstacle;
class Tree;
class Rock;

constexpr unsigned int sizeFrameBufferW = 800;
constexpr unsigned int sizeFrameBufferH = 600;
constexpr unsigned int numberFrameBuffers = 2;

constexpr FLOAT FPS_LIMIT = 100.0f;
constexpr COLORREF C_WHITE = RGB(0xff, 0xff, 0xff);
constexpr COLORREF C_SMOG = RGB(0xe6, 0xe6, 0xe6);
constexpr COLORREF C_GREEN = RGB(0x00, 0x80, 0x00);

constexpr int DIR_FORWARD = 0x01;
constexpr int DIR_BACKWARD = 0x02;
constexpr int DIR_LEFT = 0x04;
constexpr int DIR_RIGHT = 0x08;
constexpr int DIR_UP = 0x10;
constexpr int DIR_DOWN = 0x20;

#endif //PCH_H
