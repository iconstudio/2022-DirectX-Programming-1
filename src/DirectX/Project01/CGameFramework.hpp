#pragma once
#include "pch.h"
#include "CommandController.hpp"
#include "CGameTimer.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	// 프레임워크를 초기화하는 함수이다(주 윈도우가 생성되면 호출된다). 
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	// 스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트를 생성하는 함수이다.
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList(); 
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();

	// 렌더 타겟 뷰와 깊이-스텐실 뷰를 생성하는 함수이다. 
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	// 렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다.
	void BuildObjects();
	void ReleaseObjects();

	// 프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다.
	void ProcessInput();
	void AnimateObjects();

	// CPU와 GPU를 동기화하는 함수이다.
	void FrameAdvance();

	// 윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다.
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void SetRenderingSize(const int width, const int height);
	void OnResizeBackBuffers();

private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;

	CommandController controlCommands;

	// DXGI 팩토리 인터페이스에 대한 포인터이다.
	IDXGIFactory4* m_pdxgiFactory;

	// Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ID3D12Device* m_pd3dDevice;

	// 그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터이다.
	ID3D12PipelineState* m_pd3dPipelineState;

	// 스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	IDXGISwapChain3* m_pdxgiSwapChain;
	// 스왑 체인의 후면 버퍼의 개수
	static constexpr UINT m_nSwapChainBuffers = 2;
	// 현재 스왑 체인의 후면 버퍼 인덱스
	UINT m_nSwapChainBufferIndex;
	// 뷰포트와 씨저 사각형이다.
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	// 렌더 타겟 버퍼 목록
	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	// 서술자 힙 인터페이스 포인터
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	// 렌더 타겟 모드 서술자
	DXGI_MODE_DESC dxgiRenderTargetParameters;
	// 렌더 타겟 서술자 원소의 크기
	UINT m_nRtvDescriptorIncrementSize;

	// 깊이-스텐실 버퍼
	ID3D12Resource* m_pd3dDepthStencilBuffer;
	// 서술자 힙 인터페이스 포인터
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	// 깊이-스텐실 서술자 원소의 크기
	UINT m_nDsvDescriptorIncrementSize;

	// MSAA 다중 샘플링 여부
	bool m_bMsaa4xEnable = false;
	// 다중 샘플링 레벨 수준
	UINT m_nMsaa4xQualityLevels = 0;

	CGameTimer Timer;
	WCHAR m_pszFrameRate[50];
};
