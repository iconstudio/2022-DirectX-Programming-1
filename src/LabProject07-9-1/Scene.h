#pragma once
#include "Shader.h"
#include "Player.h"
#include "Light.hpp"

class Scene
{
public:
	Scene(GameFramework& framework, const char* name);
	virtual ~Scene();

	virtual void Awake(HWND hwnd, P3DDevice device, P3DGrpCommandList cmd_list) = 0;

	virtual void Start() = 0;
	virtual void Reset() = 0;
	virtual void Update(float time_elapsed) = 0;
	virtual void Render(GameCamera* pCamera = nullptr) = 0;
};

class CScene
{
public:
	CScene(GameFramework& framework, const char* name);
	virtual ~CScene();

	// 초기화
	void Awake(HWND hwnd, P3DDevice device, P3DGrpCommandList cmd_list);
	virtual void Build();

	virtual ID3D12RootSignature* CreateGraphicsRootSignature();
	virtual void InitializeUniforms();
	virtual void ReleaseUploadBuffers();

	// 시작
	virtual void Start();
	virtual void Reset();

	// 갱신
	virtual void Update(float time_elapsed);
	virtual void UpdateUniforms();
	virtual bool ProcessInput(UCHAR* pKeysBuffer);

	// 렌더링
	void Render(GameCamera* pCamera = nullptr);

	// 이름 얻기
	virtual const std::string& GetName() const noexcept;
	// 파이프라인의 쉐이더 서명 얻기
	ID3D12RootSignature* GetGraphicsRootSignature();
	ID3D12RootSignature const* GetGraphicsRootSignature() const;

	// 메모리 해제
	virtual void ReleaseUniforms();

	// 이벤트
	virtual void OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) = 0;
	virtual void OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state) = 0;

	CPlayer* m_pPlayer = NULL;

public:
	const std::string myName;

	GameFramework& myFramework;

	HWND myWindow;
	P3DDevice d3dDevice;
	P3DGrpCommandList d3dTaskList;
	ID3D12RootSignature* d3dShaderParameters;

	std::vector<shared_ptr<GameObject>> myInstances;

	CLight* m_pLights = NULL;
	int m_nLights = 0;

	XMFLOAT4 m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	POINT posCursor;
	float lastDeltaTime = 0.0f;
};
