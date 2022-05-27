#include "stdafx.h"
#include "Material.hpp"

UINT gnCbvSrvDescriptorIncrementSize = 32;

std::vector<RawMaterial> LoadMaterialsInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };

	std::vector<RawMaterial> result{};

	auto count = ReadIntegerFromFile(pInFile);
	result.reserve(count);

	int number = 0;
	RawMaterial temp{};
	ZeroMemory(&temp, sizeof(temp));

	XMFLOAT4 albedo{};
	XMFLOAT4 emissive{};
	XMFLOAT4 specular{};

	float highlight = 0.0f;
	float glossiness = 0.0f;
	float metalic = 0.0f;
	float reflection = 0.0f;
	float smoothness = 0.0f;

	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Material>:"))
		{
			if (0 != number)
			{
				temp = RawMaterial{};
				ZeroMemory(&temp, sizeof(temp));
			}

			number = ReadIntegerFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			fread(&albedo, sizeof(float), 4, pInFile);
			temp.m_xmf4AlbedoColor = albedo;
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			fread(&emissive, sizeof(float), 4, pInFile);
			temp.m_xmf4EmissiveColor = emissive;
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			fread(&specular, sizeof(float), 4, pInFile);
			temp.m_xmf4SpecularColor = specular;
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			fread(&glossiness, sizeof(float), 1, pInFile);
			temp.m_fGlossiness = glossiness;
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			fread(&smoothness, sizeof(float), 1, pInFile);
			temp.m_fSmoothness = smoothness;
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			fread(&highlight, sizeof(float), 1, pInFile);
			temp.m_fSpecularHighlight = highlight;
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			fread(&metalic, sizeof(float), 1, pInFile);
			temp.m_fMetallic = metalic;
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			fread(&reflection, sizeof(float), 1, pInFile);
			temp.m_fGlossyReflection = reflection;
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			result.push_back(temp);

			break;
		}
	}

	if (number != result.size())
	{
		throw "재질 불러오기 오류!";
	}

	return result;
}

ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource **ppd3dUploadBuffer)
{
	ID3D12Resource *pd3dBuffer = NULL;

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

	HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, NULL, __uuidof(ID3D12Resource), (void **)&pd3dBuffer);

	if (pData)
	{
		switch (d3dHeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppd3dUploadBuffer)
			{
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void **)ppd3dUploadBuffer);

				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8 *pBufferDataBegin = NULL;
				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void **)&pBufferDataBegin);
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
			UINT8 *pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void **)&pBufferDataBegin);
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

int ReadIntegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

DESC_HANDLE operator+(const DESC_HANDLE& handle, const size_t increment)
{
	DESC_HANDLE result = DESC_HANDLE(handle);
	result.ptr += increment;
	return result;
}

DESC_HANDLE operator+(DESC_HANDLE&& handle, const size_t increment)
{
	DESC_HANDLE result = std::forward<DESC_HANDLE>(handle);
	result.ptr += increment;
	return result;
}

DESC_HANDLE& operator+=(DESC_HANDLE& handle, const size_t increment)
{
	handle.ptr += increment;
	return handle;
}

