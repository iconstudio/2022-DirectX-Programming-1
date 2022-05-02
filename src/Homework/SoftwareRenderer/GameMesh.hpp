#pragma once
#include "stdafx.hpp"
#include "Mesh.hpp"

class GameMesh
{
public:
	GameMesh();
	~GameMesh();

	void SetMesh(const std::shared_ptr<CMesh>& pMesh);
	void SetColor(DWORD dwColor);

	bool Available() const noexcept;
	std::size_t GetPolygonsNumber() const;
	BoundingOrientedBox& GetCollider();

	void PrepareRendering(GameScene& scene);
	void Render(HDC surface) const;
	void RenderByFragments(HDC surface) const;

	std::shared_ptr<CMesh> myMeshPtr;

	DWORD myColour;
	HPEN myPen;
};
