#pragma once
#include "pch.h"
#include "DirectObject.hpp"

class CommandController : public DirectObject
{
public:
	CommandController();

	bool OnCreate() override;
	void OnDestroy() override;

	void RSSetViewports(const D3D12_VIEWPORT& port);
	void RSSetScissorRects(const D3D12_RECT& window);
	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE& ptr, const FLOAT rgba[4]);
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE& ptr);
	void OMSetRenderTargets(D3D12_CPU_DESCRIPTOR_HANDLE& rt, D3D12_CPU_DESCRIPTOR_HANDLE& ds);

	HRESULT TryResetAllocator();
	HRESULT TryResetList();
	HRESULT TryCloseList();

	void WaitForPresent(D3D12_RESOURCE_BARRIER& barrier);
	void WaitForGpuComplete();
	void Execute();

	friend class CGameFramework;

private:
	ID3D12CommandQueue* m_pd3dCommandQueue; // 명령 큐
	ID3D12CommandAllocator* m_pd3dCommandAllocator; // 명령 할당자
	ID3D12GraphicsCommandList* m_pd3dCommandList; // 명령 리스트

	ID3D12Fence* m_pd3dFence; // 펜스 인터페이스 포인터
	UINT64 m_nFenceValue; // 펜스의 값
	HANDLE m_hFenceEvent; // 이벤트 핸들

};
