#include "pch.hpp"
#include "Mesh.h"
#include "RawMesh.hpp"
#include "Material.hpp"
#include "Pipeline.hpp"

CMesh::CMesh()
	: myVertexType(0), typePrimitive(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_nSlot(0), m_nOffset()
	, countVertices(0), myVertices()
	, countPolygons(0), myPolygons()
	, myPositionBuffer(nullptr), myPositionBufferView(), myUploadingPositonBuffer(nullptr)
{}

CMesh::CMesh(P3DDevice device, P3DGrpCommandList cmdlist, const RawMesh& raw)
	: CMesh()
{
	countVertices = raw.countVertices;
	myVertexType = raw.myVertexType;

	constexpr auto pos_obj_sz = sizeof(XMFLOAT3);
	const auto pos_sz = pos_obj_sz * countVertices;

	const auto& container = raw.myPositions;
	const auto& blob = container.data();

	myPositionBuffer = CreateBufferResource(device, cmdlist
		, blob, pos_sz
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &myUploadingPositonBuffer);

	myPositionBufferView.BufferLocation = myPositionBuffer->GetGPUVirtualAddress();
	myPositionBufferView.StrideInBytes = pos_obj_sz;
	myPositionBufferView.SizeInBytes = static_cast<UINT>(pos_sz);

	countPolygons = raw.countPolygons;
	if (0 < countPolygons)
	{
		// 통째로 복사
		myPolygons = raw.myPolygons;

		myIndexBuffers = new ID3D12Resource * [countPolygons];
		myIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[countPolygons];
		myUploadingIndexBuffer = new ID3D12Resource * [countPolygons];

		for (int i = 0; i < countPolygons; i++)
		{
			const auto& polygon = myPolygons.at(i);
			const auto index_count = polygon.GetSize();
			const auto indice_size = sizeof(UINT) * index_count;
			const auto indice_blob = reinterpret_cast<const void*>(polygon.GetData());

			myIndexBuffers[i] = CreateBufferResource(device, cmdlist
				, indice_blob, indice_size
				, D3D12_HEAP_TYPE_DEFAULT
				, D3D12_RESOURCE_STATE_INDEX_BUFFER
				, &myUploadingIndexBuffer[i]);

			myIndexBufferViews[i].BufferLocation = myIndexBuffers[i]->GetGPUVirtualAddress();
			myIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
			myIndexBufferViews[i].SizeInBytes = static_cast<UINT>(indice_size);
		}
	}
}

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
}

void CMesh::PrepareRendering(P3DGrpCommandList cmdlist) const
{
	cmdlist->IASetPrimitiveTopology(typePrimitive);
	cmdlist->IASetVertexBuffers(m_nSlot, 1, &myPositionBufferView);
}

void CMesh::Render(P3DGrpCommandList cmdlist) const
{
	PrepareRendering(cmdlist);

	if (0 < countPolygons)
	{
		for (int i = 0; i < countPolygons; i++)
		{
			Render(cmdlist, i);
		}
	}
	else
	{
		cmdlist->DrawInstanced(countVertices, 1, m_nOffset, 0);
	}
}

void CMesh::Render(P3DGrpCommandList cmdlist, int polygon_index) const
{
	cmdlist->IASetIndexBuffer(&(myIndexBufferViews[polygon_index]));
	PolygonAt(polygon_index).Render(cmdlist);
}

void CMesh::RenderIndexed(P3DGrpCommandList cmdlist, int index, UINT count) const
{
	cmdlist->IASetIndexBuffer(&(myIndexBufferViews[index]));
	cmdlist->DrawIndexedInstanced(count, 1, 0, 0, 0);
}

void CMesh::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
{
	typePrimitive = topology;
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

const CPolygon& CMesh::PolygonAt(const size_t index) const
{
	return myPolygons.at(index);
}

CPolygon& CMesh::PolygonAt(const size_t index)
{
	return myPolygons.at(index);
}

UINT CMesh::GetType() const
{
	return myVertexType;
}

CDiffusedMesh::CDiffusedMesh(P3DDevice device, P3DGrpCommandList cmdlist, const RawMesh& raw)
	: CMesh(device, cmdlist, raw)
	, myColourBuffer(nullptr), myColourBufferView(), myUploadingColourBuffer()
{
	constexpr auto col_obj_sz = sizeof(XMFLOAT4);
	const auto col_sz = col_obj_sz * countVertices;

	const auto& container = raw.myColours;
	const auto& blob = container.data();

	myColourBuffer = CreateBufferResource(device, cmdlist
		, blob, col_sz
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &myUploadingColourBuffer);

	myColourBufferView.BufferLocation = myColourBuffer->GetGPUVirtualAddress();
	myColourBufferView.StrideInBytes = col_obj_sz;
	myColourBufferView.SizeInBytes = static_cast<UINT>(col_sz);
}

CDiffusedMesh::~CDiffusedMesh()
{
	if (myColourBuffer)
	{
		myColourBuffer->Release();
		myColourBuffer = nullptr;
	}
}

void CDiffusedMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();

	if (myUploadingColourBuffer)
	{
		myUploadingColourBuffer->Release();
	}
	myUploadingColourBuffer = nullptr;
}

void CDiffusedMesh::PrepareRendering(P3DGrpCommandList cmdlist) const
{
	auto& pipeline = Pipeline::diffusedShader;
	if (!pipeline)
	{
		throw "색상 쉐이더가 존재하지 않음!";
	}

	cmdlist->IASetPrimitiveTopology(typePrimitive);

	D3D12_VERTEX_BUFFER_VIEW vertex_buffers[2] = { myPositionBufferView, myColourBufferView };
	cmdlist->IASetVertexBuffers(m_nSlot, 2, vertex_buffers);
}

CMaterialMesh::CMaterialMesh(P3DDevice device, P3DGrpCommandList cmdlist, const RawMesh& raw)
	: CMesh(device, cmdlist, raw)
	, myDefaultMaterial(nullptr), myMaterials()
{}

CPlainMesh::CPlainMesh(P3DDevice device, P3DGrpCommandList cmdlist, const RawMesh& raw)
	: CDiffusedMesh(device, cmdlist, raw)
{}

CPlainMesh::~CPlainMesh()
{}

void CPlainMesh::PrepareRendering(P3DGrpCommandList cmdlist) const
{
	cmdlist->IASetPrimitiveTopology(typePrimitive);

	D3D12_VERTEX_BUFFER_VIEW vertex_buffers[2] = { myPositionBufferView, myColourBufferView };
	cmdlist->IASetVertexBuffers(m_nSlot, 2, vertex_buffers);
}

CMaterialMesh::~CMaterialMesh()
{}

void CMaterialMesh::AssignShader(int mat_index, Pipeline* pipeline)
{
	if (mat_index < myMaterials.size())
	{
		auto& mat = myMaterials.at(mat_index);
		mat->SetShader(pipeline);
	}
	else
	{
		throw "잘못된 재질의 번호를 참조하는 중!";
	}
}

void CMaterialMesh::AssignMaterial(const std::vector<shared_ptr<CMaterial>>& list)
{
	myMaterials = list;
}

void CMaterialMesh::AssignMaterial(std::vector<shared_ptr<CMaterial>>&& list)
{
	myMaterials = std::forward<std::vector<shared_ptr<CMaterial>>>(list);
}

void CMaterialMesh::AssignMaterial(const std::vector<RawMaterial>& list, Pipeline* pipeline)
{
	auto inserter = std::back_inserter(myMaterials);

	std::transform(list.begin(), list.end(), inserter
		, [&](const RawMaterial& raw_mat) -> shared_ptr<CMaterial> {
			auto result = make_shared<CMaterial>(raw_mat);
			result->SetShader(pipeline);
			return result;
		});
}

void CMaterialMesh::SetMaterial(int mat_index, const CMaterial& material)
{
	if (mat_index < myMaterials.size())
	{
		auto& mat = myMaterials.at(mat_index);
		mat = make_shared<CMaterial>(material);
	}
	else
	{
		throw "잘못된 재질의 번호를 참조하는 중!";
	}
}

void CMaterialMesh::SetMaterial(int mat_index, CMaterial&& material)
{
	if (mat_index < myMaterials.size())
	{
		auto& mat = myMaterials.at(mat_index);
		mat = make_shared<CMaterial>(std::forward<CMaterial>(material));
	}
	else
	{
		throw "잘못된 재질의 번호를 참조하는 중!";
	}
}

void CMaterialMesh::AddMaterial(const shared_ptr<CMaterial>& material)
{
	myMaterials.push_back(material);
}

void CMaterialMesh::AddMaterial(shared_ptr<CMaterial>&& material)
{
	myMaterials.push_back(std::forward<shared_ptr<CMaterial>>(material));
}

void CMaterialMesh::Render(P3DGrpCommandList cmdlist) const
{
	PrepareRendering(cmdlist);

	if (0 < countPolygons)
	{
		for (int i = 0; i < countPolygons; i++)
		{
			shared_ptr<CMaterial> proceed_mat = nullptr;
			if (i < myMaterials.size())
			{
				auto& material = myMaterials.at(i);
				proceed_mat = material;
			}
			else if (myDefaultMaterial)
			{
				proceed_mat = myDefaultMaterial;
			}
			else
			{
				throw "메쉬에 재질이 존재하지 않음!";
			}

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
	else
	{
		cmdlist->DrawInstanced(countVertices, 1, m_nOffset, 0);
	}
}

CLightenMesh::CLightenMesh(P3DDevice device, P3DGrpCommandList cmdlist, const RawMesh& raw_mesh)
	: CMaterialMesh(device, cmdlist, raw_mesh)
{
	constexpr auto nrm_obj_sz = sizeof(XMFLOAT3);
	const auto nrm_sz = nrm_obj_sz * countVertices;

	const auto& container = raw_mesh.myNormals;
	const auto& blob = container.data();

	myNormalBuffer = CreateBufferResource(device, cmdlist
		, blob, nrm_sz
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &myUploadingNormalBuffer);

	myNormalBufferView.BufferLocation = myNormalBuffer->GetGPUVirtualAddress();
	myNormalBufferView.StrideInBytes = nrm_obj_sz;
	myNormalBufferView.SizeInBytes = static_cast<UINT>(nrm_sz);
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

void CLightenMesh::PrepareRendering(P3DGrpCommandList cmdlist) const
{
	cmdlist->IASetPrimitiveTopology(typePrimitive);

	D3D12_VERTEX_BUFFER_VIEW vertex_buffers[2] = { myPositionBufferView, myNormalBufferView };
	cmdlist->IASetVertexBuffers(m_nSlot, 2, vertex_buffers);
}

void CLightenMesh::Render(P3DGrpCommandList cmdlist) const
{
	CMaterialMesh::Render(cmdlist);
}

void CLightenMesh::Render(P3DGrpCommandList cmdlist, int polygon_index) const
{
	PrepareRendering(cmdlist);

	CMesh::Render(cmdlist, polygon_index);
}
