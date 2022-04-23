#include "stdafx.hpp"
#include "Player.hpp"
#include "GameScene.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"

Player::Player(GameScene& scene)
	: GameObject(scene)
	, Window(NULL), Cursor()
	, m_xmf3CameraOffset(), m_xmf3Velocity()
{}

Player::~Player()
{}

void Player::SetHwnd(HWND hwnd)
{
	Window = hwnd;
}

void Player::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);

	GameObject::SetPosition(x, y, z);
}

void Player::SetPosition(const XMFLOAT3& xmf3Position)
{
	m_xmf3Position = xmf3Position;

	GameObject::SetPosition(XMFLOAT3(xmf3Position));
}

void Player::SetPosition(XMFLOAT3&& xmf3Position)
{
	m_xmf3Position = xmf3Position;

	GameObject::SetPosition(xmf3Position);
}

void Player::SetRotation(float x, float y, float z)
{}

void Player::SetCameraOffset(const XMFLOAT3& xmf3CameraOffset)
{
	SetCameraOffset(std::move(XMFLOAT3(xmf3CameraOffset)));
}

void Player::SetCameraOffset(XMFLOAT3&& xmf3CameraOffset)
{
	m_xmf3CameraOffset = xmf3CameraOffset;

	Camera->SetLookAt(Vector3::Add(m_xmf3Position, m_xmf3CameraOffset), m_xmf3Position, m_xmf3Up);
	Camera->GenerateViewMatrix();
}

void Player::Move(DWORD dwDirection, float fDistance)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(std::move(xmf3Shift), true);
	}
}

void Player::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	Move(std::move(XMFLOAT3(xmf3Shift)), bUpdateVelocity);
}

void Player::Move(XMFLOAT3&& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(xmf3Shift, m_xmf3Position);
		Camera->Move(xmf3Shift);
	}
}

void Player::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z), false);
}

void Player::Rotate(float fPitch, float fYaw, float fRoll)
{
	Camera->Rotate(fPitch, fYaw, fRoll);
	//GameObject::Rotate(fPitch, fYaw, fRoll);

	//*
	if (fPitch != 0.0f) // x
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
	}
	if (fYaw != 0.0f) // y
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f) // z
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	m_xmf3Up = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));
	//*/
}

void Player::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void Player::Update(float fTimeElapsed)
{
	if (NULL != Window && GetCapture() == Window)
	{
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);

		float cxMouseDelta = (float)(ptCursorPos.x - Cursor.x) / 3.0f;
		float cyMouseDelta = (float)(ptCursorPos.y - Cursor.y) / 3.0f;
		SetCursorPos(Cursor.x, Cursor.y);
		//Cursor = ptCursorPos;

		if (cxMouseDelta || cyMouseDelta)
		{
			//if (pKeyBuffer[VK_RBUTTON] & 0xF0)
			//Rotate(cyMouseDelta, 0.0f, -cxMouseDelta);
			//else
			//Rotate(0, cxMouseDelta, 0.0f);
			Rotate(cyMouseDelta, cxMouseDelta, 0.0f);
		}
	}

	Move(m_xmf3Velocity, false);

	Camera->Update(this, m_xmf3Position, fTimeElapsed);
	Camera->GenerateViewMatrix();

	XMFLOAT3 xmf3Deceleration = Vector3::Normalize(Vector3::ScalarProduct(m_xmf3Velocity, -1.0f));
	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = m_fFriction * fTimeElapsed;
	if (fDeceleration > fLength) fDeceleration = fLength;

	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Deceleration, fDeceleration);

	GameObject::Update(fTimeElapsed);

	OnUpdateTransform();
}

void Player::OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		break;

		case WM_RBUTTONDOWN:
		{
			SetCapture(hwnd);
			GetCursorPos(&Cursor);
		}
		break;

		case WM_LBUTTONUP:
		break;

		case WM_RBUTTONUP:
		{
			ReleaseCapture();
		}
		break;

		case WM_MOUSEMOVE:
		break;
	}
}

void Player::OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	DWORD dwDirection = 0;

	switch (msg)
	{
		case WM_KEYDOWN:
		{
			switch (wp)
			{
				case 'A':
				{
					dwDirection |= DIR_LEFT;
				}
				break;
				case 'D':
				{
					dwDirection |= DIR_RIGHT;
				}
				break;
				case 'W':
				{
					dwDirection |= DIR_FORWARD;
				}
				break;
				case 'S':
				{
					dwDirection |= DIR_BACKWARD;
				}
				break;
			}
		}
		break;

		case WM_KEYUP:
		{
		}
		break;
	}

	if (dwDirection)
	{
		Move(dwDirection, 0.15f);
	}
}

void Player::OnUpdateTransform()
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
}
