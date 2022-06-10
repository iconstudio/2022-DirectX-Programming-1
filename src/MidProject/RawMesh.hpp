#pragma once

class RawMesh
{
public:
	RawMesh();
	~RawMesh();

public:
	char m_pstrMeshName[256] = { 0 };

	UINT m_nType = 0x00;

	XMFLOAT3 m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	int m_nVertices = 0;
	XMFLOAT3* m_pxmf3Positions = NULL;
	XMFLOAT4* m_pxmf4Colors = NULL;
	XMFLOAT3* m_pxmf3Normals = NULL;

	int m_nIndices = 0;
	UINT* m_pnIndices = NULL;

	int countPolygons = 0;
	int* countPolygonIndices = NULL;
	UINT** m_ppnSubSetIndices = NULL;
};
