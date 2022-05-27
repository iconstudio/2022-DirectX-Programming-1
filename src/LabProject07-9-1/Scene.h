#pragma once
#include "TaggedObject.hpp"
#include "Shader.h"
#include "Player.h"
#include "Light.hpp"

class CScene : public TaggedObject
{
public:
	CScene(GameFramework& framework, const char* name);
	virtual ~CScene();

	// 초기화
	void Awake(PtrDevice device, PtrGrpCommandList  cmd_list);
	virtual void Build();

	// 시작
	virtual void Start();
	virtual void Reset();
	virtual void Restart();

	// 갱신
	virtual void Update(float time_elapsed);
	virtual void UpdateShaderVariables();

	// 렌더링
	void Render(GameCamera* pCamera = nullptr);

	ID3D12RootSignature* GetGraphicsRootSignature();
	ID3D12RootSignature const* GetGraphicsRootSignature() const;

	//
	virtual void ReleaseUploadBuffers();
	virtual void ReleaseShaderVariables();

	bool OnMouseEvent(HWND hWnd, UINT msg, WPARAM btn, LPARAM info);
	bool OnKeyboardEvent(HWND hWnd, UINT msg, WPARAM key, LPARAM state);
	bool OnWindowsEvent(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	CPlayer* m_pPlayer = NULL;

private:
	ID3D12RootSignature* CreateGraphicsRootSignature();
	virtual void CreateShaderVariables();

	PtrDevice d3dDevice;
	PtrGrpCommandList  d3dTaskList;
	ID3D12RootSignature* d3dShaderParameters;
	unique_ptr<CShader> myIlluminationShader;

	std::vector<shared_ptr<GameObject>> myInstances;

	CLight* m_pLights = NULL;
	int m_nLights = 0;

	XMFLOAT4 globalAmbientLight;
	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	float lastDeltaTime = 0.0f;
};
