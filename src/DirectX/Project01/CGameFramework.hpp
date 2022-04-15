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
	// MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
	UINT m_nMsaa4xQualityLevels = 0;

	// 스왑 체인의 후면 버퍼의 개수
	static const UINT m_nSwapChainBuffers = 2;
	// 현재 스왑 체인의 후면 버퍼 인덱스
	UINT m_nSwapChainBufferIndex;
	// 렌더 타겟 버퍼
	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	// 서술자 힙 인터페이스 포인터
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	// 렌더 타겟 서술자 원소의 크기
	UINT m_nRtvDescriptorIncrementSize;

	 //깊이-스텐실 버퍼
	ID3D12Resource* m_pd3dDepthStencilBuffer;
	// 서술자 힙 인터페이스 포인터
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	// 깊이-스텐실 서술자 원소의 크기
	UINT m_nDsvDescriptorIncrementSize;

	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;
	//뷰포트와 씨저 사각형이다.

public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd); //프레임워크를 초기화하는 함수이다(주 윈도우가 생성되면 호출된다). 
	void OnDestroy();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList(); //스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트를 생성하는 함수이다.

	void CreateRenderTargetViews();
	void CreateDepthStencilView(); //렌더 타겟 뷰와 깊이-스텐실 뷰를 생성하는 함수이다. 
	void BuildObjects();
	void ReleaseObjects();

	//렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다.
	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	//CPU와 GPU를 동기화하는 함수이다.
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
		//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다.
};
