#pragma once
#include "Material.hpp"
#include "Vertex.hpp"
#include "RawMesh.hpp"

class CMesh
{
public:
	CMesh();
	CMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw);
	virtual ~CMesh();

	virtual void ReleaseUploadBuffers();

	const CPolygon& PolygonAt(const size_t index) const;
	CPolygon& PolygonAt(const size_t index);

	virtual void PrepareRender(P3DGrpCommandList cmdlist) const;
	virtual void Render(P3DGrpCommandList cmdlist) const;
	virtual void Render(P3DGrpCommandList cmdlist, int polygon_index) const;

	UINT GetType() const;

protected:
	UINT myVertexType;
	D3D12_PRIMITIVE_TOPOLOGY typePrimitive;
	UINT m_nSlot;
	UINT m_nOffset;

	UINT countVertices;
	std::vector<CVertex*> myVertices;

	size_t countPolygons;
	std::vector<CPolygon> myPolygons;

	ID3D12Resource* myPositionBuffer;
	D3D12_VERTEX_BUFFER_VIEW myPositionBufferView;
	ID3D12Resource* myUploadingPositonBuffer;

	ID3D12Resource** myIndexBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW* myIndexBufferViews = NULL;
	ID3D12Resource** myUploadingIndexBuffer = NULL;

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
};

class CDiffusedMesh : public CMesh
{
public:
	CDiffusedMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw);
	~CDiffusedMesh();

	void ReleaseUploadBuffers() override;

	void PrepareRender(P3DGrpCommandList cmdlist) const override;

protected:
	ID3D12Resource* myColourBuffer;
	D3D12_VERTEX_BUFFER_VIEW myColourBufferView;
	ID3D12Resource* myUploadingColourBuffer;
};

class CMaterialMesh : public CMesh
{
public:
	CMaterialMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw);
	virtual ~CMaterialMesh();

	void AssignShader(int mat_index, Pipeline* pipeline);
	void AssignMaterial(const std::vector<CMaterial*>& list);
	void AssignMaterial(std::vector<CMaterial*>&& list);
	void AssignMaterial(std::vector<RawMaterial*> list, Pipeline* pipeline);
	void SetMaterial(int mat_index, CMaterial* material);

	virtual void Render(P3DGrpCommandList cmdlist) const override;

	shared_ptr<CMaterial> myDefaultMaterial;
	std::vector<CMaterial*> myMaterials;
};

class CLightenMesh : public CMaterialMesh
{
public:
	CLightenMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* pMeshInfo);
	~CLightenMesh();

	void ReleaseUploadBuffers() override;

	void PrepareRender(P3DGrpCommandList cmdlist) const override;
	void Render(P3DGrpCommandList cmdlist, int polygon_index) const override;

protected:
	ID3D12Resource* myNormalBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW myNormalBufferView;
	ID3D12Resource* myUploadingNormalBuffer = NULL;
};
