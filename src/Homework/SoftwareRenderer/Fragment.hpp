#pragma once
#include "stdafx.hpp"
#include "GameScene.hpp"
#include "GameFramework.hpp"
#include "GamePipeline.hpp"

class CFragment
{
public:
	// ���� ��ǥ
	float x1, y1, z1, x2, y2, z2;

	// ���� ��ȯ�� ���� x, y, z ��ǥ
	float u1, v1, w1;

	// ���� ��ȯ�� ��ǥ x, y, z ��ǥ
	float u2, v2, w2;

	COLORREF Colour;
};
