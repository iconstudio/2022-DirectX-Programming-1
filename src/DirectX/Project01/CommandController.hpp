#pragma once
#include "pch.h"

class CommandController : protected DirectObject
{
public:
	CommandController(ID3D12Device*& device);

	bool OnCreate() override;
	void OnDestroy() override;

private:
	ID3D12CommandQueue* m_pd3dCommandQueue; //명령 큐
	ID3D12CommandAllocator* m_pd3dCommandAllocator; //명령 할당자
	ID3D12GraphicsCommandList* m_pd3dCommandList; // 명령 리스트
};
