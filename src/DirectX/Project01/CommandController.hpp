#pragma once
#include "pch.h"
#include "DirectObject.hpp"

class CommandController : protected DirectObject
{
public:
	CommandController(ID3D12Device*& device);

	bool OnCreate() override;
	void OnDestroy() override;

	HRESULT TryResetAllocator();
	HRESULT TryResetList();

	inline void WaitForPresent(D3D12_RESOURCE_BARRIER& barrier);

private:
	ID3D12CommandQueue* m_pd3dCommandQueue; //��� ť
	ID3D12CommandAllocator* m_pd3dCommandAllocator; //��� �Ҵ���
	ID3D12GraphicsCommandList* m_pd3dCommandList; // ��� ����Ʈ
};
