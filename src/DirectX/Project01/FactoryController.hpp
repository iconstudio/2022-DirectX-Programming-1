#pragma once
#include "pch.h"
#include "DirectObject.hpp"

class FactoryController : public DirectObject
{
public:
	FactoryController();

	bool OnCreate() override;
	void OnDestroy() override;


private:
	// DXGI 팩토리 인터페이스에 대한 포인터이다.
	IDXGIFactory4* m_pdxgiFactory;

	// 스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	IDXGISwapChain3* m_pdxgiSwapChain;

	// Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ID3D12Device* m_pd3dDevice;

	// 그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터이다.
	ID3D12PipelineState* m_pd3dPipelineState;
};
