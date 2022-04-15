#pragma once
#include "pch.h"
#include "CommandController.hpp"
#include "FactroyController.hpp"
#include "CGameTimer.h"

class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;

	CGameTimer Timer;
	WCHAR m_pszFrameRate[50];

	FactroyController controlFactory;
	CommandController controlCommands;

	bool m_bMsaa4xEnable = false;
	// MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.
	UINT m_nMsaa4xQualityLevels = 0;

	// ���� ü���� �ĸ� ������ ����
	static const UINT m_nSwapChainBuffers = 2;
	// ���� ���� ü���� �ĸ� ���� �ε���
	UINT m_nSwapChainBufferIndex;
	// ���� Ÿ�� ����
	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	// ������ �� �������̽� ������
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	// ���� Ÿ�� ������ ������ ũ��
	UINT m_nRtvDescriptorIncrementSize;

	 //����-���ٽ� ����
	ID3D12Resource* m_pd3dDepthStencilBuffer;
	// ������ �� �������̽� ������
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	// ����-���ٽ� ������ ������ ũ��
	UINT m_nDsvDescriptorIncrementSize;

	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;
	//����Ʈ�� ���� �簢���̴�.

public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd); //�����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ��̴�(�� �����찡 �����Ǹ� ȣ��ȴ�). 
	void OnDestroy();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList(); //���� ü��, ����̽�, ������ ��, ��� ť/�Ҵ���/����Ʈ�� �����ϴ� �Լ��̴�.

	void CreateRenderTargetViews();
	void CreateDepthStencilView(); //���� Ÿ�� ��� ����-���ٽ� �並 �����ϴ� �Լ��̴�. 
	void BuildObjects();
	void ReleaseObjects();

	//�������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�.
	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	//CPU�� GPU�� ����ȭ�ϴ� �Լ��̴�.
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
		//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�.
};
