#pragma once
#include "Shader.h"
#include "Player.h"
#include "Light.hpp"

class CScene
{
public:
	CScene(GameFramework& framework, const char* name);
	virtual ~CScene();

	// 초기화
	void Awake(HWND hwnd, ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list);
	virtual void Build();

	ID3D12RootSignature* CreateGraphicsRootSignature();
	virtual void CreateShaderVariables();
	virtual void ReleaseUploadBuffers();

	// 시작
	virtual void Start();
	virtual void Reset();
	virtual void Restart();

	// 갱신
	virtual void Update(float time_elapsed);
	virtual void UpdateShaderVariables();
	virtual bool ProcessInput(UCHAR* pKeysBuffer);

	// 렌더링
	void Render(GameCamera* pCamera = nullptr);

	// 이름 얻기
	virtual const char* GetName() const noexcept { return "Scene"; };
	// 파이프라인의 쉐이더 서명 얻기
	ID3D12RootSignature* GetGraphicsRootSignature() { return(d3dShaderParameters); }

	//
	virtual void ReleaseShaderVariables();

	bool OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info);
	bool OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state);

	CPlayer* m_pPlayer = NULL;

public:
	const std::string myName;

	GameFramework& myFramework;

	HWND myWindow;
	ID3D12Device* d3dDevice;
	ID3D12GraphicsCommandList* d3dTaskList;
	ID3D12RootSignature* d3dShaderParameters;

	std::vector<shared_ptr<GameObject>> myInstances;

	CLight* m_pLights = NULL;
	int m_nLights = 0;

	XMFLOAT4 m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	POINT m_ptOldCursorPos;
	float lastDeltaTime = 0.0f;
};
