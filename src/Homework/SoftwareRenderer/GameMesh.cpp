#include "stdafx.hpp"
#include "GameMesh.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"
#include <map>
#include <set>

GameMesh::GameMesh()
	: myMeshPtr(nullptr)
	, myColour(RGB(255, 0, 0)), myPen(CreatePen(PS_SOLID, 0, myColour))
{}

void GameMesh::SetMesh(std::shared_ptr<CMesh>& pMesh)
{
	myMeshPtr = pMesh;
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
