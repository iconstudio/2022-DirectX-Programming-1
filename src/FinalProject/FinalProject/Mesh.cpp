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

	// 모든 정점을 넘긴다.
	cmdlist->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);

	if ((numberSubMeshes > 0) && (nSubSet < numberSubMeshes))
	{
		// 하위 메쉬의 색인된 정점을 넘긴다.
		cmdlist->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		// 하위 메쉬를 그린다. 
		cmdlist->DrawIndexedInstanced(numberSubMeshesIndexes[nSubSet], 1
			, 0 // 색인된 정점
			, 0 // 정점 위치 시작점
			, 0); // 인스턴스 시작점 (ID)
	}
	else
	{
		// 그냥 정점 버퍼에 있는 정점을 따라 그린다.
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
