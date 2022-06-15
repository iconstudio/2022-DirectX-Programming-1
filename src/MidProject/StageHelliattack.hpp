#pragma once
#include "IlluminatedScene.hpp"
#include "RawMesh.hpp"
#include "Terrains.hpp"

class StageHelliattack : public IlluminatedScene
{
public:
	StageHelliattack(GameFramework& framework, HWND hwnd);
	virtual ~StageHelliattack();

	void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
	void Start() override;
	void Reset() override;
	void Update(float delta_time) override;
	void PrepareRendering() override;
	void Render() const override;

	void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
	void OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override;

	void ProcessInput(UCHAR* pKeysBuffer);

	float globalTime;

	XMFLOAT3 playerSpawnPoint;
	XMFLOAT3 playerLightRotations[2];

	Terrain myTerrain;
	XMFLOAT4X4 matrixTerrain;
	float worldWidth, worldHeight;


	HWND handleWindow;
};
