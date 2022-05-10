#include "stdafx.hpp"
#include "Mesh.hpp"
#include "GameScene.hpp"
#include "GamePipeline.hpp"
#include "Fragment.hpp"

CMesh::CMesh()
	: CMesh(0)
{
	Polygons.clear();
}

CMesh::CMesh(const size_t number_polygons)
	: Collider()
	, Polygons(number_polygons)
	, localIndex(), lastFound(0), Dictionary()
	, localIndexed()
	, localFragments()
{
	Polygons.reserve(number_polygons);
	Polygons.clear();
	localFragments.reserve(number_polygons);
	localFragments.clear();
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

void CMesh::BuildLocalFragment(const CPolygon& poly)
{
	const auto& Vertices = poly.Vertices;
	size_t first_id = -1;

	for (const auto& vertex : Vertices)
	{
		const auto& coord = vertex.Position;
		const auto& it = Dictionary.find(coord);
		const auto& seek = std::find_if(localIndex.cbegin(), localIndex.cend()
			, [&coord](const XMFLOAT3& other) -> bool {
			return (other == coord);
		});

		size_t index = -1;

		if (Dictionary.cend() == it)
		{
			index = lastFound;

			Dictionary.try_emplace(coord, lastFound);
			localIndex.push_back(coord);
			lastFound++;
		}
		else
		{
			index = Dictionary.at(coord);
		}
		if (-1 == first_id) first_id = index;

		TryAddFragment(index);
	}
	TryAddFragment(first_id);

	std::set<CLocalFragment> refinery{ localFragments.cbegin(), localFragments.cend() };
	volatile auto test_sz1 = localFragments.size();

	localFragments.clear();
	localFragments.assign(refinery.cbegin(), refinery.cend());

	volatile auto test_sz2 = localFragments.size();
	if (test_sz1 == test_sz2)
	{
		assert("색인 오류!");
	}

	while (!localIndexed.empty())
	{
		localIndexed.pop();
	}
}

void CMesh::BuildFragments()
{
	auto inserter = std::back_inserter(myFragments);
	std::transform(localFragments.begin(), localFragments.end(), inserter
		, [&](const CLocalFragment& it) {

		return CFragment{ localIndex.at(it.from), localIndex.at(it.to) };
	});
}

void CMesh::TryAddFragment(const size_t vertex_id)
{
	localIndexed.push(vertex_id);

	if (2 <= localIndexed.size())
	{
		const auto id_from = localIndexed.front();
		localIndexed.pop();
		const auto id_to = localIndexed.front();

		if (id_from < id_to)
			localFragments.push_back(CLocalFragment{ id_from, id_to });
		else
			localFragments.push_back(CLocalFragment{ id_to, id_from });
	}
}

void CMesh::Set(const size_t index, const CPolygon& poly)
{
	Polygons[index] = poly;
}

void CMesh::Set(const size_t index, CPolygon&& poly)
{
	BuildLocalFragment(poly);

	Polygons[index] = std::move(poly);
}

void CMesh::Push(const CPolygon& poly)
{
	Push(std::move(CPolygon(poly)));
}

void CMesh::Push(CPolygon&& poly)
{
	BuildLocalFragment(poly);

	Polygons.push_back(std::move(poly));
}

std::size_t CMesh::GetPolygonsNumber() const
{
	return Polygons.size();
}

const BoundingOrientedBox& CMesh::GetCollider() const
{
	return Collider;
}

BoundingOrientedBox& CMesh::GetCollider()
{
	return Collider;
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

int CMesh::Raycast(XMVECTOR& ray_pos, XMVECTOR& ray_dir, float* out_distance) const
{
	int nIntersections = 0;
	bool bIntersected = Collider.Intersects(ray_pos, ray_dir, *out_distance);

	if (bIntersected)
	{
		for (auto& polygon : Polygons)
		{
			auto& vertices = polygon.Vertices;
			auto vertex_number = vertices.size();

			const auto& vertex_0 = vertices[0].Position;
			const auto& vertex_1 = vertices[1].Position;
			const auto& vertex_2 = vertices[2].Position;

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

					const auto& vertex_3 = vertices[3].Position;
					auto v3 = XMLoadFloat3(&(vertices[3].Position));

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

void CMesh::PrepareRendering(GameScene& scene, COLORREF color) const
{
	for (const auto& frag : myFragments)
	{
		const auto& start = frag.start;
		const auto& dest = frag.dest;

		const auto vtx_from = GamePipeline::WorldProjectTransform(start);
		const auto vtx_to = GamePipeline::WorldProjectTransform(dest);

		const auto inside_from = CheckProjection(vtx_from.x, vtx_from.y);
		const auto inside_to = CheckProjection(vtx_to.x, vtx_to.y);

		if (CheckDepth(vtx_from.z) && CheckDepth(vtx_to.z) && (inside_from || inside_to))
		{
			scene.AddFragment(CFragment{ vtx_from, vtx_to, color });
		}
	}
}

void CMesh::PrepareRenderingUnchecked(GameScene& scene, COLORREF color) const
{
	for (const auto& frag : myFragments)
	{
		const auto& start = frag.start;
		const auto& dest = frag.dest;

		const auto vtx_from = GamePipeline::WorldProjectTransform(start);
		const auto vtx_to = GamePipeline::WorldProjectTransform(dest);

		if (CheckDepth(vtx_from.z) && CheckDepth(vtx_to.z))
		{
			scene.AddFragment(CFragment{ vtx_from, vtx_to, color });
		}
	}
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

		const XMFLOAT3 vtx_first = GamePipeline::WorldProjectTransform(vertices.at(0).Position);
		is_inside_first = CheckProjection(vtx_first.x, vtx_first.y);

		XMFLOAT3 vtx_last = vtx_first;
		is_inside_last = is_inside_first;

		for (UINT i = 1; i < sz; ++i)
		{
			const auto& vertex = vertices.at(i);
			const XMFLOAT3 vtx_it = GamePipeline::WorldProjectTransform(vertex.Position);
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

void CMesh::RenderByFragments(HDC surface) const
{
	for (const auto& frag : myFragments)
	{
		const auto vtx_from = GamePipeline::WorldProjectTransform(frag.start);
		const auto vtx_to = GamePipeline::WorldProjectTransform(frag.dest);

		const auto inside_from = CheckProjection(vtx_from.x, vtx_from.y);
		const auto inside_to = CheckProjection(vtx_to.x, vtx_to.y);

		if (CheckDepth(vtx_from.z) && CheckDepth(vtx_to.z) && (inside_from || inside_to))
		{
			DrawSide(surface, vtx_from, vtx_to);
		}
	}
}

CPolygon::CPolygon()
	: Vertices()
{
	Vertices.clear();
}

CPolygon::CPolygon(const size_t number_vertices)
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
	: Position()
{}

CVertex::CVertex(const float x, const float y, const float z)
	: Position(x, y, z)
{}

CVertex::CVertex(XMFLOAT3 position)
	: Position(position)
{}

CVertex::~CVertex()
{}
