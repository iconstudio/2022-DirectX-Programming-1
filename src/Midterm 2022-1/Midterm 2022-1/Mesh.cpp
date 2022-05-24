#include "pch.h"
#include "Mesh.hpp"

CMesh::CMesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, const CMeshLoadInfo& desc)
	: d3dDevice(device), d3dCommandList(cmd_list)
	, m_nType(0)
	, m_d3dPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_nSlot(0), m_nVertices(0), m_nOffset(0)
	, m_d3dPositionBufferView()
	, m_pd3dPositionBuffer(nullptr), m_pd3dPositionUploadBuffer(nullptr)
	, m_nSubMeshes(0), m_pnSubSetIndices(nullptr)
	, m_ppd3dSubSetIndexBuffers(nullptr), m_ppd3dSubSetIndexUploadBuffers(nullptr), m_pd3dSubSetIndexBufferViews(nullptr)
{}

CMesh::~CMesh()
{}

UINT CMesh::GetType() const
{
	return m_nType;
}

void CMesh::Render()
{
	Render(d3dCommandList, 0);
}

void CMesh::Render(int sub_set)
{
	Render(d3dCommandList, sub_set);
}

void CMesh::Render(ID3D12GraphicsCommandList* cmd_list)
{
	Render(cmd_list, 0);
}

void CMesh::Render(ID3D12GraphicsCommandList* cmd_list, int sub_set)
{
	Render(cmd_list, sub_set);
}

CMeshLoadInfo::CMeshLoadInfo()
	: m_pstrMeshName()
{
	ZeroMemory(m_pstrMeshName, sizeof(m_pstrMeshName));
}

CMeshLoadInfo::~CMeshLoadInfo()
{}
