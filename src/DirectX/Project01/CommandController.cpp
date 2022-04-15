#include "pch.h"
#include "CommandController.hpp"
#include "DirectObject.hpp"

CommandController::CommandController()
	: DirectObject()
	, m_pd3dCommandQueue(nullptr), m_pd3dCommandList(nullptr), m_pd3dCommandAllocator(nullptr)
	, m_hFenceEvent(NULL), m_pd3dFence(nullptr), m_nFenceValue(0)
{}

bool CommandController::OnCreate()
{
	auto type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	// ����(Direct) ��� ť
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

	// ����(Direct) ��� �Ҵ���
	place = reinterpret_cast<void**>(&m_pd3dCommandAllocator);
	id = _uuidof(ID3D12CommandAllocator);

	result = Device->CreateCommandAllocator(type, id, place);
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateCommandAllocator()");
		return false;
	}

	// ����(Direct) ��� ����Ʈ
	place = reinterpret_cast<void**>(&m_pd3dCommandList);
	id = _uuidof(ID3D12GraphicsCommandList);

	result = Device->CreateCommandList(0, type, m_pd3dCommandAllocator, NULL, id, place);
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateCommandList()");
		return false;
	}

	// ��� ����Ʈ�� �����Ǹ� ����(Open) �����̹Ƿ� ����(Closed) ���·� �����.
	result = m_pd3dCommandList->Close();
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateCommandList()");
		return false;
	}

	// �潺�� �����ϰ� �潺 ���� 0���� �����Ѵ�.
	place = reinterpret_cast<void**>(&m_pd3dFence);
	id = _uuidof(ID3D12Fence);

	result = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, id, place);
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateFence()");
		return false;
	}

	/*
		�潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� �����Ѵ� (�̺�Ʈ ��ü�� �ʱⰪ�� FALSE�̴�).
		�̺�Ʈ�� ����Ǹ�(Signal) �̺�Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.
	*/
	m_nFenceValue = 0;
	m_hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == m_hFenceEvent)
	{
		ErrorDisplay(L"CreateEvent()");
		return false;
	}

	return true;
}

void CommandController::OnDestroy()
{
	CloseHandle(m_hFenceEvent);

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();
	if (m_pd3dFence) m_pd3dFence->Release();
}

void CommandController::RSSetViewports(const D3D12_VIEWPORT& port)
{
	m_pd3dCommandList->RSSetViewports(1, &port);
}

void CommandController::RSSetScissorRects(const D3D12_RECT& window)
{
	m_pd3dCommandList->RSSetScissorRects(1, &window);
}

void CommandController::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE& ptr, const FLOAT rgba[4])
{
	m_pd3dCommandList->ClearRenderTargetView(ptr, rgba, 0, NULL);
}

void CommandController::ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE& ptr)
{
	m_pd3dCommandList->ClearDepthStencilView(ptr, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL
		, 1.0f, 0, 0, NULL);
}

void CommandController::OMSetRenderTargets(D3D12_CPU_DESCRIPTOR_HANDLE& rt, D3D12_CPU_DESCRIPTOR_HANDLE& ds)
{
	m_pd3dCommandList->OMSetRenderTargets(1, &rt, TRUE, &ds);
}

HRESULT CommandController::TryResetAllocator()
{
	return m_pd3dCommandAllocator->Reset();
}

HRESULT CommandController::TryResetList()
{
	return m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
}

HRESULT CommandController::TryCloseList()
{
	return m_pd3dCommandList->Close();
}

void CommandController::WaitForPresent(D3D12_RESOURCE_BARRIER& barrier)
{
	m_pd3dCommandList->ResourceBarrier(1, &barrier);
}

void CommandController::WaitForGpuComplete()
{
	m_nFenceValue++; //CPU �潺�� ���� �����Ѵ�. 

	const UINT64 nFence = m_nFenceValue;
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFence); //GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰��Ѵ�. 

	if (m_pd3dFence->GetCompletedValue() < nFence)
	{
		//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���. 

		hResult = m_pd3dFence->SetEventOnCompletion(nFence, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CommandController::Execute()
{
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
}
