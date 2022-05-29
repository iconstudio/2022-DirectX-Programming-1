#pragma once
#include "IlluminatedScene.hpp"

class StageGame : public IlluminatedScene
{
public:
	StageGame(GameFramework& framework, HWND hwnd);

	void Awake(P3DDevice device, P3DGrpCommandList cmd_list) override;
	void Start() override;
	void Reset() override;
	void Update(float elapsed_time) override;
	void Render() override;
	void RenderUI(HDC surface) override;

	void OnAwake() override;
	void OnInialized() override;
	void OnUpdate() override;
	void OnRender() override;

	void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
	void OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override;

	void ProcessInput(UCHAR* pKeysBuffer);
};
