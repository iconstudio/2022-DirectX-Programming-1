#include "pch.h"
#include "CommandController.hpp"
#include "DirectObject.hpp"

CommandController::CommandController(ID3D12Device*& device)
	: DirectObject(device)
	, m_pd3dCommandQueue(nullptr), m_pd3dCommandList(nullptr), m_pd3dCommandAllocator(nullptr)
{}

bool CommandController::OnCreate()
{
	auto type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	// 직접(Direct) 명령 큐
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(d3dCommandQueueDesc));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = type;

	void** place = reinterpret_cast<void**>(&m_pd3dCommandQueue);
	IID id = _uuidof(ID3D12CommandQueue);

	auto result = Device->CreateCommandQueue(&d3dCommandQueueDesc, id, place);
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateCommandQueue()");
		return false;
	}

	// 직접(Direct) 명령 할당자
	place = reinterpret_cast<void**>(&m_pd3dCommandAllocator);
	id = _uuidof(ID3D12CommandAllocator);

	result = Device->CreateCommandAllocator(type, id, place);
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateCommandAllocator()");
		return false;
	}

	// 직접(Direct) 명령 리스트
	place = reinterpret_cast<void**>(&m_pd3dCommandList);
	id = _uuidof(ID3D12GraphicsCommandList);

	result = Device->CreateCommandList(0, type, m_pd3dCommandAllocator, NULL, id, place);
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateCommandList()");
		return false;
	}

	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다.
	result = m_pd3dCommandList->Close();
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateCommandList()");
		return false;
	}

	return true;
}

void CommandController::OnDestroy()
{
	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();
}

HRESULT CommandController::TryResetAllocator()
{
	return m_pd3dCommandAllocator->Reset();
}

HRESULT CommandController::TryResetList()
{
	return m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
}

inline void CommandController::WaitForPresent(D3D12_RESOURCE_BARRIER& barrier)
{
	m_pd3dCommandList->ResourceBarrier(1, &barrier);
}
