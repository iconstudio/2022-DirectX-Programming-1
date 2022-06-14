#pragma once
#include "Object.h"
#include "Mesh.h"
#include "RawMesh.hpp"

class Terrain : public GameObject
{

};

class CTerrainMesh : public CDiffusedMesh
{
public:

};

struct TerrainData
{
	TerrainData(const Filepath& image, int w, int size, XMFLOAT3 scale);
	~TerrainData();

	RawMesh Build() const;

	BYTE* m_pHeightMapPixels;

	int m_nWidth;
	int m_nLength;
	XMFLOAT3 m_xmf3Scale;


};
