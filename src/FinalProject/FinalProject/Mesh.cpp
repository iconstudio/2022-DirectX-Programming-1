#include "pch.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include <numeric>

Mesh::Mesh()
	: myMaterials()
	, mPositionBuffer(nullptr), myPositionUploadBuffer(nullptr)
	, myPositionBufferView()
	, numberSubMeshes(0), numberSubMeshesIndexes(nullptr)
	, myIndexBuffers(nullptr), myIndexUploadBuffers(nullptr)
	, myIndexBufferViews(nullptr)
	, m_pd3dNormalBuffer(nullptr), m_pd3dNormalUploadBuffer(nullptr)
	, m_d3dNormalBufferView()
{}

Mesh::~Mesh()
{
	myMaterials.erase(myMaterials.begin(), myMaterials.end());
}

void Mesh::AddMaterial(Material* mat)
{
	myMaterials.push_back(mat);
}

void Mesh::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{}

void Mesh::Awake(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw)
{
	numberVertices = raw->numberVertices;
	m_nType = raw->m_nType;

	// 서술자 & 서술자 힙이 필요없다.
	mPositionBuffer = ::CreateBufferResource(device, cmdlist, raw->m_pxmf3Positions, sizeof(XMFLOAT3) * numberVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &myPositionUploadBuffer);

	myPositionBufferView.BufferLocation = mPositionBuffer->GetGPUVirtualAddress();
	myPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	myPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * numberVertices;

	numberSubMeshes = raw->numberSubMeshes;
	if (numberSubMeshes > 0)
	{
		// 렌더링 용 업로드 버퍼
		myIndexUploadBuffers = new ID3D12Resource * [numberSubMeshes];

		myIndexBuffers = new ID3D12Resource * [numberSubMeshes];
		myIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[numberSubMeshes];

		numberSubMeshesIndexes = new int[numberSubMeshes];

		for (int i = 0; i < numberSubMeshes; i++)
		{
			const auto& submeshes_count = raw->numberSubMeshesIndexes[i];
			numberSubMeshesIndexes[i] = submeshes_count;

			auto& buffer = myIndexBuffers[i];
			auto& buffer_view = myIndexBufferViews[i];

			const auto& origin_indices = raw->indexSubMeshes[i];
			const auto& origin_indices_count = numberSubMeshesIndexes[i];

			myIndexBuffers[i] = CreateBufferResource(device, cmdlist
				, origin_indices, sizeof(UINT) * origin_indices_count
				, D3D12_HEAP_TYPE_DEFAULT
				, D3D12_RESOURCE_STATE_INDEX_BUFFER
				, &myIndexUploadBuffers[i]);

			buffer_view.BufferLocation = buffer->GetGPUVirtualAddress();
			buffer_view.Format = DXGI_FORMAT_R32_UINT;
			buffer_view.SizeInBytes = sizeof(UINT) * submeshes_count;
		}
	}
}

void Mesh::Release()
{
	if (myPositionUploadBuffer)
	{
		myPositionUploadBuffer->Release();
	}
	myPositionUploadBuffer = nullptr;

	if (0 < numberSubMeshes && myIndexUploadBuffers)
	{
		for (int i = 0; i < numberSubMeshes; i++)
		{
			if (myIndexUploadBuffers[i])
			{
				myIndexUploadBuffers[i]->Release();
			}
		}

		if (myIndexUploadBuffers)
		{
			delete[] myIndexUploadBuffers;
		}

		myIndexUploadBuffers = nullptr;
	}
}

void Mesh::PrepareRendering(P3DGrpCommandList cmdlist) const
{}

void Mesh::Render(P3DGrpCommandList cmdlist) const
{
	int mesh_index = 0;

	size_t mat_index = 0;
	const auto mat_count = myMaterials.size();
	const Material* mat = myMaterials.at(mat_index);
	while (true)
	{
		mat->PrepareRendering(cmdlist);
		
		Render(cmdlist, mesh_index);

		if (mat_index < mat_count - 1)
		{
			mat = myMaterials.at(++mat_index);
		}

		if (mesh_index < numberSubMeshes - 1)
		{
			mesh_index++;
		}
		else
		{
			break;
		}
	}

	//for (int i = 0; i < mat_count; i++)
	{
		//auto& mat = myMaterials.at(i);
		//mat->PrepareRendering(cmdlist);

		//Render(cmdlist, i);
	}
}

void Mesh::Render(P3DGrpCommandList cmdlist, int index) const
{
	cmdlist->IASetPrimitiveTopology(typePrimitive);

	// 모든 정점을 넘긴다.
	cmdlist->IASetVertexBuffers(m_nSlot, 1, &myPositionBufferView);

	if (0 < numberSubMeshes && index < numberSubMeshes)
	{
		// 하위 메쉬의 색인된 정점을 넘긴다.
		cmdlist->IASetIndexBuffer(&(myIndexBufferViews[index]));
		// 하위 메쉬를 그린다. 
		cmdlist->DrawIndexedInstanced(numberSubMeshesIndexes[index], 1
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

	if (0 < m_nMaterials)
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
