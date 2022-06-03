#pragma once
#include "IlluminatedScene.hpp"

class StageGame : public IlluminatedScene
{
public:
	StageGame(Framework& framework, HWND hwnd);

	void Awake(P3DDevice device, P3DGrpCommandList cmd_list) override;
	void Start() override;
	void Reset() override;
	void Update(float elapsed_time) override;
	void Render() override;

	void OnAwake() override;
	void OnInialized() override;
	void OnUpdate() override;
	void OnRender() override;

	void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
	void OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override;

	void ProcessInput(UCHAR* pKeysBuffer);

	const float roadWidth = 100.0f;
	const float roadHeight = 3000.0f;

	BoundingSphere myGoal;

	XMFLOAT3 playerSpawnPoint;

	std::vector<GameObject*> myWalls;

	const float defaultColor[4] = { 0.2f, 0.6f, 0.4f, 1.0f };

	const float raceColors[4][4];
	float raceTimer = 4.0f;
};
