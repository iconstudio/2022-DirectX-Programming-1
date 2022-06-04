#pragma once
#include "Vertex.hpp"

class RawMesh
{
public:
	RawMesh();
	~RawMesh();

	char myName[256] = { 0 };

	UINT m_nType = 0x00;

	XMFLOAT3 m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	int numberVertices = 0;
	XMFLOAT3* m_pxmf3Positions = NULL;
	XMFLOAT4* m_pxmf4Colors = NULL;
	XMFLOAT3* m_pxmf3Normals = NULL;

	int m_nIndices = 0;
	UINT* m_pnIndices = NULL;

	int m_nMaterials = 0;
	RawMaterial** m_pMaterials = NULL;

	int numberSubMeshes = 0;
	int* numberSubMeshesIndexes = NULL;
	UINT** indexSubMeshes = NULL;
};

class Mesh
{
public:
	Mesh(P3DDevice device, P3DGrpCommandList cmd_list, RawMesh* pMeshInfo);
	virtual ~Mesh();

	virtual void Build();
	virtual void Cleanup();

	virtual void PrepareRendering(P3DGrpCommandList cmdlist);
	virtual void Render(P3DGrpCommandList cmdlist);
	virtual void Render(P3DGrpCommandList cmd_list, int nSubSet);

	D3D12_PRIMITIVE_TOPOLOGY typePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT numberVertices = 0;
	UINT m_nOffset = 0;

	UINT m_nType = 0;

	std::vector<Material*> myMaterials;

protected:
	ID3D12Resource* m_pd3dPositionBuffer;
	ID3D12Resource* m_pd3dPositionUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_d3dPositionBufferView;

	int numberSubMeshes;
	int* numberSubMeshesIndexes;

	ID3D12Resource** m_ppd3dSubSetIndexBuffers;
	ID3D12Resource** m_ppd3dSubSetIndexUploadBuffers;
	D3D12_INDEX_BUFFER_VIEW* m_pd3dSubSetIndexBufferViews;

	ID3D12Resource* m_pd3dNormalBuffer;
	ID3D12Resource* m_pd3dNormalUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_d3dNormalBufferView;
};

