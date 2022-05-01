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

GameMesh::~GameMesh()
{
	if (myPen)
	{
		DeleteObject(myPen);
	}
}

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
	return myMeshPtr->GetPolygonsNumber();
}

BoundingOrientedBox& GameMesh::GetCollider()
{
	return myMeshPtr->GetCollider();
}

void GameMesh::PrepareRendering(GameScene& scene)
{
	if (myMeshPtr)
	{

	}
}

void GameMesh::Render(HDC surface) const
{
	if (myMeshPtr)
	{
		auto hOldPen = HPEN(SelectObject(surface, myPen));
		myMeshPtr->Render(surface);
		SelectObject(surface, hOldPen);
	}
}

void GameMesh::RenderByFragments(HDC surface) const
{
	if (myMeshPtr)
	{
		auto hOldPen = HPEN(SelectObject(surface, myPen));
		myMeshPtr->RenderByFragments(surface);
		SelectObject(surface, hOldPen);
	}
}
