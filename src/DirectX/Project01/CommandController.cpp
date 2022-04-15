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

	// 명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다.
	result = m_pd3dCommandList->Close();
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateCommandList()");
		return false;
	}

	// 펜스를 생성하고 펜스 값을 0으로 설정한다.
	place = reinterpret_cast<void**>(&m_pd3dFence);
	id = _uuidof(ID3D12Fence);

	result = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, id, place);
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateFence()");
		return false;
	}

	/*
		펜스와 동기화를 위한 이벤트 객체를 생성한다 (이벤트 객체의 초기값을 FALSE이다).
		이벤트가 실행되면(Signal) 이벤트의 값을 자동적으로 FALSE가 되도록 생성한다.
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
	m_nFenceValue++; //CPU 펜스의 값을 증가한다. 

	const UINT64 nFence = m_nFenceValue;
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFence); //GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다. 

	if (m_pd3dFence->GetCompletedValue() < nFence)
	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다. 

		hResult = m_pd3dFence->SetEventOnCompletion(nFence, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CommandController::Execute()
{
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
}
