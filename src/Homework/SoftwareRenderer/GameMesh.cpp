#include "stdafx.hpp"
#include "GameMesh.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"

GameMesh::GameMesh()
	: myMeshPtr(nullptr)
	, myPen(NULL), myColour(RGB(255, 0, 0))
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
		myMeshPtr->PrepareRendering(scene, myColour);
	}
}

void GameMesh::Render(HDC surface) const
{
	if (myMeshPtr)
	{
		auto old_pen = HPEN(SelectObject(surface, myPen));
		myMeshPtr->Render(surface);
		SelectObject(surface, old_pen);
	}
}

void GameMesh::RenderByFragments(HDC surface) const
{
	if (myMeshPtr)
	{
		auto old_pen = HPEN(SelectObject(surface, myPen));
		myMeshPtr->RenderByFragments(surface);
		SelectObject(surface, old_pen);
	}
}
