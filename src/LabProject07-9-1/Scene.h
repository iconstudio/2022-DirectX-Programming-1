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

	// �ʱ�ȭ
	void Awake(HWND hwnd, P3DDevice device, P3DGrpCommandList cmd_list);
	virtual void Build();

	virtual ID3D12RootSignature* CreateGraphicsRootSignature();
	virtual void InitializeUniforms();
	virtual void ReleaseUploadBuffers();

	// ����
	virtual void Start();
	virtual void Reset();

	// ����
	virtual void Update(float time_elapsed);
	virtual void UpdateUniforms();
	virtual bool ProcessInput(UCHAR* pKeysBuffer);

	// ������
	void Render(GameCamera* pCamera = nullptr);

	// �̸� ���
	virtual const std::string& GetName() const noexcept;
	// ������������ ���̴� ���� ���
	ID3D12RootSignature* GetGraphicsRootSignature();
	ID3D12RootSignature const* GetGraphicsRootSignature() const;

	// �޸� ����
	virtual void ReleaseUniforms();

	// �̺�Ʈ
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
