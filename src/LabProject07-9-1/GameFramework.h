#pragma once
#include "Timer.h"
#include "Player.h"
#include "Scene.h"

class GameFramework
{
public:
	GameFramework(unsigned int width, unsigned int height);
	~GameFramework();

	void Awake(HINSTANCE hinstance, HWND window);
	void CreateDirect3DDevice();
	void CreateCommanders();
	void CreateSwapChain();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void Start();

	void ToggleFullscreen();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE myHInstance;
	HWND myWindow;

	int frameWidth;
	int frameHeight;

	bool isAntiAliasingEnabled;
	UINT levelAntiAliasing;
	UINT indexFrameBuffer;

	static const UINT numberFrameBuffers = 2;

	IDXGIFactory4* myFactory;
	IDXGISwapChain3* mySwapChain;
	ID3D12Device* myDevice;

	ID3D12GraphicsCommandList* myCommandList;
	ID3D12CommandQueue* myCommandQueue;
	ID3D12CommandAllocator* myCommandAlloc;

	ID3D12Resource* resSwapChainBackBuffers[numberFrameBuffers];
	ID3D12DescriptorHeap* heapRtvDesc;
	UINT szRtvDescIncrements;

	ID3D12Resource* myDepthStencilBuffer;
	ID3D12DescriptorHeap* heapDsvDesc;
	UINT szDsvDescIncrements;

	ID3D12Fence* myRenderFence;
	UINT64 myFences[numberFrameBuffers];
	HANDLE eventFence;

#if defined(_DEBUG)
	ID3D12Debug* myDebugController;
#endif

	CGameTimer m_GameTimer;

	CScene* m_pScene = NULL;
	CPlayer* m_pPlayer = NULL;
	GameCamera* m_pCamera = NULL;

	POINT m_ptOldCursorPos;

	_TCHAR m_pszFrameRate[70];
};

