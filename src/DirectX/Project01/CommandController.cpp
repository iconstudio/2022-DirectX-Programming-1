#include "pch.h"
#include "CommandController.hpp"
#include "DirectObject.hpp"

CommandController::CommandController(ID3D12Device*& device)
	: DirectObject(device)
	, m_pd3dCommandQueue(nullptr), m_pd3dCommandList(nullptr), m_pd3dCommandAllocator(nullptr)
{}

bool CommandController::OnCreate()
{
	// 직접 명령 큐
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(d3dCommandQueueDesc));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	void** place = reinterpret_cast<void**>(&m_pd3dCommandQueue);
	IID id = _uuidof(ID3D12CommandQueue);

	auto result = Device->CreateCommandQueue(&d3dCommandQueueDesc, id, place);
	if (FAILED(result))
	{
		return false;
	}

	result = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT
		, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);
	//직접(Direct) 명령 할당자를 생성한다.

	result = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT
		, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList)
		, (void**)(&m_pd3dCommandList));
	//직접(Direct) 명령 리스트를 생성한다.

	result = m_pd3dCommandList->Close();
	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다.

	return false;
}

void CommandController::OnDestroy()
{
	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();
}
