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

	if (countPolygonIndices) delete[] countPolygonIndices;

	for (int i = 0; i < countPolygons; i++)
	{
		if (m_ppnSubSetIndices[i])
		{
			delete[] m_ppnSubSetIndices[i];
		}
	}

	if (m_ppnSubSetIndices) delete[] m_ppnSubSetIndices;
}
