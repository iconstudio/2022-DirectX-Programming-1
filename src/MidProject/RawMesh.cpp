#include "pch.hpp"
#include "RawMesh.hpp"

RawMesh::RawMesh()
{}

RawMesh::~RawMesh()
{
	if (m_pxmf3Positions) delete[] m_pxmf3Positions;
	if (m_pxmf4Colors) delete[] m_pxmf4Colors;
	if (m_pxmf3Normals) delete[] m_pxmf3Normals;

	if (m_pnIndices) delete[] m_pnIndices;

	//if (countPolygonIndices) delete[] countPolygonIndices;

	for (int i = 0; i < countPolygons; i++)
	{
		//if (indexByPolygons[i])
		{
		//	delete[] indexByPolygons[i];
		}
	}

	//if (indexByPolygons) delete[] indexByPolygons;
}

void RawMesh::ReservePolygons(const size_t count)
{
	myPolygons.resize(count);
}

const CPolygon& RawMesh::PolygonAt(const size_t index) const
{
	return myPolygons.at(index);
}

CPolygon& RawMesh::PolygonAt(const size_t index)
{
	return myPolygons.at(index);
}
