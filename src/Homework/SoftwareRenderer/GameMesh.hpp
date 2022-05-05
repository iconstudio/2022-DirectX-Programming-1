#pragma once
#include "stdafx.hpp"
#include "Mesh.hpp"

class GameMesh
{
public:
	GameMesh();
	GameMesh(const GameMesh&) = default;
	GameMesh(GameMesh&&) = default;
	~GameMesh();

	void SetMesh(std::shared_ptr<CMesh>& pMesh);
	void SetColor(COLORREF dwColor);

	bool IsAvailable() const noexcept;

	std::shared_ptr<CMesh> GetMesh() const;
	std::size_t GetPolygonsNumber() const;
	BoundingOrientedBox& GetCollider();

	void PrepareRendering(GameScene& scene);
	void Render(HDC surface) const;
	void RenderByFragments(HDC surface) const;

	std::weak_ptr<CMesh> myMeshPtr;

	COLORREF myColour;
	HPEN myPen;
};
