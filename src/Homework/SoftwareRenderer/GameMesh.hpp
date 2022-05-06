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

	void SetMesh(const shared_ptr<CMesh>& pMesh);
	void SetColor(COLORREF dwColor);

	bool IsAvailable() const noexcept;

	const shared_ptr<const CMesh>& GetMesh() const;
	shared_ptr<CMesh>& GetMesh();
	std::size_t GetPolygonsNumber() const;
	const BoundingOrientedBox& GetCollider() const;
	BoundingOrientedBox& GetCollider();

	void PrepareRendering(GameScene& scene) const;
	void Render(HDC surface) const;
	void RenderByFragments(HDC surface) const;

	shared_ptr<CMesh> myMeshPtr;

	COLORREF myColour;
	HPEN myPen;
};
