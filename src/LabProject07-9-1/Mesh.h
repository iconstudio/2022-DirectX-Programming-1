#pragma once

class CMesh
{
protected:
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nOffset = 0;

	D3D12_PRIMITIVE_TOPOLOGY typePrimitives = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nType = 0;

public:
	virtual void ReleaseUploadBuffers() {}

	virtual void Render(PtrGrpCommandList  pd3dCommandList) {}
	virtual void Render(PtrGrpCommandList  pd3dCommandList, int nSubSet) {}

	UINT GetType() { return m_nType; }
};

class CMeshLoadInfo
{
public:
	CMeshLoadInfo();
	~CMeshLoadInfo();

public:
	char							m_pstrMeshName[256] = { 0 };

	UINT							m_nType = 0x00;

	XMFLOAT3						m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3						m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	int								m_nVertices = 0;
	XMFLOAT3						*m_pxmf3Positions = NULL;
	XMFLOAT4						*m_pxmf4Colors = NULL;
	XMFLOAT3						*m_pxmf3Normals = NULL;

	int								m_nIndices = 0;
	UINT							*m_pnIndices = NULL;

	int								m_nSubMeshes = 0;
	int								*m_pnSubSetIndices = NULL;
	UINT							**m_ppnSubSetIndices = NULL;
};

class CMeshFromFile : public CMesh
{
public:
	CMeshFromFile(PtrDevice pd3dDevice, PtrGrpCommandList pd3dCommandList, CMeshLoadInfo *pMeshInfo);
	virtual ~CMeshFromFile();

public:
	virtual void ReleaseUploadBuffers();

protected:
	ID3D12Resource					*m_pd3dPositionBuffer = NULL;
	ID3D12Resource					*m_pd3dPositionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	int								m_nSubMeshes = 0;
	int								*m_pnSubSetIndices = NULL;

	ID3D12Resource					**m_ppd3dSubSetIndexBuffers = NULL;
	ID3D12Resource					**m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW			*m_pd3dSubSetIndexBufferViews = NULL;

public:
	virtual void Render(PtrGrpCommandList pd3dCommandList, int nSubSet);
};

class CMeshIlluminatedFromFile : public CMeshFromFile
{
public:
	CMeshIlluminatedFromFile(PtrDevice pd3dDevice, PtrGrpCommandList pd3dCommandList, CMeshLoadInfo *pMeshInfo);
	virtual ~CMeshIlluminatedFromFile();

	virtual void ReleaseUploadBuffers();

protected:
	ID3D12Resource					*m_pd3dNormalBuffer = NULL;
	ID3D12Resource					*m_pd3dNormalUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dNormalBufferView;

public:
	virtual void Render(PtrGrpCommandList pd3dCommandList, int nSubSet);
};
