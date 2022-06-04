#include "pch.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include <numeric>

Mesh::Mesh(P3DDevice device, P3DGrpCommandList cmd_list, RawMesh* pMeshInfo)
	: myMaterials()
	, m_pd3dPositionBuffer(nullptr), m_pd3dPositionUploadBuffer(nullptr)
	, m_d3dPositionBufferView()
	, numberSubMeshes(0), numberSubMeshesIndexes(nullptr)
	, m_ppd3dSubSetIndexBuffers(nullptr), m_ppd3dSubSetIndexUploadBuffers(nullptr)
	, m_pd3dSubSetIndexBufferViews(nullptr)
	, m_pd3dNormalBuffer(nullptr), m_pd3dNormalUploadBuffer(nullptr)
	, m_d3dNormalBufferView()
{}

Mesh::~Mesh()
{
	myMaterials.erase(myMaterials.begin(), myMaterials.end());
}

void Mesh::Build()
{}

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
