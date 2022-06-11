#pragma once
#include "IlluminatedScene.hpp"
#include "RawMesh.hpp"

class StageGame : public IlluminatedScene
{
public:
	StageGame(GameFramework& framework, HWND hwnd);
	virtual ~StageGame();

	void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
	void Start() override;
	void Reset() override;
	void Update(float delta_time) override;
	void PrepareRendering() override;
	void Render() override;

	void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
	void OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override;

	void ProcessInput(UCHAR* pKeysBuffer);

	const float roadWidth = 100.0f;
	const float roadHeight = 3000.0f;

	RawMesh roadData;
	CDiffusedMesh* roadMesh;
	XMFLOAT3 roadStartPoint;
	XMFLOAT3 roadDestPoint;
	XMFLOAT3 playerSpawnPoint;
	BoundingSphere myGoalie;

	const float defaultColor[4] = { 0.2f, 0.6f, 0.4f, 1.0f };

	const float raceColors[4][4];
	float raceTimer = 4.0f;

	HWND handleWindow;
};
