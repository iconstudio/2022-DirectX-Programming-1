#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Player.h"
#include "Light.hpp"

class IlluminatedScene : public Scene
{
public:
	IlluminatedScene(GameFramework& framework, HWND hwnd, const char* name);
	virtual ~IlluminatedScene();

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmd_list) override;
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Update(float elapsed_time) override;
	virtual void Render() override;
	virtual void RenderUI(HDC surface) override;

	virtual void OnAwake() override;
	virtual void OnInialized() override;
	virtual void OnUpdate() override;
	virtual void OnRender() override;

	virtual void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	virtual void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
	virtual void OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override;

	CPlayer* myPlayer = nullptr;

protected:
	virtual P3DSignature CreateGraphicsRootSignature() override;
	virtual void ReleaseUploadBuffers() override;

	virtual void InitializeUniforms() override;
	virtual void UpdateUniforms() override;
	virtual void ReleaseUniforms() override;

	CLight* myLights = NULL;
	int numberLights = 0;

	XMFLOAT4 m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = NULL;
	StaticLights* myStaticLights = NULL;
};
