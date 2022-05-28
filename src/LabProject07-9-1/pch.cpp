#include "stdafx.h"
#include "pch.hpp"
#include "Material.hpp"
#include "Object.h"

char token[64]{};

UINT gnCbvSrvDescriptorIncrementSize = 32;

ShaderHandle CreateEmptyShaderHandle()
{
	ShaderHandle bytecodes{};
	bytecodes.BytecodeLength = 0;
	bytecodes.pShaderBytecode = NULL;

	return bytecodes;
}

CMeshLoadInfo* LoadMeshInfoFromFile(FILE* stream)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nPositions = 0, nColors = 0, nNormals = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	CMeshLoadInfo* pMeshInfo = new CMeshLoadInfo;

	pMeshInfo->m_nVertices = ::ReadIntegerFromFile(stream);
	::ReadStringFromFile(stream, pMeshInfo->m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(stream, pstrToken);

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&(pMeshInfo->m_xmf3AABBCenter), sizeof(XMFLOAT3), 1, stream);
			nReads = (UINT)::fread(&(pMeshInfo->m_xmf3AABBExtents), sizeof(XMFLOAT3), 1, stream);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nPositions = ::ReadIntegerFromFile(stream);
			if (nPositions > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_POSITION;
				pMeshInfo->m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, stream);
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nColors = ::ReadIntegerFromFile(stream);
			if (nColors > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_COLOR;
				pMeshInfo->m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf4Colors, sizeof(XMFLOAT4), nColors, stream);
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nNormals = ::ReadIntegerFromFile(stream);
			if (nNormals > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_NORMAL;
				pMeshInfo->m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, stream);
			}
		}
		else if (!strcmp(pstrToken, "<Indices>:"))
		{
			nIndices = ::ReadIntegerFromFile(stream);
			if (nIndices > 0)
			{
				pMeshInfo->m_pnIndices = new UINT[nIndices];
				nReads = (UINT)::fread(pMeshInfo->m_pnIndices, sizeof(int), nIndices, stream);
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			pMeshInfo->m_nSubMeshes = ::ReadIntegerFromFile(stream);
			if (pMeshInfo->m_nSubMeshes > 0)
			{
				pMeshInfo->m_pnSubSetIndices = new int[pMeshInfo->m_nSubMeshes];
				pMeshInfo->m_ppnSubSetIndices = new UINT * [pMeshInfo->m_nSubMeshes];
				for (int i = 0; i < pMeshInfo->m_nSubMeshes; i++)
				{
					::ReadStringFromFile(stream, pstrToken);
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = ::ReadIntegerFromFile(stream);
						pMeshInfo->m_pnSubSetIndices[i] = ::ReadIntegerFromFile(stream);
						if (pMeshInfo->m_pnSubSetIndices[i] > 0)
						{
							pMeshInfo->m_ppnSubSetIndices[i] = new UINT[pMeshInfo->m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(pMeshInfo->m_ppnSubSetIndices[i], sizeof(int), pMeshInfo->m_pnSubSetIndices[i], stream);
						}

					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
	return(pMeshInfo);
}

std::vector<RawMaterial> LoadRawMaterials(FILE* stream)
{
	auto count = ReadIntegerFromFile(stream);

	std::vector<RawMaterial> result{};
	result.reserve(count);

	XMFLOAT4 albedo{};
	XMFLOAT4 emissive{};
	XMFLOAT4 specular{};

	float highlight = 0.0f;
	float glossiness = 0.0f;
	float metalic = 0.0f;
	float reflection = 0.0f;
	float smoothness = 0.0f;

	int number = 0;
	RawMaterial temp{};
	ZeroMemory(&temp, sizeof(temp));

	while (true)
	{
		ZeroMemory(token, sizeof(token));

		ReadStringFromFile(stream, token);
		if (!strcmp(token, "<Material>:"))
		{
			if (0 != number)
			{
				result.push_back(temp);

				temp = RawMaterial{};
				ZeroMemory(&temp, sizeof(temp));
			}

			number = ReadIntegerFromFile(stream);
		}
		else if (!strcmp(token, "<AlbedoColor>:"))
		{
			fread(&albedo, sizeof(float), 4, stream);
			temp.m_xmf4AlbedoColor = albedo;
		}
		else if (!strcmp(token, "<EmissiveColor>:"))
		{
			fread(&emissive, sizeof(float), 4, stream);
			temp.m_xmf4EmissiveColor = emissive;
		}
		else if (!strcmp(token, "<SpecularColor>:"))
		{
			fread(&specular, sizeof(float), 4, stream);
			temp.m_xmf4SpecularColor = specular;
		}
		else if (!strcmp(token, "<Glossiness>:"))
		{
			fread(&glossiness, sizeof(float), 1, stream);
			temp.m_fGlossiness = glossiness;
		}
		else if (!strcmp(token, "<Smoothness>:"))
		{
			fread(&smoothness, sizeof(float), 1, stream);
			temp.m_fSmoothness = smoothness;
		}
		else if (!strcmp(token, "<Metallic>:"))
		{
			fread(&highlight, sizeof(float), 1, stream);
			temp.m_fSpecularHighlight = highlight;
		}
		else if (!strcmp(token, "<SpecularHighlight>:"))
		{
			fread(&metalic, sizeof(float), 1, stream);
			temp.m_fMetallic = metalic;
		}
		else if (!strcmp(token, "<GlossyReflection>:"))
		{
			fread(&reflection, sizeof(float), 1, stream);
			temp.m_fGlossyReflection = reflection;
		}
		else if (!strcmp(token, "</Materials>"))
		{
			break;
		}
	}

	if (number != result.size())
	{
		throw "재질 불러오기 오류!";
	}

	return result;
}

ID3D12Resource* CreateBufferResource(PtrDevice pd3dDevice, PtrGrpCommandList pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer)
{
	ID3D12Resource* pd3dBuffer = NULL;

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = d3dHeapType;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, NULL, __uuidof(ID3D12Resource), (void**)&pd3dBuffer);

	if (pData)
	{
		switch (d3dHeapType)
		{
			case D3D12_HEAP_TYPE_DEFAULT:
			{
				if (ppd3dUploadBuffer)
				{
					d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
					pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);

					D3D12_RANGE d3dReadRange = { 0, 0 };
					UINT8* pBufferDataBegin = NULL;
					(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
					memcpy(pBufferDataBegin, pData, nBytes);
					(*ppd3dUploadBuffer)->Unmap(0, NULL);

					pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);

					D3D12_RESOURCE_BARRIER d3dResourceBarrier;
					::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
					d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					d3dResourceBarrier.Transition.pResource = pd3dBuffer;
					d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
					d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
					d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
				}
				break;
			}
			case D3D12_HEAP_TYPE_UPLOAD:
			{
				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8* pBufferDataBegin = NULL;
				pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
				memcpy(pBufferDataBegin, pData, nBytes);
				pd3dBuffer->Unmap(0, NULL);
				break;
			}
			case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return(pd3dBuffer);
}
