#include "stdafx.hpp"
#include "Mesh.hpp"
#include "GamePipeline.hpp"

CMesh::CMesh()
	: Collider()
{}

CMesh::CMesh(int number_polygons)
	: Collider(), Polygons(number_polygons)
{}

CMesh::~CMesh()
{}

void CMesh::AddRef()
{
	m_nReferences++;
}

void CMesh::Release()
{
	m_nReferences--;
	if (m_nReferences <= 0)
	{
		delete this;
	}
}

void CMesh::Set(const UINT index, const CPolygon& poly)
{
	Polygons[index] = poly;
}

void CMesh::Set(const UINT index, CPolygon&& poly)
{
	Polygons[index] = std::move(poly);
}

void CMesh::Push(const CPolygon& poly)
{
	Push(std::move(CPolygon(poly)));
}

void CMesh::Push(CPolygon&& poly)
{
	Polygons.push_back(std::move(poly));
}

BOOL CMesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance)
{
	float fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);

	if (bIntersected && (fHitDistance < *pfNearHitDistance))
	{
		*pfNearHitDistance = fHitDistance;
	}

	return bIntersected;
}

int CMesh::CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance)
{
	int nIntersections = 0;
	bool bIntersected = Collider.Intersects(xmvPickRayOrigin, xmvPickRayDirection, *pfNearHitDistance);

	if (bIntersected)
	{
		for (auto& polygon : Polygons)
		{
			auto& vertices = polygon.Vertices;
			auto vertex_number = vertices.size();

			const auto& vertex_0 = vertices[0].TranformedPosition;
			const auto& vertex_1 = vertices[1].TranformedPosition;
			const auto& vertex_2 = vertices[2].TranformedPosition;

			auto v0 = XMLoadFloat3(&vertex_0);
			auto v1 = XMLoadFloat3(&vertex_1);
			auto v2 = XMLoadFloat3(&vertex_2);

			switch (vertex_number)
			{
				case 3:
				{
					BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
					
					if (bIntersected)
					{
						nIntersections++;
					}
				}
				break;

				case 4:
				{
					BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
					
					if (bIntersected)
					{
						nIntersections++;
					}

					const auto& vertex_3 = vertices[3].TranformedPosition;
					auto v3 = XMLoadFloat3(&(vertices[3].TranformedPosition));

					bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v2, v3, pfNearHitDistance);
					
					if (bIntersected)
					{
						nIntersections++;
					}
				}
				break;
			}
		}
	}
	return nIntersections;
}

void CMesh::Render(HDC hDCFrameBuffer)
{
	XMFLOAT3 f3InitialProject, f3PreviousProject;
	bool bPreviousInside = false, bInitialInside = false, bCurrentInside = false, bIntersectInside = false;

	for (auto& polygon : Polygons)
	{
		auto& vertices = polygon.Vertices;

		f3InitialProject = GamePipeline::Project(vertices[0].Position);

		f3PreviousProject = f3InitialProject;

		bInitialInside = (-1.0f <= f3InitialProject.x) && (f3InitialProject.x <= 1.0f) && (-1.0f <= f3InitialProject.y) && (f3InitialProject.y <= 1.0f);
		bPreviousInside = bInitialInside;
		
		for (auto& vertex : vertices)
		{
			auto f3CurrentProject = GamePipeline::Project(vertex.Position);

			vertex.TranformedPosition = f3CurrentProject;

			bCurrentInside = (-1.0f <= f3CurrentProject.x) && (f3CurrentProject.x <= 1.0f) && (-1.0f <= f3CurrentProject.y) && (f3CurrentProject.y <= 1.0f);

			if (((0.0f <= f3CurrentProject.z) && (f3CurrentProject.z <= 1.0f)) && ((bCurrentInside || bPreviousInside)))
			{
				::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
			}

			f3PreviousProject = f3CurrentProject;
			bPreviousInside = bCurrentInside;
		}

		if (0.0f <= f3InitialProject.z && f3InitialProject.z <= 1.0f
			&& (bInitialInside || bPreviousInside))
		{

			::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3InitialProject);
		}
	}
}

CPolygon::CPolygon()
	: Vertices()
{}

CPolygon::CPolygon(const UINT number_vertices)
	: Vertices(number_vertices)
{}

CPolygon::~CPolygon()
{}

void CPolygon::Set(const UINT index, const CVertex& vertex)
{
	Vertices[index] = vertex;
}

void CPolygon::Set(const UINT index, CVertex&& vertex)
{
	Vertices[index] = std::move(vertex);
}

void CPolygon::Push(const CVertex& vertex)
{
	Push(std::move(CVertex(vertex)));
}

void CPolygon::Push(CVertex&& vertex)
{
	Vertices.push_back(std::move(vertex));
}

CVertex::CVertex()
	: Position(), TranformedPosition()
{}

CVertex::CVertex(const float x, const float y, const float z)
	: Position(x, y, z), TranformedPosition(x, y, z)
{}

CVertex::CVertex(XMFLOAT3 position)
	: Position(position), TranformedPosition(position)
{}

CVertex::~CVertex()
{}
