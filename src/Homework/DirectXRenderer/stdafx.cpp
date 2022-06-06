#include "pch.hpp"
#include "stdafx.hpp"

std::equal_to<CLocalFragment> lfrag_eq{};
std::less<CLocalFragment> lfrag_less{};

std::hash<XMFLOAT3> xm3f_hasher{};

bool std::equal_to<CLocalFragment>::operator()(const CLocalFragment& lhs, const CLocalFragment& rhs) const
{
	return (lhs.from == rhs.from && lhs.to == rhs.to)
		|| (lhs.to == rhs.from && lhs.from == rhs.to);
}

bool std::less<CLocalFragment>::operator()(const CLocalFragment& lhs, const CLocalFragment& rhs) const
{
	return !lfrag_eq(lhs, rhs) && (&lhs < &rhs);
}

bool operator==(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

bool operator<(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return xm3f_hasher(lhs) < xm3f_hasher(rhs);
}

bool std::equal_to<XMFLOAT3>::operator()(const XMFLOAT3& lhs, const XMFLOAT3& rhs) const
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

bool std::less<XMFLOAT3>::operator()(const XMFLOAT3& lhs, const XMFLOAT3& rhs) const
{
	return xm3f_hasher(lhs) < xm3f_hasher(rhs);
}
