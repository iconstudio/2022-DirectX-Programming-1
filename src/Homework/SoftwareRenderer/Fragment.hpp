#pragma once
#include "stdafx.hpp"

class CFragment
{
public:
	// ���� ��ǥ
	float x1, y1, z1, x2, y2, z2;

	// ���� ��ȯ�� ���� x, y, z ��ǥ
	XMFLOAT3 start;

	// ���� ��ȯ�� ��ǥ x, y, z ��ǥ
	XMFLOAT3 dest;

	COLORREF Colour;
};
