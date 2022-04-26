#pragma once
#include "stdafx.hpp"
#include "GameScene.hpp"
#include "GameFramework.hpp"
#include "GamePipeline.hpp"

class CFragment
{
public:
	// 월드 좌표
	float x1, y1, z1, x2, y2, z2;

	// 투영 변환된 시작 x, y, z 좌표
	float u1, v1, w1;

	// 투영 변환된 목표 x, y, z 좌표
	float u2, v2, w2;

	COLORREF Colour;
};
