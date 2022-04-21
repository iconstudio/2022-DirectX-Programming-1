#pragma once
#include "stdafx.hpp"

class CMesh
{
public:
	CMesh();
	CMesh(int number_polygons);
	virtual ~CMesh();

	void AddRef();
	void Release();

	void Set(const UINT index, const CPolygon& poly);
	void Set(const UINT index, CPolygon&& poly);
	void Push(const CPolygon& poly);
	void Push(CPolygon&& poly);
	virtual void Render(HDC hDCFrameBuffer);

	BOOL RayIntersectionByTriangle(XMVECTOR& ray, XMVECTOR& ray_dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* out_near);
	int CheckRayIntersection(XMVECTOR& ray, XMVECTOR& ray_dir, float* out_near);

	BoundingOrientedBox m_xmOOBB;

protected:
	std::vector<CPolygon> Polygons;

private:
	int m_nReferences = 1;
};

class CPolygon
{
public:
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
	CVertex(const float x, const float y, const float z);
	CVertex(XMFLOAT3 position);
	virtual ~CVertex();

	XMFLOAT3 Position;
	XMFLOAT3 TranformedPosition;
};
