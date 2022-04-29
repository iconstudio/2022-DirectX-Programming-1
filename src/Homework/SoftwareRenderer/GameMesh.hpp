#pragma once
#include "stdafx.hpp"
#include "Mesh.hpp"

class GameMesh
{
public:
	GameMesh();

	void SetMesh(std::shared_ptr<CMesh>& pMesh);
	void SetColor(DWORD dwColor);

	bool Available() const noexcept;
	std::size_t GetPolygonsNumber() const;

	void PrepareRendering(GameScene& scene);
	void Render(HDC surface) const;

	std::shared_ptr<CMesh> myMeshPtr;
	std::vector<CFragment> myFragments;

	DWORD myColour;
	HPEN myPen;

private:
	struct CLocalFragment
	{
		std::vector<std::size_t> smallFragments;
	};
};
