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
	XMFLOAT3* m_pxmf3Positions = NULL;
	XMFLOAT4* m_pxmf4Colors = NULL;
	XMFLOAT3* m_pxmf3Normals = NULL;

	int m_nIndices = 0;
	UINT* m_pnIndices = NULL;
	std::vector<CVertex> myVertices;

	// �ε��� ��ü�� ��
	size_t countPolygons = 0;
	// ���� �ε��� ��ü�� ���� ������ ��
	//int* countPolygonIndices = NULL;
	// ���� �ε��� ��ü�� ���� ������ �迭
	//UINT** indexByPolygons = NULL;

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
