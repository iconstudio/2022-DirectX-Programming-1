#include "stdafx.hpp"
#include "Player.hpp"
#include "GameScene.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"

Player::Player(GameScene& scene)
	: GameObject(scene)
	, Window(NULL), Cursor(), Orientation(0), focused(false)
	, cameraOffset(), cameraLookAtOffset(), cameraLookDistance(600.0f)
{
	Friction = 30.0f;
}

Player::~Player()
{}

void Player::SetHwnd(HWND hwnd)
{
	Window = hwnd;
}

void Player::SetCameraOffset(const XMFLOAT3& xmf3CameraOffset)
{
	SetCameraOffset(std::move(XMFLOAT3(xmf3CameraOffset)));
}

void Player::SetCameraOffset(XMFLOAT3&& xmf3CameraOffset)
{
	cameraOffset = xmf3CameraOffset;

	const auto pos = XMFLOAT3(Transform.GetPosition());
	const auto right = XMFLOAT3(Transform.GetRight());
	const auto up = XMFLOAT3(Transform.GetUp());
	const auto look = XMFLOAT3(Transform.GetLook());

	const XMFLOAT3 look_from = Vector3::Add(pos, cameraOffset);
	const XMFLOAT3 look_at = Vector3::Add(pos, look, cameraLookDistance);

	const XMFLOAT3 dir = Vector3::Subtract(look_at, look_from);
	const XMFLOAT3 cam_up = Vector3::CrossProduct(dir, right);

	Camera->SetLookAt(look_from, look_at, up);
	Camera->GenerateViewMatrix();
}

void Player::Crawl(DWORD dwdir, float accel)
{
	XMFLOAT3 dir = XMFLOAT3(0, 0, 0);

	auto pos = XMFLOAT3(Transform.GetPosition());
	auto right = XMFLOAT3(Transform.GetRight());
	auto up = XMFLOAT3(Transform.GetUp());
	auto look = XMFLOAT3(Transform.GetLook());

	if (dwdir & DIR_UP) dir = Vector3::Add(dir, up, 1.0f);
	if (dwdir & DIR_DOWN) dir = Vector3::Add(dir, up, -1.0f);

	if (dwdir & DIR_RIGHT) dir = Vector3::Add(dir, right, 1.0f);
	if (dwdir & DIR_LEFT) dir = Vector3::Add(dir, right, -1.0f);

	if (dwdir & DIR_FORWARD) dir = Vector3::Add(dir, look, 1.0f);
	if (dwdir & DIR_BACKWARD) dir = Vector3::Add(dir, look, -1.0f);

	SetDirection(dir);
	AddSpeed(accel, 0.1f); // 10m/s
}

void Player::Move(const XMFLOAT3& vDirection, float distance)
{
	Camera->Move(vDirection, distance);
	GameObject::Move(vDirection, distance);
}

void Player::Rotate(float pitch, float yaw, float roll)
{
	Camera->Rotate(pitch, yaw, roll);
	GameObject::Rotate(pitch, yaw, roll);
}

void Player::Update(float elapsed_time)
{
	if (NULL != Window && focused)
	{
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);

		float cxMouseDelta = (float)(ptCursorPos.x - Cursor.x) / 4.0f;
		float cyMouseDelta = (float)(ptCursorPos.y - Cursor.y) / 4.0f;
		Cursor = ptCursorPos;

		if (cxMouseDelta || cyMouseDelta)
		{
			Rotate(0.0f, cxMouseDelta, 0.0f);
		}
	}

	if (Orientation)
	{
		Crawl(Orientation, 10.0f * elapsed_time); // 1000m/s^2
	}

	GameObject::Update(elapsed_time);
	Camera->Update(cameraOffset, elapsed_time);
	Camera->GenerateViewMatrix();

	OnUpdateTransform();
}

void Player::OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		{
		}
		break;

		case WM_RBUTTONDOWN:
		{
		}
		break;

		case WM_LBUTTONUP:
		{
		}
		break;

		case WM_RBUTTONUP:
		{
		}
		break;

		case WM_MOUSEMOVE:
		break;
	}
}

void Player::OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			switch (wp)
			{
				case 'A':
				{
					Orientation |= DIR_LEFT;
				}
				break;
				case 'D':
				{
					Orientation |= DIR_RIGHT;
				}
				break;
				case 'W':
				{
					Orientation |= DIR_FORWARD;
				}
				break;
				case 'S':
				{
					Orientation |= DIR_BACKWARD;
				}
				break;
			}
		}
		break;

		case WM_KEYUP:
		{
			switch (wp)
			{
				case 'A':
				{
					Orientation &= 0xff & ~DIR_LEFT;
				}
				break;
				case 'D':
				{
					Orientation &= 0xff & ~DIR_RIGHT;
				}
				break;
				case 'W':
				{
					Orientation &= 0xff & ~DIR_FORWARD;
				}
				break;
				case 'S':
				{
					Orientation &= 0xff & ~DIR_BACKWARD;
				}
				break;
			}
		}
		break;
	}
}

void Player::OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_ACTIVATE:
		{
			const auto act = LOWORD(wp);

			if (WA_INACTIVE == act)
			{
				focused = false;
				//ReleaseCapture();
			}
			else
			{
				focused = true;
				//SetCapture(hwnd);
				GetCursorPos(&Cursor);
			}
		}
		break;

		case WM_SIZE:
		{
		}
		break;
	}
}

void Player::OnUpdateTransform()
{
	/*
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;

	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;

	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;

	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
	*/
}
