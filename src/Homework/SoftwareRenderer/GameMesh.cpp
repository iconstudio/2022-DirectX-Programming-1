#include "stdafx.hpp"
#include "GameMesh.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"

GameMesh::GameMesh()
	: myMeshPtr(nullptr), myFragments()
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

void GameMesh::PrepareRendering(GameScene& scene)
{

}
