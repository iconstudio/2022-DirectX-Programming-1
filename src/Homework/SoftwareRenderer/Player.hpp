#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"

class Player : public GameObject
{
public:
	Player(GameScene& scene);
	virtual ~Player();

	void SetHwnd(HWND hwnd);
	void SetCameraOffset(const XMFLOAT3& xmf3CameraOffset);
	void SetCameraOffset(XMFLOAT3&& xmf3CameraOffset);

	void Crawl(DWORD dwdir, float accel);

	virtual void Move(const XMFLOAT3& vDirection, float distance);
	virtual void Rotate(float pitch, float yaw, float roll);

	virtual void Update(float elapsed_time);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnUpdateTransform();

	friend class GameCamera;

private:
	XMFLOAT3 cameraOffset, cameraLookAtOffset;
	float cameraLookDistance;

	HWND Window;
	POINT Cursor;
	DWORD Orientation;
	bool focused;
};
