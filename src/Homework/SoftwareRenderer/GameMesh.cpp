#include "stdafx.hpp"
#include "GameMesh.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"
#include <map>
#include <set>

bool operator==(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

bool operator<(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return Vector3::Length(lhs) < Vector3::Length(rhs);
}

bool operator<=(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return (lhs < rhs) || (lhs != rhs);
}

GameMesh::GameMesh()
	: myMeshPtr(nullptr), myFragments()
	, myColour(RGB(255, 0, 0)), myPen(CreatePen(PS_SOLID, 0, myColour))
{}

void GameMesh::SetMesh(std::shared_ptr<CMesh>& pMesh)
{
	myMeshPtr = pMesh;
	myFragments.reserve(GetPolygonsNumber());

	std::vector<CLocalFragment> smallfrags{};
	smallfrags.reserve(GetPolygonsNumber() * 3);

	std::vector<XMFLOAT3> indexer{};
	std::vector<size_t> result{};
	
	const auto& polygons = pMesh->Polygons;
	for (const auto& polygon : polygons)
	{
		const auto& Vertices = polygon.Vertices;

		for (const auto& vertex : Vertices)
		{
			const auto& pos = vertex.Position;
			const auto& seek = std::find(indexer.cbegin(), indexer.cend(), pos);
			const auto index = indexer.size();

			if (indexer.cend() == seek)
			{
				result.push_back(index);

				indexer.push_back(pos);
			}
			else
			{
				const auto place = std::distance(indexer.cbegin(), seek);
				//const auto& pair = *seek;
				//const auto index = pair.second;

				result.push_back(static_cast<size_t>(place));
			}
		}
	}

	
}

void GameMesh::SetColor(DWORD dwColor)
{
	if (myColour != dwColor)
	{
		DeleteObject(myPen);
		myPen = CreatePen(PS_SOLID, 0, dwColor);
		myColour = dwColor;
	}
}

bool GameMesh::Available() const noexcept
{
	return bool(myMeshPtr);
}

std::size_t GameMesh::GetPolygonsNumber() const
{
	return myMeshPtr ? myMeshPtr->Polygons.size() : 0;
}

void GameMesh::PrepareRendering(GameScene& scene)
{

}

void GameMesh::Render(HDC surface) const
{
	auto hOldPen = HPEN(SelectObject(surface, myPen));
	myMeshPtr->Render(surface);
	SelectObject(surface, hOldPen);
}
