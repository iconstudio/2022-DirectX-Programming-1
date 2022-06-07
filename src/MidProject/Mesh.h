#pragma once

class CVertex
{
public:
	XMFLOAT3 myPosition;
};

class CDiffusedVertex : public CVertex
{
public:
	XMFLOAT4 myColour;
};

class CLightenVertex : public CVertex
{
public:
	XMFLOAT3 myNormal;
};

class CPolygon
{
public:
	virtual void PrepareRender(P3DGrpCommandList cmdlist);
	virtual void Render(P3DGrpCommandList cmdlist);

	std::vector<UINT> myIndices;

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

	ID3D12Resource* m_pd3dPositionBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_d3dPositionBufferView;
	ID3D12Resource* m_pd3dPositionUploadBuffer;
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

	int m_nSubMeshes = 0;
	int* m_pnSubSetIndices = NULL;
	UINT** m_ppnSubSetIndices = NULL;
};

class COriginalMesh
{
public:
	COriginalMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* pMeshInfo);
	virtual ~COriginalMesh();

	virtual void ReleaseUploadBuffers();

	UINT GetType() { return (m_nType); }
	virtual void Render(P3DGrpCommandList cmdlist) {}
	virtual void Render(P3DGrpCommandList cmdlist, int nSubSet);

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected:
	D3D12_PRIMITIVE_TOPOLOGY typePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nVertices = 0;
	UINT							m_nOffset = 0;

	UINT							m_nType = 0;

	ID3D12Resource* m_pd3dPositionBuffer = NULL;
	ID3D12Resource* m_pd3dPositionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	int								m_nSubMeshes = 0;
	int* m_pnSubSetIndices = NULL;

	ID3D12Resource** m_ppd3dSubSetIndexBuffers = NULL;
	ID3D12Resource** m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW* m_pd3dSubSetIndexBufferViews = NULL;

};

class CLightenMesh : public COriginalMesh
{
public:
	CLightenMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* pMeshInfo);
	virtual ~CLightenMesh();

	virtual void ReleaseUploadBuffers();

protected:
	ID3D12Resource* m_pd3dNormalBuffer = NULL;
	ID3D12Resource* m_pd3dNormalUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dNormalBufferView;

public:
	virtual void Render(P3DGrpCommandList cmdlist, int nSubSet);
};
