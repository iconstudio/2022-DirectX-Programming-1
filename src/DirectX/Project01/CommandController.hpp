#pragma once
#include "pch.h"
#include "DirectObject.hpp"

class CommandController : protected DirectObject
{
public:
	CommandController(ID3D12Device*& device);

	bool OnCreate() override;
	void OnDestroy() override;

	void RSSetViewports(const D3D12_VIEWPORT& port);
	void RSSetScissorRects(const D3D12_RECT& window);
	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE ptr, const FLOAT rgba[4]);
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE ptr);

	HRESULT TryResetAllocator();
	HRESULT TryResetList();

	inline void WaitForPresent(D3D12_RESOURCE_BARRIER& barrier);

private:
	ID3D12CommandQueue* m_pd3dCommandQueue; //명령 큐
	ID3D12CommandAllocator* m_pd3dCommandAllocator; //명령 할당자
	ID3D12GraphicsCommandList* m_pd3dCommandList; // 명령 리스트
};
