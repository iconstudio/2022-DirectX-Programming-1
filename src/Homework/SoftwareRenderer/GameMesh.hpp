#pragma once
#include "stdafx.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"

class GameMesh
{
public:
	GameMesh();

	void SetMesh(std::shared_ptr<CMesh>& pMesh);
	void SetColor(DWORD dwColor);

	bool Available() const noexcept;

	void PrepareRendering(GameScene& scene);

	std::shared_ptr<CMesh> myMeshPtr;
	std::vector<CFragment> myFragments;

	DWORD myColour;
	HPEN myPen;
};
