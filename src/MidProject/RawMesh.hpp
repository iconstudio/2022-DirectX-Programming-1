#pragma once
#include "Vertex.hpp"
#include "Polygon.hpp"

class RawMesh
{
public:
	RawMesh();
	~RawMesh();

	void ReservePolygons(const size_t count);

	const CPolygon& PolygonAt(const size_t index) const;
	CPolygon& PolygonAt(const size_t index);

	char m_pstrMeshName[256] = { 0 };

	UINT myVertexType = 0x00;

	XMFLOAT3 m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	int countVertices = 0;
	std::vector<XMFLOAT3> myPositions;
	std::vector<XMFLOAT4> myColours;
	std::vector<XMFLOAT3> myNormals;

	int m_nIndices = 0;
	UINT* m_pnIndices = NULL;

	size_t countPolygons = 0;
	std::vector<CPolygon> myPolygons;
};

class RawDiffusedMesh : public RawMesh
{
public:

};

class RawLightenMesh : public RawMesh
{
public:

};
