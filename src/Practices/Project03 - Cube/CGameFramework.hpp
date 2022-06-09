#pragma once
#include "CommandController.hpp"
#include "CScene.hpp"
#include "CGameTimer.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	// �����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ��̴�(�� �����찡 �����Ǹ� ȣ��ȴ�). 
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	// ���� ü��, ����̽�, ������ ��, ���� ť/�Ҵ���/����Ʈ�� �����ϴ� �Լ��̴�.
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList(); 
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();

	// ���� Ÿ�� ��� ����-���ٽ� �並 �����ϴ� �Լ��̴�. 
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void Start();
	void BuildCamera();
	void BuildScene();
	void ReleaseObjects();

	// �����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�.
	void ProcessInput();
	void AnimateObjects(float delta_time);

	// CPU�� GPU�� ����ȭ�ϴ� �Լ��̴�.
	void WaitForGpuComplete();
	void FrameAdvance(float delta_time);
	void MoveToNextFrame();

	// �������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�.
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void SetRenderingSize(const int width, const int height);
	void OnResizeBackBuffers();

	friend class CommandController;
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;

	CommandController controlCommands;

	// DXGI ���丮 �������̽��� ���� �������̴�.
	IDXGIFactory4* m_pdxgiFactory;

	// Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D12Device* m_pd3dDevice;

	// ���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	IDXGISwapChain3* m_pdxgiSwapChain;
	// ���� ü���� �ĸ� ������ ����
	static constexpr UINT m_nSwapChainBuffers = 2;
	// ���� ���� ü���� �ĸ� ���� �ε���
	UINT m_nSwapChainBufferIndex;

	// �潺 �������̽� ������
	ID3D12Fence* m_pd3dFence;

	UINT64 m_nFenceValues[CGameFramework::m_nSwapChainBuffers];
	UINT64 m_nFenceValue; // �潺�� ��
	HANDLE m_hFenceEvent; // �̺�Ʈ �ڵ�

	// ����Ʈ�� ���� �簢���̴�.
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	// ���� Ÿ�� ���� ���
	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	// ������ �� �������̽� ������
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	// ���� Ÿ�� ��� ������
	DXGI_MODE_DESC dxgiRenderTargetParameters;
	// ���� Ÿ�� ������ ������ ũ��
	UINT m_nRtvDescriptorIncrementSize;

	// ����-���ٽ� ����
	ID3D12Resource* m_pd3dDepthStencilBuffer;
	// ������ �� �������̽� ������
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	// ����-���ٽ� ������ ������ ũ��
	UINT m_nDsvDescriptorIncrementSize;

	// MSAA ���� ���ø� ����
	bool m_bMsaa4xEnable = false;
	// ���� ���ø� ���� ����
	UINT m_nMsaa4xQualityLevels = 0;

	shared_ptr<CCamera> m_pCamera;
	unique_ptr<CScene> m_pScene;
};