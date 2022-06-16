#pragma once
#include "Object.h"
#include "Mesh.h"
#include "RawMesh.hpp"
#include "Vertex.hpp"

struct TerrainData
{
	TerrainData(size_t w, size_t h);
	~TerrainData();

	void Awake(const Filepath& image);
	void Start(const XMFLOAT3& scale);

	float GetHeight(int x, int z) const;
	float GetRawHeight(int x, int z) const;
	float GetActualHeight(float x, float z, bool reverse) const;
	XMFLOAT4 GetColor(int x, int z) const;
	XMFLOAT3 GetNormal(int x, int z) const;

	const size_t myMapWidth;
	const size_t myMapHeight;
	XMFLOAT3 m_xmf3Scale;

	std::vector<std::vector<float>> myHeightMap;
	std::vector<std::vector<XMFLOAT4>> myColourMap;
	std::vector<std::vector<float>> myHeights;

	RawMesh myRawMesh;
};

class Terrain// : public GameObject
{
public:
	Terrain(size_t w, size_t h);
	~Terrain();

	void Awake(const Filepath& image);
	void Start(P3DDevice device, P3DGrpCommandList cmdlist, const XMFLOAT3& scale);
	void PrepareRendering(P3DGrpCommandList cmdlist) const;
	void Render(P3DGrpCommandList cmdlist) const;

	float GetHeight(int x, int z) const;
	float GetHeight(float x, float z, bool reverse) const;
	float GetRawHeight(int x, int z) const;
	XMFLOAT4 GetColor(int x, int z) const;
	XMFLOAT3 GetNormal(int x, int z) const;

	TerrainData myData;

	UINT countVertices;
	CLightenMesh* myMesh;
};
