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
	// DXGI ���丮 �������̽��� ���� �������̴�.
	IDXGIFactory4* m_pdxgiFactory;

	// ���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	IDXGISwapChain3* m_pdxgiSwapChain;

	// Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D12Device* m_pd3dDevice;

	// �׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� �������̴�.
	ID3D12PipelineState* m_pd3dPipelineState;
};
