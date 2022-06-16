#include "pch.hpp"
#include "Polygon.hpp"

CPolygon::CPolygon()
	: myIndices()
{}

CPolygon::~CPolygon()
{}

void CPolygon::Reserve(const size_t count)
{
	myIndices.reserve(count);
}

void CPolygon::Add(const UINT index)
{
	myIndices.push_back(index);
}

template<std::size_t count>
void CPolygon::Add(const UINT indices[count])
{
	myIndices.assign(std::begin(indices), std::end(indices));
}

const UINT& CPolygon::At(const size_t index) const
{
	return myIndices.at(index);
}

UINT& CPolygon::At(const size_t index)
{
	return myIndices.at(index);
}

void CPolygon::Render(P3DGrpCommandList cmdlist) const
{
	cmdlist->DrawIndexedInstanced(static_cast<UINT>(myIndices.size()), 1, 0, 0, 0);
}

const UINT* CPolygon::GetData() const
{
	return myIndices.data();
}

UINT* CPolygon::GetData()
{
	return myIndices.data();
}

std::size_t CPolygon::GetSize() const
{
	return myIndices.size();
}
