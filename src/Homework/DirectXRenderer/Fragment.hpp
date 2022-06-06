#pragma once

class CFragment
{
public:
	CFragment() = default;
	CFragment(const CFragment&) = default;
	CFragment& operator=(const CFragment&) = default;
	CFragment(CFragment&&) = default;
	CFragment& operator=(CFragment&&) = default;

	CFragment(const XMFLOAT3& begin, const XMFLOAT3& end, COLORREF col = 0);
	CFragment(XMFLOAT3&& begin, XMFLOAT3&& end, COLORREF col = 0);

	// ���� x, y, z ��ǥ
	XMFLOAT3 start;

	// ��ǥ x, y, z ��ǥ
	XMFLOAT3 dest;

	COLORREF Colour;
};
