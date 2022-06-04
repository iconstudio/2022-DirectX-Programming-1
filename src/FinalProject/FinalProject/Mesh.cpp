#include "pch.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include <numeric>

Mesh::Mesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw)
	: myMaterials()
	, m_pd3dPositionBuffer(nullptr), m_pd3dPositionUploadBuffer(nullptr)
	, m_d3dPositionBufferView()
	, numberSubMeshes(0), numberSubMeshesIndexes(nullptr)
	, m_ppd3dSubSetIndexBuffers(nullptr), m_ppd3dSubSetIndexUploadBuffers(nullptr)
	, m_pd3dSubSetIndexBufferViews(nullptr)
	, m_pd3dNormalBuffer(nullptr), m_pd3dNormalUploadBuffer(nullptr)
	, m_d3dNormalBufferView()
{
	Build(device, cmdlist, raw);
	Cleanup();
}

Mesh::~Mesh()
{
	myMaterials.erase(myMaterials.begin(), myMaterials.end());
}

void Mesh::Build(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw)
{
	numberVertices = raw->numberVertices;
	m_nType = raw->m_nType;

	// ������ & ������ ���� �ʿ����.
	m_pd3dPositionBuffer = ::CreateBufferResource(device, cmdlist, raw->m_pxmf3Positions, sizeof(XMFLOAT3) * numberVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * numberVertices;

	numberSubMeshes = raw->numberSubMeshes;
	if (numberSubMeshes > 0)
	{
		// ������ �� ���ε� ����
		m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [numberSubMeshes];

		m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [numberSubMeshes];
		m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[numberSubMeshes];

		numberSubMeshesIndexes = new int[numberSubMeshes];

		for (int i = 0; i < numberSubMeshes; i++)
		{
			const auto& submeshes_count = raw->numberSubMeshesIndexes[i];
			numberSubMeshesIndexes[i] = submeshes_count;

			auto& buffer = m_ppd3dSubSetIndexBuffers[i];
			auto& buffer_view = m_pd3dSubSetIndexBufferViews[i];

			const auto& origin_indices = raw->indexSubMeshes[i];
			const auto& origin_indices_count = numberSubMeshesIndexes[i]

			m_ppd3dSubSetIndexBuffers[i] = CreateBufferResource(device, cmdlist
				, origin_indices, sizeof(UINT) * origin_indices_count
				, D3D12_HEAP_TYPE_DEFAULT
				, D3D12_RESOURCE_STATE_INDEX_BUFFER
				, &m_ppd3dSubSetIndexUploadBuffers[i]);

			buffer_view.BufferLocation = buffer->GetGPUVirtualAddress();
			buffer_view.Format = DXGI_FORMAT_R32_UINT;
			buffer_view.SizeInBytes = sizeof(UINT) * submeshes_count;
		}
	}
}

void Mesh::Cleanup()
{}

void Mesh::PrepareRendering(P3DGrpCommandList cmdlist)
{}

void Mesh::Render(P3DGrpCommandList cmdlist)
{
	for (int i = 0; i < myMaterials.size(); i++)
	{
		auto& mat = myMaterials.at(i);
		mat->PrepareRendering(cmdlist);

		Render(cmdlist, i);
	}
}

void Mesh::Render(P3DGrpCommandList cmdlist, int nSubSet)
{
	cmdlist->IASetPrimitiveTopology(typePrimitive);

	// ��� ������ �ѱ��.
	cmdlist->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);

	if ((numberSubMeshes > 0) && (nSubSet < numberSubMeshes))
	{
		// ���� �޽��� ���ε� ������ �ѱ��.
		cmdlist->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		// ���� �޽��� �׸���. 
		cmdlist->DrawIndexedInstanced(numberSubMeshesIndexes[nSubSet], 1
			, 0 // ���ε� ����
			, 0 // ���� ��ġ ������
			, 0); // �ν��Ͻ� ������ (ID)
	}
	else
	{
		// �׳� ���� ���ۿ� �ִ� ������ ���� �׸���.
		cmdlist->DrawInstanced(numberVertices, 1, m_nOffset, 0);
	}
}

RawMesh::RawMesh()
{}

RawMesh::~RawMesh()
{
	if (m_pxmf3Positions) delete[] m_pxmf3Positions;
	if (m_pxmf4Colors) delete[] m_pxmf4Colors;
	if (m_pxmf3Normals) delete[] m_pxmf3Normals;

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_pMaterials[i])
			{
				delete m_pMaterials[i];
			}
		}
	}

	if (m_pnIndices) delete[] m_pnIndices;

	if (numberSubMeshesIndexes) delete[] numberSubMeshesIndexes;

	for (int i = 0; i < numberSubMeshes; i++)
	{
		if (indexSubMeshes[i])
		{
			delete[] indexSubMeshes[i];
		}
	}

	if (indexSubMeshes) delete[] indexSubMeshes;
}
