#pragma once
#include "Object.h"
#include "Mesh.h"
#include "RawMesh.hpp"

class TerrainMesh
{
public:
	TerrainMesh();
	~TerrainMesh();

	void Awake(const TerrainData& data);

	CDiffusedMesh* myRawMesh;
};

struct TerrainData
{
	TerrainData(int w, int h, XMFLOAT3 scale);
	~TerrainData();

	void Awake(const Filepath& image);

	BYTE GetHeight(int x, int z) const;

	std::vector<std::vector<BYTE>> m_pHeightMapPixels;

	const int m_nWidth;
	const int m_nLength;
	const XMFLOAT3 m_xmf3Scale;
};

class Terrain// : public GameObject
{
public:
	Terrain();
	~Terrain();

	TerrainMesh myMesh;
};
