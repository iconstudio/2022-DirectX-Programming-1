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
	if (0 < m_nMaterials)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) m_ppMaterials[i]->Release();
		}
	}

	if (m_ppMaterials)
	{
		delete[] m_ppMaterials;
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

void CMaterialMesh::SetShader(Pipeline* pipeline)
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial * [m_nMaterials];
	m_ppMaterials[0] = new CMaterial;
	m_ppMaterials[0]->SetShader(pipeline);
}

void CMaterialMesh::SetShader(int index, Pipeline* pipeline)
{
	if (m_ppMaterials[index])
	{
		m_ppMaterials[index]->SetShader(pipeline);
	}
}

void CMaterialMesh::SetMaterial(int index, CMaterial* mat)
{
	if (m_ppMaterials[index])
		m_ppMaterials[index]->Release();

	m_ppMaterials[index] = mat;

	if (m_ppMaterials[index])
		m_ppMaterials[index]->AddRef();
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
			auto& material = m_ppMaterials[i];
			
			if (material)
			{
				auto& pipeline = material->m_pShader;

				if (pipeline)
				{
					pipeline->PrepareRendering(cmdlist);
					material->PrepareRendering(cmdlist);

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
