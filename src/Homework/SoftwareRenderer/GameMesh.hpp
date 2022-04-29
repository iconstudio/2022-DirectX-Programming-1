#pragma once
#include "stdafx.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"

class GameMesh
{
public:
	std::shared_ptr<CMesh> MeshPtr;
	std::vector<CFragment> Fragments;
};
