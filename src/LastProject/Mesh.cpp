#include "pch.hpp"
#include "Mesh.h"

RawMesh::~RawMesh()
{
	if (myPositions) delete[] myPositions;
	if (myColours) delete[] myColours;
	if (myNormals) delete[] myNormals;

	if (m_pnIndices) delete[] m_pnIndices;

	if (m_pnSubSetIndices)
	{
		delete[] m_pnSubSetIndices;
	}

	for (int i = 0; i < numberMyMesh; i++)
	{
		if (m_ppnSubSetIndices[i])
		{
			delete[] m_ppnSubSetIndices[i];
		}
	}

	if (m_ppnSubSetIndices)
	{
		delete[] m_ppnSubSetIndices;
	}
}

CIndexedMesh::CIndexedMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw)
{
	numberMyVertex = raw->numberMyVertex;
	m_nType = raw->m_nType;

	// 서술자 & 서술자 힙이 필요없다.
	m_pd3dPositionBuffer = ::CreateBufferResource(device, cmdlist, raw->myPositions, sizeof(XMFLOAT3) * numberMyVertex, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * numberMyVertex;

	numberMyMesh = raw->numberMyMesh;
	if (numberMyMesh > 0)
	{
		m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [numberMyMesh];
		m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [numberMyMesh];
		m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[numberMyMesh];

		m_pnSubSetIndices = new int[numberMyMesh];

		for (int i = 0; i < numberMyMesh; i++)
		{
			m_pnSubSetIndices[i] = raw->m_pnSubSetIndices[i];
			m_ppd3dSubSetIndexBuffers[i] = CreateBufferResource(device, cmdlist
				, raw->m_ppnSubSetIndices[i]
				, sizeof(UINT) * m_pnSubSetIndices[i]
				, D3D12_HEAP_TYPE_DEFAULT
				, D3D12_RESOURCE_STATE_INDEX_BUFFER
				, &m_ppd3dSubSetIndexUploadBuffers[i]);

			m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
			m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
			m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * raw->m_pnSubSetIndices[i];
		}
	}
}

CIndexedMesh::~CIndexedMesh()
{
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();

	if (0 < numberMyMesh)
	{
		for (int i = 0; i < numberMyMesh; i++)
		{
			if (m_ppd3dSubSetIndexBuffers[i])
			{
				m_ppd3dSubSetIndexBuffers[i]->Release();
			}
		}

		if (m_ppd3dSubSetIndexBuffers)
		{
			delete[] m_ppd3dSubSetIndexBuffers;
		}

		if (m_pd3dSubSetIndexBufferViews)
		{
			delete[] m_pd3dSubSetIndexBufferViews;
		}

		if (m_pnSubSetIndices)
		{
			delete[] m_pnSubSetIndices;
		}
	}
}

void CIndexedMesh::ReleaseUploadBuffers()
{
	Mesh::ReleaseUploadBuffers();

	if (m_pd3dPositionUploadBuffer)
	{
		m_pd3dPositionUploadBuffer->Release();
	}
	m_pd3dPositionUploadBuffer = NULL;

	if (0 < numberMyMesh && m_ppd3dSubSetIndexUploadBuffers)
	{
		for (int i = 0; i < numberMyMesh; i++)
		{
			if (m_ppd3dSubSetIndexUploadBuffers[i])
			{
				m_ppd3dSubSetIndexUploadBuffers[i]->Release();
			}
		}

		if (m_ppd3dSubSetIndexUploadBuffers)
		{
			delete[] m_ppd3dSubSetIndexUploadBuffers;
		}
		m_ppd3dSubSetIndexUploadBuffers = NULL;
	}
}

void CIndexedMesh::Render(P3DGrpCommandList cmd_list, int nSubSet)
{
	cmd_list->IASetPrimitiveTopology(typePrimitive);
	cmd_list->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);

	if ((numberMyMesh > 0) && (nSubSet < numberMyMesh))
	{
		cmd_list->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		cmd_list->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		cmd_list->DrawInstanced(numberMyVertex, 1, m_nOffset, 0);
	}
}

CIlluminatedMesh::CIlluminatedMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw)
	: CIndexedMesh::CIndexedMesh(device, cmdlist, raw)
{
	m_pd3dNormalBuffer = CreateBufferResource(device, cmdlist
		, raw->myNormals
		, sizeof(XMFLOAT3) * numberMyVertex
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &m_pd3dNormalUploadBuffer);

	m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * numberMyVertex;
}

CIlluminatedMesh::~CIlluminatedMesh()
{
	if (m_pd3dNormalBuffer)
	{
		m_pd3dNormalBuffer->Release();
	}
}

void CIlluminatedMesh::ReleaseUploadBuffers()
{
	CIndexedMesh::ReleaseUploadBuffers();

	if (m_pd3dNormalUploadBuffer)
	{
		m_pd3dNormalUploadBuffer->Release();
	}
	m_pd3dNormalUploadBuffer = NULL;
}

void CIlluminatedMesh::Render(P3DGrpCommandList cmd_list, int nSubSet)
{
	cmd_list->IASetPrimitiveTopology(typePrimitive);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[2] = { m_d3dPositionBufferView, m_d3dNormalBufferView };
	cmd_list->IASetVertexBuffers(m_nSlot, 2, pVertexBufferViews);

	if ((numberMyMesh > 0) && (nSubSet < numberMyMesh))
	{
		cmd_list->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		cmd_list->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		cmd_list->DrawInstanced(numberMyVertex, 1, m_nOffset, 0);
	}
}
