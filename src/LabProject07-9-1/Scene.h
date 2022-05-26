#pragma once
#include "Shader.h"
#include "Player.h"
#include "Light.hpp"

class CScene
{
public:
	CScene(const char* name);
	~CScene();

	// 초기화
	virtual void Awake(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list);
	virtual void CreateShaderVariables();
	virtual void UpdateShaderVariables();
	virtual void ReleaseShaderVariables();
	virtual void BuildDefaultLightsAndMaterials();
	virtual void BuildObjects();

	// 시작
	virtual void Start();
	virtual void Reset();
	virtual void Restart();

	// 갱신
	virtual void Update(float time_elapsed);
	bool ProcessInput(UCHAR* pKeysBuffer);

	virtual const char* GetName() const noexcept { return 0; };

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	void Render(GameCamera* pCamera = nullptr);

	void ReleaseUploadBuffers();

	bool OnMouseEvent(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnKeyboardEvent(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	CPlayer* m_pPlayer = NULL;

public:
	const char* myName;

	ID3D12Device* d3dDevice;
	ID3D12GraphicsCommandList* d3dTaskList;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	GameObject** m_ppGameObjects = NULL;
	int							m_nGameObjects = 0;


	CLight* m_pLights = NULL;
	int							m_nLights = 0;

	XMFLOAT4					m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	float						m_fElapsedTime = 0.0f;
};
