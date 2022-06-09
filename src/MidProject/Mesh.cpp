#include "pch.hpp"
#include "Mesh.h"
#include "Material.hpp"
#include "Pipeline.hpp"

RawMesh::~RawMesh()
{
	if (m_pxmf3Positions) delete[] m_pxmf3Positions;
	if (m_pxmf4Colors) delete[] m_pxmf4Colors;
	if (m_pxmf3Normals) delete[] m_pxmf3Normals;

	if (m_pnIndices) delete[] m_pnIndices;

	if (countPolygonIndices) delete[] countPolygonIndices;

	for (int i = 0; i < countPolygons; i++) if (m_ppnSubSetIndices[i]) delete[] m_ppnSubSetIndices[i];
	if (m_ppnSubSetIndices) delete[] m_ppnSubSetIndices;
}

CMaterialMesh::CMaterialMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw)
	: myDefaultMaterial(nullptr), myMaterials()
{
	m_nVertices = raw->m_nVertices;
	m_nType = raw->m_nType;

	// 서술자 & 서술자 힙이 필요없다.
	myPositionBuffer = ::CreateBufferResource(device, cmdlist, raw->m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &myUploadingPositonBuffer);

	myPositionBufferView.BufferLocation = myPositionBuffer->GetGPUVirtualAddress();
	myPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	myPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	countPolygons = raw->countPolygons;
	if (countPolygons > 0)
	{
		myIndexBuffers = new ID3D12Resource * [countPolygons];
		myUploadingIndexBuffer = new ID3D12Resource * [countPolygons];
		myIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[countPolygons];

		countPolygonIndices = new int[countPolygons];

		for (int i = 0; i < countPolygons; i++)
		{
			countPolygonIndices[i] = raw->countPolygonIndices[i];
			myIndexBuffers[i] = ::CreateBufferResource(device, cmdlist, raw->m_ppnSubSetIndices[i], sizeof(UINT) * countPolygonIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &myUploadingIndexBuffer[i]);

			myIndexBufferViews[i].BufferLocation = myIndexBuffers[i]->GetGPUVirtualAddress();
			myIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
			myIndexBufferViews[i].SizeInBytes = sizeof(UINT) * raw->countPolygonIndices[i];
		}
	}
}

CMaterialMesh::~CMaterialMesh()
{
	if (0 < myMaterials.size())
	{
		std::for_each(myMaterials.begin(), myMaterials.end()
			, [](CMaterial* mat) {
				mat->Release();
			});
	}

	if (myDefaultMaterial)
	{
		myDefaultMaterial->Release();
	}

	if (myPositionBuffer) myPositionBuffer->Release();

	if (countPolygons > 0)
	{
		for (int i = 0; i < countPolygons; i++)
		{
			if (myIndexBuffers[i]) myIndexBuffers[i]->Release();
		}
		if (myIndexBuffers) delete[] myIndexBuffers;
		if (myIndexBufferViews) delete[] myIndexBufferViews;

		if (countPolygonIndices) delete[] countPolygonIndices;
	}
}

void CMaterialMesh::AssignShader(int mat_index, Pipeline* pipeline)
{
	if (mat_index < myMaterials.size())
	{
		auto& mat = myMaterials.at(mat_index);
		if (mat)
		{
			mat->SetShader(pipeline);
		}
		else
		{
			throw "잘못된 재질이 들어있음!";
		}
	}
	else
	{
		throw "잘못된 재질의 번호를 참조하는 중!";
	}
}

void CMaterialMesh::AssignMaterial(const std::vector<CMaterial*>& list)
{
	myMaterials = list;
}

void CMaterialMesh::AssignMaterial(std::vector<CMaterial*>&& list)
{
	myMaterials = std::forward<std::vector<CMaterial*>>(list);
}

void CMaterialMesh::AssignMaterial(std::vector<RawMaterial*> list, Pipeline* pipeline)
{
	auto inserter = std::back_inserter(myMaterials);

	std::transform(list.begin(), list.end(), inserter
		, [&](RawMaterial* raw_mat) -> CMaterial* {
			auto result = new CMaterial(raw_mat);
			result->SetShader(pipeline);
			return result;
		});
}

void CMaterialMesh::SetMaterial(int mat_index, CMaterial* material)
{
	if (mat_index < myMaterials.size())
	{
		auto& mat = myMaterials.at(mat_index);
		if (mat)
		{
			mat->Release();
		}

		mat = material;

		if (mat)
		{
			mat->AddRef();
		}
	}
	else
	{
		throw "잘못된 재질의 번호를 참조하는 중!";
	}
}

void CMaterialMesh::ReleaseUploadBuffers()
{
	if (myUploadingPositonBuffer) myUploadingPositonBuffer->Release();

	myUploadingPositonBuffer = NULL;

	if (0 < countPolygons && myUploadingIndexBuffer)
	{
		for (int i = 0; i < countPolygons; i++)
		{
			if (myUploadingIndexBuffer[i])
			{
				myUploadingIndexBuffer[i]->Release();
			}
		}

		if (myUploadingIndexBuffer)
		{
			delete[] myUploadingIndexBuffer;
		}

		myUploadingIndexBuffer = nullptr;
	}
}

void CMaterialMesh::Render(P3DGrpCommandList cmdlist)
{
	if (0 < countPolygons)
	{
		for (int i = 0; i < countPolygons; i++)
		{
			auto& material = myMaterials.at(i);

			auto proceed_mat = material;
			if (!material)
			{
				proceed_mat = myDefaultMaterial;
			}

			if (proceed_mat)
			{
				auto& pipeline = proceed_mat->m_pShader;

				if (pipeline)
				{
					pipeline->PrepareRendering(cmdlist);
					proceed_mat->PrepareRendering(cmdlist);

					Render(cmdlist, i);
				}
				else
				{
					throw "파이프라인과 쉐이더가 존재하지 않음!";
				}
			}
		}
	}
}

void CMaterialMesh::Render(P3DGrpCommandList cmdlist, int polygon_index)
{
	cmdlist->IASetPrimitiveTopology(typePrimitive);
	cmdlist->IASetVertexBuffers(m_nSlot, 1, &myPositionBufferView);

	if (0 < countPolygons && polygon_index < countPolygons)
	{
		cmdlist->IASetIndexBuffer(&(myIndexBufferViews[polygon_index]));
		cmdlist->DrawIndexedInstanced(countPolygonIndices[polygon_index], 1, 0, 0, 0);
	}
	else
	{
		cmdlist->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

UINT CMaterialMesh::GetType() const
{
	return m_nType;
}

CLightenMesh::CLightenMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* pMeshInfo)
	: CMaterialMesh::CMaterialMesh(device, cmdlist, pMeshInfo)
{
	myNormalBuffer = ::CreateBufferResource(device, cmdlist, pMeshInfo->m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &myUploadingNormalBuffer);

	myNormalBufferView.BufferLocation = myNormalBuffer->GetGPUVirtualAddress();
	myNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
	myNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
}

CLightenMesh::~CLightenMesh()
{
	if (myNormalBuffer)
	{
		myNormalBuffer->Release();
	}
}

void CLightenMesh::ReleaseUploadBuffers()
{
	CMaterialMesh::ReleaseUploadBuffers();

	if (myUploadingNormalBuffer)
	{
		myUploadingNormalBuffer->Release();
	}
	myUploadingNormalBuffer = nullptr;
}

void CLightenMesh::Render(P3DGrpCommandList cmdlist, int polygon_index)
{
	cmdlist->IASetPrimitiveTopology(typePrimitive);

	D3D12_VERTEX_BUFFER_VIEW vertex_buffers[2] = { myPositionBufferView, myNormalBufferView };
	cmdlist->IASetVertexBuffers(m_nSlot, 2, vertex_buffers);

	if (0 < countPolygons && polygon_index < countPolygons)
	{
		cmdlist->IASetIndexBuffer(&(myIndexBufferViews[polygon_index]));
		cmdlist->DrawIndexedInstanced(countPolygonIndices[polygon_index], 1, 0, 0, 0);
	}
	else
	{
		cmdlist->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}
