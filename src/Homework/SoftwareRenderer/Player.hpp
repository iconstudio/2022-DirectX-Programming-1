#pragma once
#include "stdafx.hpp"
#include "GameScene.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"

class Player : public GameObject
{
public:
	Player(GameScene& scene);
	virtual ~Player();

	void SetHwnd(HWND hwnd);
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(const XMFLOAT3& xmf3Position);
	virtual void SetPosition(XMFLOAT3&& xmf3Position);
	void SetRotation(float x, float y, float z);
	void SetCameraOffset(const XMFLOAT3& xmf3CameraOffset);
	void SetCameraOffset(XMFLOAT3&& xmf3CameraOffset);

	void Move(DWORD dwDirection, float fDistance);
	void Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(XMFLOAT3&& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	virtual void Update(float fTimeElapsed);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnUpdateTransform();

	friend class GameCamera;

private:
	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3 m_xmf3CameraOffset;
	XMFLOAT3 m_xmf3Velocity;
	float m_fFriction = 125.0f;

	float m_fPitch = 0.0f;
	float m_fYaw = 0.0f;
	float m_fRoll = 0.0f;

	HWND Window;
	POINT Cursor;
};