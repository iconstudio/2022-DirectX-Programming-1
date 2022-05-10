#include "stdafx.hpp"

std::equal_to<CLocalFragment> lfrag_eq{};
std::less<CLocalFragment> lfrag_less{};

bool std::equal_to<CLocalFragment>::operator()(const CLocalFragment& lhs, const CLocalFragment& rhs) const
{
	return (lhs.from == rhs.from && lhs.to == rhs.to)
		|| (lhs.to == rhs.from && lhs.from == rhs.to);
}

bool std::less<CLocalFragment>::operator()(const CLocalFragment& lhs, const CLocalFragment& rhs) const
{
	return !lfrag_eq(lhs, rhs) && (&lhs < &rhs);
}
