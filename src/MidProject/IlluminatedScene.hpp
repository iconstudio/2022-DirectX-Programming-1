#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Player.h"
#include "Light.hpp"

class IlluminatedScene : public Scene
{
public:
	IlluminatedScene(GameFramework& framework, const char* name);
	virtual ~IlluminatedScene();

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Update(float delta_time) override;
	virtual void PrepareRendering() override;
	virtual void Render() override;

	virtual void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	virtual void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
	virtual void OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override;

	CAirplanePlayer* myPlayer = nullptr;

protected:
	virtual P3DSignature CreateGraphicsRootSignature() override;

	CLight* myLights = NULL;
	int numberLights = 0;

	XMFLOAT4 m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = NULL;
	StaticLights* myStaticLights = NULL;
};
