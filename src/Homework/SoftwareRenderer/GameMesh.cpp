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

void GameMesh::SetMesh(std::shared_ptr<CMesh>& mesh)
{
	myMeshPtr = std::weak_ptr(mesh);
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
	return bool(myMeshPtr.expired());
}

std::shared_ptr<CMesh> GameMesh::GetMesh() const
{
	return myMeshPtr.lock();
}

std::size_t GameMesh::GetPolygonsNumber() const
{
	return GetMesh()->GetPolygonsNumber();
}

BoundingOrientedBox& GameMesh::GetCollider()
{
	return GetMesh()->GetCollider();
}

void GameMesh::PrepareRendering(GameScene& scene)
{
	if (IsAvailable())
	{
		GetMesh()->PrepareRendering(scene, myColour);
	}
}

void GameMesh::Render(HDC surface) const
{
	if (IsAvailable())
	{
		auto old_pen = HPEN(SelectObject(surface, myPen));
		GetMesh()->Render(surface);
		SelectObject(surface, old_pen);
	}
}

void GameMesh::RenderByFragments(HDC surface) const
{
	if (IsAvailable())
	{
		auto old_pen = HPEN(SelectObject(surface, myPen));
		myMeshPtr.lock()->RenderByFragments(surface);
		SelectObject(surface, old_pen);
	}
}
