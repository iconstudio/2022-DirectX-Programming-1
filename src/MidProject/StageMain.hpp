#pragma once
#include "Scene.h"

class StageMain : public Scene
{
public:
	StageMain(GameFramework& framework);

	void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
	void Start() override;
	void Reset() override;
	void Update(float delta_time) override;
	void PrepareRendering() override;
	void Render() const override;

	void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
	void OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override;
};
