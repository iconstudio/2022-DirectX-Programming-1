#include "pch.hpp"
#include "Mesh.h"
#include "RawMesh.hpp"
#include "Material.hpp"
#include "Pipeline.hpp"

CMesh::CMesh()
	: myPositionBuffer(nullptr), myPositionBufferView(), myUploadingPositonBuffer(nullptr)
{}

CMesh::~CMesh()
{
	if (myPositionBuffer)
	{
		myPositionBuffer->Release();
		myPositionBuffer = nullptr;
	}

	if (myIndexBuffers)
	{
		if (0 < countPolygons)
		{
			for (int i = 0; i < countPolygons; i++)
			{
				if (myIndexBuffers[i])
				{
					myIndexBuffers[i]->Release();
				}
			}
		}

		delete[] myIndexBuffers;
		myIndexBuffers = nullptr;
	}

	if (myIndexBufferViews)
	{
		delete[] myIndexBufferViews;
	}

	if (countPolygonIndices)
	{
		delete[] countPolygonIndices;
	}
}

void CMesh::PrepareRender(P3DGrpCommandList cmdlist) const
{
	cmdlist->IASetPrimitiveTopology(typePrimitive);
	cmdlist->IASetVertexBuffers(m_nSlot, 1, &myPositionBufferView);
}

void CMesh::Render(P3DGrpCommandList cmdlist) const
{
	PrepareRender(cmdlist);

	if (0 < countPolygons)
	{
		for (int i = 0; i < countPolygons; i++)
		{
			Render(cmdlist, i);
		}
	}
}

void CMesh::Render(P3DGrpCommandList cmdlist, int polygon_index) const
{
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

void CMesh::ReleaseUploadBuffers()
{
	if (myUploadingPositonBuffer)
	{
		myUploadingPositonBuffer->Release();
	}
	myUploadingPositonBuffer = nullptr;

	if (myUploadingIndexBuffer)
	{
		if (0 < countPolygons)
		{
			for (int i = 0; i < countPolygons; i++)
			{
				if (myUploadingIndexBuffer[i])
				{
					myUploadingIndexBuffer[i]->Release();
				}
			}
		}

		delete[] myUploadingIndexBuffer;
		myUploadingIndexBuffer = nullptr;
	}
}

UINT CMesh::GetType() const
{
	return m_nType;
}

CDiffusedMesh::CDiffusedMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw)
	: CMesh()
	, myColourBuffer(nullptr), myColourBufferView(), myUploadingColourBuffer()
{
	m_nVertices = raw->m_nVertices;
	m_nType = raw->m_nType;

	constexpr auto pos_obj_sz = sizeof(XMFLOAT3);
	const auto pos_sz = pos_obj_sz * m_nVertices;
	myPositionBuffer = CreateBufferResource(device, cmdlist
		, raw->m_pxmf3Positions, pos_sz
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &myUploadingPositonBuffer);

	myPositionBufferView.BufferLocation = myPositionBuffer->GetGPUVirtualAddress();
	myPositionBufferView.StrideInBytes = pos_obj_sz;
	myPositionBufferView.SizeInBytes = pos_sz;

	constexpr auto col_obj_sz = sizeof(XMFLOAT4);
	const auto col_sz = col_obj_sz * m_nVertices;
	myColourBuffer = CreateBufferResource(device, cmdlist
		, raw->m_pxmf4Colors, col_sz
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &myUploadingColourBuffer);

	myColourBufferView.BufferLocation = myColourBuffer->GetGPUVirtualAddress();
	myColourBufferView.StrideInBytes = col_obj_sz;
	myColourBufferView.SizeInBytes = col_sz;

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

CDiffusedMesh::~CDiffusedMesh()
{}

void CDiffusedMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();

	if (myUploadingColourBuffer)
	{
		myUploadingColourBuffer->Release();
	}
	myUploadingColourBuffer = nullptr;
}

void CDiffusedMesh::PrepareRender(P3DGrpCommandList cmdlist) const
{
	cmdlist->IASetPrimitiveTopology(typePrimitive);

	D3D12_VERTEX_BUFFER_VIEW vertex_buffers[2] = { myPositionBufferView, myColourBufferView };
	cmdlist->IASetVertexBuffers(m_nSlot, 2, vertex_buffers);
}

CMaterialMesh::CMaterialMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* raw)
	: myDefaultMaterial(nullptr), myMaterials()
{
	m_nVertices = raw->m_nVertices;
	m_nType = raw->m_nType;

	myPositionBuffer = CreateBufferResource(device, cmdlist, raw->m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &myUploadingPositonBuffer);

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
	CMesh::ReleaseUploadBuffers();
}

void CMaterialMesh::Render(P3DGrpCommandList cmdlist) const
{
	PrepareRender(cmdlist);

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

					CMesh::Render(cmdlist, i);
				}
				else
				{
					throw "파이프라인과 쉐이더가 존재하지 않음!";
				}
			}
		}
	}
}

CLightenMesh::CLightenMesh(P3DDevice device, P3DGrpCommandList cmdlist, RawMesh* pMeshInfo)
	: CMaterialMesh(device, cmdlist, pMeshInfo)
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

void CLightenMesh::PrepareRender(P3DGrpCommandList cmdlist) const
{
	cmdlist->IASetPrimitiveTopology(typePrimitive);

	D3D12_VERTEX_BUFFER_VIEW vertex_buffers[2] = { myPositionBufferView, myNormalBufferView };
	cmdlist->IASetVertexBuffers(m_nSlot, 2, vertex_buffers);
}

void CLightenMesh::Render(P3DGrpCommandList cmdlist, int polygon_index) const
{
	PrepareRender(cmdlist);

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
