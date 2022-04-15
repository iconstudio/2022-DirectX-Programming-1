#include "pch.h"
#include "FactoryController.hpp"

FactoryController::FactoryController()
	:m_pdxgiFactory(nullptr), m_pd3dDevice(nullptr)
	, m_pd3dPipelineState(nullptr)
{

}

bool FactoryController::OnCreate()
{
	UINT flags = 0;

	void** place = reinterpret_cast<void**>(&m_pdxgiFactory);
	IID id = _uuidof(IDXGIFactory4);

	auto result = CreateDXGIFactory2(flags, id, place);
	if (FAILED(result))
	{
		ErrorDisplay(L"CreateDXGIFactory2()");
		return false;
	}

	// ��� �ϵ���� ����Ϳ� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� �����Ѵ�.
	auto feature = D3D_FEATURE_LEVEL_12_0;

	IDXGIAdapter1* pd3dAdapter = NULL;
	DXGI_ADAPTER_DESC1 dxgiAdapterDesc{};
	place = reinterpret_cast<void**>(&m_pd3dDevice);
	id = _uuidof(ID3D12Device);

	UINT index = 0;
	while (true)
	{
		if (DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(index, &pd3dAdapter))
		{
			break;
		}

		ZeroMemory(&dxgiAdapterDesc, sizeof(dxgiAdapterDesc));
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		
		result = D3D12CreateDevice(pd3dAdapter, feature, id, place);
		if (SUCCEEDED(result)) break;

		index++;
	}

	//Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� �����Ѵ�.
	if (!pd3dAdapter)
	{
		result = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		if (FAILED(result))
		{
			ErrorDisplay(L"EnumWarpAdapter()");
			return false;
		}

		result = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, id, place);
		if (FAILED(result))
		{
			ErrorDisplay(L"EnumWarpAdapter �� D3D12CreateDevice()");
			return false;
		}
	}

	return true;
}

void FactoryController::OnDestroy()
{

}
