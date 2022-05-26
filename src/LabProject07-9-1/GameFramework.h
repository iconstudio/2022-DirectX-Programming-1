#pragma once
#include "Player.h"
#include "Scene.h"

class GameFramework
{
public:
	GameFramework(unsigned int width, unsigned int height);
	~GameFramework();

	// ���̷�ƮX �ʱ�ȭ
	void Awake(HINSTANCE hinstance, HWND window);
	void CreateDirect3DDevice();
	void CreateCommanders();
	void CreateSwapChain();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	// ���� �ʱ�ȭ
	void Start();
	void BuildWorld();
	void BuildParticles();
	void BuildPlayer();
	void BuildTerrains();
	void BuildObjects();

	// ����
	void Update(float elapsed_time);

	// ������
	void Render();
	void PrepareRendering();
	void AfterRendering();

	// ����ȭ
	void WaitForGpuComplete();

	// ��üȭ�� ��ȯ
	void ToggleFullscreen();

	void OnMouseEvent(HWND hWnd, UINT msg, WPARAM btn, LPARAM info);
	void OnKeyboardEvent(HWND hWnd, UINT msg, WPARAM key, LPARAM state);
	LRESULT CALLBACK OnWindowsEvent(HWND, UINT msg, WPARAM wp, LPARAM lp);

private:
	bool D3DAssert(HRESULT valid, const char* error);

	void ResetCmdList(ID3D12PipelineState* pipeline = nullptr);
	void CloseCmdList();
	void ExecuteCmdList(ID3D12CommandList* list[], UINT count);

	void SetFenceEvent(HANDLE signal, UINT64 limit);
	void SignalToFence(UINT64 count);

	HINSTANCE myAppInstance;
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

	CScene* m_pScene = NULL;
	CPlayer* m_pPlayer = NULL;
	GameCamera* m_pCamera = NULL;

	POINT m_ptOldCursorPos;
};

