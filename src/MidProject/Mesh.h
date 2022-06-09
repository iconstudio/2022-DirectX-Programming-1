#pragma once
#include "Material.hpp"
#include "Vertex.hpp"

class CPolygon
{
public:
	virtual void PrepareRender(P3DGrpCommandList cmdlist);
	virtual void Render(P3DGrpCommandList cmdlist);

	std::vector<UINT> myPolygonIndices;

	ID3D12Resource* m_ppd3dSubSetIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_pd3dSubSetIndexBufferView;
	ID3D12Resource* m_ppd3dSubSetIndexUploadBuffer;
};

class CDiffusedPolygon : public CPolygon
{
public:
};

class CLightenPolygon : public CPolygon
{
public:
};

class CMesh
{
public:
	CMesh() {}
	virtual ~CMesh() {}

	virtual void Awake() = 0;
	virtual void Cleanup() = 0;
	virtual void PrepareRender(P3DGrpCommandList cmdlist) = 0;
	virtual void Render(P3DGrpCommandList cmdlist) = 0;
	virtual void Release() = 0;

protected:
	int myPipeline;

	std::vector<CVertex*> myVertices;
	std::vector<CPolygon*> mySubMeshes;

	ID3D12Resource* myPositionBuffer;
	D3D12_VERTEX_BUFFER_VIEW myPositionBufferView;
	ID3D12Resource* myUploadingPositonBuffer;
};

class CDiffusedMesh : public CMesh
{
public:
	virtual ~CDiffusedMesh() {}
};

class RawMesh
{
public:
	RawMesh() {}
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

class CMaterialMesh
{
public:
	CMaterialMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw);
	virtual ~CMaterialMesh();

	void AssignShader(int mat_index, Pipeline* pipeline);
	void AssignMaterial(const std::vector<CMaterial*>& list);
	void AssignMaterial(std::vector<CMaterial*>&& list);
	void AssignMaterial(std::vector<RawMaterial*> list, Pipeline* pipeline);
	void SetMaterial(int mat_index, CMaterial* material);

	virtual void ReleaseUploadBuffers();

	virtual void Render(P3DGrpCommandList cmdlist);
	virtual void Render(P3DGrpCommandList cmdlist, int polygon_index);

	UINT GetType() const;

	CMaterial* myDefaultMaterial;
	std::vector<CMaterial*> myMaterials;

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected:
	D3D12_PRIMITIVE_TOPOLOGY typePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nOffset = 0;
	UINT m_nType = 0;

	UINT m_nVertices = 0;
	ID3D12Resource* myPositionBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW myPositionBufferView;
	ID3D12Resource* myUploadingPositonBuffer = NULL;

	int countPolygons = 0;
	int* countPolygonIndices = NULL;

	ID3D12Resource** myIndexBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW* myIndexBufferViews = NULL;
	ID3D12Resource** myUploadingIndexBuffer = NULL;
};

class CLightenMesh : public CMaterialMesh
{
public:
	CLightenMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* pMeshInfo);
	virtual ~CLightenMesh();

	virtual void ReleaseUploadBuffers();

protected:
	ID3D12Resource* myNormalBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW myNormalBufferView;
	ID3D12Resource* myUploadingNormalBuffer = NULL;

public:
	virtual void Render(P3DGrpCommandList cmdlist, int polygon_index);
};
