#include "stdafx.hpp"
#include "Mesh.hpp"
#include "GamePipeline.hpp"

CMesh::CMesh()
	: CMesh(0)
{
	Polygons.clear();
}

CMesh::CMesh(const size_t number_polygons)
	: Collider()
	, Polygons(number_polygons)
	, Dictionary(), Indexer(), lastIndex(0)
	, indexedFragments()
{
	Polygons.reserve(number_polygons);
	Polygons.clear();
}

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

void CMesh::Assign(const CPolygon& poly)
{
	const auto& Vertices = poly.Vertices;

	for (const auto& vertex : Vertices)
	{
		const auto& pos = vertex.Position;
		const auto& seek = Indexer.find(pos);

		if (Indexer.end() == seek)
		{
			Indexer.try_emplace(pos, lastIndex);
			Dictionary.try_emplace(lastIndex, pos);
			lastIndex++;
		}
		else
		{
			//const auto place = std::distance(indexer.cbegin(), seek);
			//const auto& pair = *seek;
			//const auto index = pair.second;

			//result.push_back(static_cast<size_t>(place));
		}
	}

}

void CMesh::Set(const size_t index, const CPolygon& poly)
{
	Polygons[index] = poly;
}

void CMesh::Set(const size_t index, CPolygon&& poly)
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

bool CMesh::CheckProjection(const float prj_x, const float prj_y) const
{
	return -1.0f <= prj_x && prj_x <= 1.0f && -1.0f <= prj_y && prj_y <= 1.0f;
}

bool CMesh::CheckDepth(const float prj_z) const
{
	return 0.0f <= prj_z && prj_z <= 1.0f;
}

BOOL CMesh::RayIntersectionByTriangle(XMVECTOR& ray_pos, XMVECTOR& ray_dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* out_distance) const
{
	float fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(ray_pos, ray_dir, v0, v1, v2, fHitDistance);

	if (bIntersected && (fHitDistance < *out_distance))
	{
		*out_distance = fHitDistance;
	}

	return bIntersected;
}

int CMesh::CheckRayIntersection(XMVECTOR& ray_pos, XMVECTOR& ray_dir, float* out_distance) const
{
	int nIntersections = 0;
	bool bIntersected = Collider.Intersects(ray_pos, ray_dir, *out_distance);

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
					BOOL bIntersected = RayIntersectionByTriangle(ray_pos, ray_dir, v0, v1, v2, out_distance);

					if (bIntersected)
					{
						nIntersections++;
					}
				}
				break;

				case 4:
				{
					BOOL bIntersected = RayIntersectionByTriangle(ray_pos, ray_dir, v0, v1, v2, out_distance);

					if (bIntersected)
					{
						nIntersections++;
					}

					const auto& vertex_3 = vertices[3].TranformedPosition;
					auto v3 = XMLoadFloat3(&(vertices[3].TranformedPosition));

					bIntersected = RayIntersectionByTriangle(ray_pos, ray_dir, v0, v2, v3, out_distance);

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

void CMesh::Render(HDC surface) const
{
	bool is_inside_first = false;
	bool is_inside_it = false;
	bool is_inside_last = false;

	for (const auto& polygon : Polygons)
	{
		const auto& vertices = polygon.Vertices;
		const auto sz = vertices.size();

		if (0 == sz) continue;

		const XMFLOAT3 vtx_first = GamePipeline::ProjectTransform(vertices.at(0).Position);
		is_inside_first = CheckProjection(vtx_first.x, vtx_first.y);

		XMFLOAT3 vtx_last = vtx_first;
		is_inside_last = is_inside_first;

		for (UINT i = 1; i < sz; ++i)
		{
			const auto& vertex = vertices.at(i);
			const XMFLOAT3 vtx_it = GamePipeline::ProjectTransform(vertex.Position);
			is_inside_it = CheckProjection(vtx_it.x, vtx_it.y);

			if (CheckDepth(vtx_it.z) && (is_inside_it || is_inside_last))
			{
				DrawSide(surface, vtx_last, vtx_it);
			}

			vtx_last = vtx_it;
			is_inside_last = is_inside_it;
		}

		if (CheckDepth(vtx_first.z) && (is_inside_first || is_inside_last))
		{
			DrawSide(surface, vtx_last, vtx_first);
		}
	}
}

CPolygon::CPolygon()
	: Vertices()
{
	Vertices.clear();
}

CPolygon::CPolygon(const UINT number_vertices)
	: Vertices(number_vertices)
{
	Vertices.reserve(number_vertices);
	Vertices.clear();
}

CPolygon::~CPolygon()
{}

void CPolygon::Set(const UINT index, const CVertex& vertex)
{
	Vertices[index] = vertex;
}

void CPolygon::Set(const UINT index, CVertex&& vertex)
{
	Vertices[index] = std::forward<CVertex>(vertex);
}

void CPolygon::Push(const CVertex& vertex)
{
	Push(std::move(CVertex(vertex)));
}

void CPolygon::Push(CVertex&& vertex)
{
	Vertices.emplace_back(vertex);
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
