#pragma once

class CMesh
{
public:
	CMesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, const CMeshLoadInfo& desc);
	virtual ~CMesh();

	UINT GetType() const;

	virtual void ReleaseUploadBuffers();

	virtual void Render();
	virtual void Render(int sub_set);
	virtual void Render(ID3D12GraphicsCommandList* cmd_list);
	virtual void Render(ID3D12GraphicsCommandList* cmd_list, int sub_set);

protected:
	ID3D12Device* d3dDevice;
	ID3D12GraphicsCommandList* d3dCommandList;

	UINT m_nType;

	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;
	UINT m_nSlot;
	UINT m_nVertices;
	UINT m_nOffset;

	D3D12_VERTEX_BUFFER_VIEW m_d3dPositionBufferView;
	ID3D12Resource* m_pd3dPositionBuffer;
	ID3D12Resource* m_pd3dPositionUploadBuffer;

	int m_nSubMeshes;
	int* m_pnSubSetIndices;

	ID3D12Resource** m_ppd3dSubSetIndexBuffers;
	ID3D12Resource** m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW* m_pd3dSubSetIndexBufferViews = NULL;
};

class CMeshLoadInfo
{
public:
	CMeshLoadInfo();
	~CMeshLoadInfo();

	char m_pstrMeshName[256];

	UINT m_nType;

	XMFLOAT3 m_xmf3AABBCenter;
	XMFLOAT3 m_xmf3AABBExtents;

	int m_nVertices = 0;
	XMFLOAT3* m_pxmf3Positions;
	XMFLOAT4* m_pxmf4Colors;
	XMFLOAT3* m_pxmf3Normals;

	int m_nIndices = 0;
	UINT* m_pnIndices;

	int m_nSubMeshes = 0;
	int* m_pnSubSetIndices;
	UINT** m_ppnSubSetIndices;
};

class CIlluminatedMesh : public CMesh
{
public:
	CIlluminatedMesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, const CMeshLoadInfo& desc);
	virtual ~CIlluminatedMesh();

	virtual void ReleaseUploadBuffers() override;

	virtual void Render(ID3D12GraphicsCommandList* cmd_list, int sub_set) override;

protected:
	ID3D12Resource* m_pd3dNormalBuffer;
	ID3D12Resource* m_pd3dNormalUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_d3dNormalBufferView;
};
