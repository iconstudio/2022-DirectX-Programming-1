#pragma once
#include "stdafx.hpp"

class CMesh
{
private:
	struct CLocalFragment
	{
		size_t from = 0, to = 0;
	};

public:
	CMesh();
	CMesh(const size_t number_polygons);
	virtual ~CMesh();

	void AddRef();
	void Release();

	void Assign(const CPolygon& poly);

	void Set(const size_t index, const CPolygon& poly);
	void Set(const size_t index, CPolygon&& poly);
	void Push(const CPolygon& poly);
	void Push(CPolygon&& poly);

	void Render(HDC surface) const;

	bool CheckProjection(const float prj_x, const float prj_y) const;
	bool CheckDepth(const float prj_z) const;
	BOOL RayIntersectionByTriangle(XMVECTOR& ray, XMVECTOR& ray_dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* out_distance) const;
	int CheckRayIntersection(XMVECTOR& ray, XMVECTOR& ray_dir, float* out_distance) const;

	BoundingOrientedBox Collider;

	std::vector<CPolygon> Polygons;

	std::map<size_t, XMFLOAT3> Dictionary;
	std::map<XMFLOAT3, size_t> Indexer;
	size_t lastIndex;

	std::vector<CLocalFragment> indexedFragments;

private:
	int m_nReferences = 1;
};

class CPolygon
{
public:
	CPolygon();
	CPolygon(const UINT number_vertices);
	virtual ~CPolygon();

	void Set(const UINT index, const CVertex& vertex);
	void Set(const UINT index, CVertex&& vertex);
	void Push(const CVertex& vertex);
	void Push(CVertex&& vertex);

	std::vector<CVertex> Vertices;
};

class CVertex
{
public:
	CVertex();
	CVertex(const CVertex&) = default;
	CVertex(const float x, const float y, const float z);
	CVertex(XMFLOAT3 position);
	virtual ~CVertex();

	XMFLOAT3 Position;
	XMFLOAT3 TranformedPosition;
};
