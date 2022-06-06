#include "pch.hpp"
#include "Fragment.hpp"

CFragment::CFragment(const XMFLOAT3& begin, const XMFLOAT3& end, COLORREF col)
{
	start = begin;
	dest = end;
	Colour = col;
}

CFragment::CFragment(XMFLOAT3&& begin, XMFLOAT3&& end, COLORREF col)
{
	start = std::forward<XMFLOAT3>(begin);
	dest = std::forward<XMFLOAT3>(end);
	Colour = col;
}
