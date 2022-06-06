#pragma once

class Mesh
{
public:
	Mesh() {}
	virtual ~Mesh() {}

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void ReleaseUploadBuffers() {}

protected:
	D3D12_PRIMITIVE_TOPOLOGY		typePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							numberMyVertex = 0;
	UINT							m_nOffset = 0;

	UINT							m_nType = 0;

public:
	UINT GetType() { return(m_nType); }
	virtual void Render(P3DGrpCommandList cmdlist) {}
	virtual void Render(P3DGrpCommandList cmdlist, int nSubSet) {}
};

class RawMesh
{
public:
	RawMesh() {}
	~RawMesh();

	char m_pstrMeshName[256] = { 0 };

	UINT m_nType = 0x00;

	XMFLOAT3 m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	int numberMyVertex = 0;
	XMFLOAT3* myPositions = NULL;
	XMFLOAT4* myColours = NULL;
	XMFLOAT3* myNormals = NULL;

	int numberMyIndex = 0;

	UINT* m_pnIndices = NULL;
	int numberMyMesh = 0;
	int* m_pnSubSetIndices = NULL;
	UINT** m_ppnSubSetIndices = NULL;
};

class CIndexedMesh : public Mesh
{
public:
	CIndexedMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw);
	virtual ~CIndexedMesh();

	virtual void ReleaseUploadBuffers();

	virtual void Render(P3DGrpCommandList cmdlist, int nSubSet);

protected:
	ID3D12Resource* m_pd3dPositionBuffer = NULL;
	ID3D12Resource* m_pd3dPositionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	int								numberMyMesh = 0;
	int* m_pnSubSetIndices = NULL;

	ID3D12Resource** m_ppd3dSubSetIndexBuffers = NULL;
	ID3D12Resource** m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW* m_pd3dSubSetIndexBufferViews = NULL;
};

class CIlluminatedMesh : public CIndexedMesh
{
public:
	CIlluminatedMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw);
	virtual ~CIlluminatedMesh();

	virtual void ReleaseUploadBuffers();

protected:
	ID3D12Resource* m_pd3dNormalBuffer = NULL;
	ID3D12Resource* m_pd3dNormalUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dNormalBufferView;

public:
	virtual void Render(P3DGrpCommandList cmdlist, int nSubSet);
};
