#pragma once
#include "Object.h"
#include "Mesh.h"
#include "RawMesh.hpp"
#include "Vertex.hpp"

struct TerrainData
{
	TerrainData(size_t w, size_t h, XMFLOAT3 scale);
	~TerrainData();

	void Awake(const Filepath& image);

	BYTE GetHeight(int x, int z) const;
	XMFLOAT4 GetColor(int x, int z) const;

	std::vector<std::vector<BYTE>> myHeightMap;
	std::vector<std::vector<XMFLOAT4>> myColourMap;

	const size_t myWidth;
	const size_t myHeight;
	const XMFLOAT3 m_xmf3Scale;
};

class TerrainMesh
{
public:
	TerrainMesh();
	~TerrainMesh();

	void Awake(const TerrainData& data);

	float GetPeekHeight(int x, int z) const;
	float GetHeight(float x, float z) const;

	RawMesh myRawMesh;
	CDiffusedMesh* myMesh;

	std::vector<std::vector<float>> myHeights;
};

class Terrain// : public GameObject
{
public:
	Terrain();
	~Terrain();

	void Awake(const TerrainMesh& data);
	void PrepareRendering(P3DGrpCommandList cmdlist) const;
	void Render(P3DGrpCommandList cmdlist) const;

	float GetHeight(int x, int z) const;
	float GetHeight(float x, float z) const;
	XMFLOAT4 GetColor(int x, int z) const;
	XMFLOAT3 GetNormal(int x, int z) const;
};
