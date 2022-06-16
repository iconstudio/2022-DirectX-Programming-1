#include "pch.hpp"
#include "RawMesh.hpp"

RawMesh::RawMesh()
{}

RawMesh::~RawMesh()
{
	if (m_pnIndices) delete[] m_pnIndices;
}

void RawMesh::ReservePolygons(const size_t count)
{
	myPolygons.resize(count);
}

void RawMesh::AddPolygon(const CPolygon& polygon)
{
	myPolygons.push_back(polygon);
}

void RawMesh::AddPolygon(CPolygon&& polygon)
{
	myPolygons.push_back(std::forward<CPolygon>(polygon));
}

const CPolygon& RawMesh::PolygonAt(const size_t index) const
{
	return myPolygons.at(index);
}

CPolygon& RawMesh::PolygonAt(const size_t index)
{
	return myPolygons.at(index);
}
