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
	ID3D12CommandQueue* m_pd3dCommandQueue; // ��� ť
	ID3D12CommandAllocator* m_pd3dCommandAllocator; // ��� �Ҵ���
	ID3D12GraphicsCommandList* m_pd3dCommandList; // ��� ����Ʈ

	ID3D12Fence* m_pd3dFence; // �潺 �������̽� ������
	UINT64 m_nFenceValue; // �潺�� ��
	HANDLE m_hFenceEvent; // �̺�Ʈ �ڵ�

};
