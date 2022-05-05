#include "stdafx.hpp"
#include "GameMesh.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"

GameMesh::GameMesh()
	: myMeshPtr()
	, myPen(NULL), myColour(RGB(255, 0, 0))
{}

GameMesh::~GameMesh()
{
	if (myPen)
	{
		DeleteObject(myPen);
	}
}

void GameMesh::SetMesh(const shared_ptr<CMesh>& mesh)
{
	myMeshPtr = mesh;
}

void GameMesh::SetColor(COLORREF color)
{
	if (myColour != color)
	{
		myColour = color;
	}
}

bool GameMesh::IsAvailable() const noexcept
{
	return bool(!myMeshPtr);
}

const shared_ptr<CMesh>& GameMesh::GetMesh() const
{
	return myMeshPtr;
}

shared_ptr<CMesh>& GameMesh::GetMesh()
{
	return myMeshPtr;
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
	if (IsAvailable())
	{
		myMeshPtr->PrepareRendering(scene, myColour);
	}
}

void GameMesh::Render(HDC surface) const
{
	if (IsAvailable())
	{
		auto old_pen = HPEN(SelectObject(surface, myPen));
		myMeshPtr->Render(surface);
		SelectObject(surface, old_pen);
	}
}

void GameMesh::RenderByFragments(HDC surface) const
{
	if (IsAvailable())
	{
		auto old_pen = HPEN(SelectObject(surface, myPen));
		myMeshPtr->RenderByFragments(surface);
		SelectObject(surface, old_pen);
	}
}
