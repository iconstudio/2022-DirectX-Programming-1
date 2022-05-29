// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// LabProject03-1.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "pch.hpp"

UINT gnCbvSrvDescriptorIncrementSize = 32;

int ReadIntegerFromFile(FILE* file)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, file);
	return(nValue);
}

float ReadFloatFromFile(FILE* file)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, file);
	return(fValue);
}

BYTE ReadStringFromFile(FILE* file, char* token)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, file);
	nReads = (UINT)::fread(token, sizeof(char), nStrLength, file);
	token[nStrLength] = '\0';

	return(nStrLength);
}

ID3D12Resource* CreateBufferResource(P3DDevice device, P3DGrpCommandList cmd_list
	, void* data, UINT data_sz
	, D3D12_HEAP_TYPE type
	, D3D12_RESOURCE_STATES states
	, ID3D12Resource** upload_buffer)
{
	ID3D12Resource* result = nullptr;

	// 모든 것을 다렉에서 알아서 해주는 힙
	D3D12_HEAP_PROPERTIES heap_property{};
	ZeroMemory(&heap_property, sizeof(heap_property));

	heap_property.Type = type;
	heap_property.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_property.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_property.CreationNodeMask = 1;
	heap_property.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC res_desc{};
	ZeroMemory(&res_desc, sizeof(res_desc));

	res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	res_desc.Alignment = 0;
	res_desc.Width = data_sz;
	res_desc.Height = 1;
	res_desc.DepthOrArraySize = 1;
	res_desc.MipLevels = 1;
	res_desc.Format = DXGI_FORMAT_UNKNOWN;
	res_desc.SampleDesc.Count = 1;
	res_desc.SampleDesc.Quality = 0;
	res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	res_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	auto init_state = D3D12_RESOURCE_STATE_COPY_DEST;
	if (type == D3D12_HEAP_TYPE_UPLOAD)
	{
		init_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	}
	else if (type == D3D12_HEAP_TYPE_READBACK)
	{
		init_state = D3D12_RESOURCE_STATE_COPY_DEST;
	}

	auto uuid = __uuidof(ID3D12Resource);
	auto place = reinterpret_cast<void**>(&result);

	auto valid = device->CreateCommittedResource(&heap_property, D3D12_HEAP_FLAG_NONE, &res_desc, init_state, NULL, uuid, place);
	if (FAILED(valid))
	{
		throw "자원 생성 실패!";
	}

	if (data)
	{
		switch (type)
		{
			// 정적인 정점의 정보
			case D3D12_HEAP_TYPE_DEFAULT:
			{
				if (upload_buffer)
				{
					heap_property.Type = D3D12_HEAP_TYPE_UPLOAD;

					D3D12_CLEAR_VALUE clear_value{};
					ZeroMemory(&clear_value, sizeof(clear_value));

					clear_value.Format = DXGI_FORMAT_UNKNOWN;

					uuid = __uuidof(ID3D12Resource);
					place = reinterpret_cast<void**>(upload_buffer);
					const auto op = D3D12_RESOURCE_STATE_GENERIC_READ;
					valid = device->CreateCommittedResource(&heap_property
						, D3D12_HEAP_FLAG_NONE
						, &res_desc, op
						, NULL
						, uuid, place);

					if (FAILED(valid))
					{
						throw "GPU에서 읽은 내용으로 메모리 할당 실패!";
					}

					// 알아서 읽는다.
					D3D12_RANGE d3dReadRange = { 0, 0 };
					UINT8* pBufferDataBegin = nullptr;

					auto& mapped_buffer = *upload_buffer;
					if (!mapped_buffer)
					{
						throw "자원 버퍼가 만들어지지 않음!";
					}

					place = reinterpret_cast<void**>(&pBufferDataBegin);
					valid = mapped_buffer->Map(0, &d3dReadRange, place);
					if (FAILED(valid))
					{
						throw "읽을 자원의 정보를 얻어오지 못함!";
					}

					memcpy(pBufferDataBegin, data, data_sz);
					mapped_buffer->Unmap(0, NULL);

					// 읽어온 자원을 CPU 메모리로 복사
					cmd_list->CopyResource(result, *upload_buffer);

					// 복사가 끝날 때 까지 접근 금지
					D3D12_RESOURCE_BARRIER barrier;
					ZeroMemory(&barrier, sizeof(barrier));

					barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barrier.Transition.pResource = result;
					barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
					barrier.Transition.StateAfter = states;
					barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

					cmd_list->ResourceBarrier(1, &barrier);
				}
			}
			break;

			// CPU에서 GPU로 업로드
			case D3D12_HEAP_TYPE_UPLOAD:
			{
				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8* pBufferDataBegin = NULL;

				place = reinterpret_cast<void**>(&pBufferDataBegin);
				valid = result->Map(0, &d3dReadRange, place);
				if (FAILED(valid))
				{
					throw "읽을 자원의 정보를 얻어오지 못함!";
				}

				memcpy(pBufferDataBegin, data, data_sz);
				result->Unmap(0, NULL);
			}
			break;

			// 읽기
			case D3D12_HEAP_TYPE_READBACK:
			{}
			break;
		}
	}

	return result;
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

