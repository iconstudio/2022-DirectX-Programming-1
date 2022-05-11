#include "stdafx.hpp"
#include "Player.hpp"
#include "GameScene.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"
#include "PlayerBullet.hpp"
#include "Terrains.hpp"

Player::Player()
	: GameObject()
	, myStatus(PLAYER_STATES::IDLE)
	, Window(NULL), Cursor(), Orientation(0)
	, Camera(nullptr)
	, shootDelay(0.0f), shootCooldown(0.3f), shootLocking(false)
	, myBulletShooted(0), myBulletMax(10), myBulletPool(myBulletMax)
{
	Friction = 30.0f;
}

Player::~Player()
{
	if (GetCapture() == Window)
	{
		ReleaseCapture();
	}
}

void Player::SetHwnd(HWND hwnd)
{
	Window = hwnd;
}

void Player::SetCamera(shared_ptr<GameCamera>& cam)
{
	Camera = cam;
}

void Player::AddBullet(PlayerBullet* bullet)
{
	bullet->Deactivate();
	bullet->SetParent(this);

	myBulletPool.push_back(bullet);
}

void Player::ReturnBullet(PlayerBullet* bullet)
{
	if (bullet)
	{
		myBulletShooted--;
	}
}

void Player::RideOn(RailBorder* entrance)
{
	if (PLAYER_STATES::NORMAL == myStatus)
	{
		myStatus = PLAYER_STATES::RIDING;

		SetPosition(entrance->GetPosition());
	}
}

void Player::TakeOff(RailBorder* exit)
{
	if (PLAYER_STATES::RIDING == myStatus)
	{
		myStatus = PLAYER_STATES::NORMAL;

		SetWorldMatrix(exit->Transform.GetWorldMatrix());
	}
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
	AddSpeed(accel, 0.3f); // 0.3m/s
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
	if (PLAYER_STATES::IDLE != myStatus)
	{
		if (NULL != Window)
		{
			POINT temp_cursor;
			GetCursorPos(&temp_cursor);

			auto delta_mx = float(temp_cursor.x - Cursor.x) * 0.25f;
			auto delta_my = float(temp_cursor.y - Cursor.y) * 0.25f;

			if (delta_mx || delta_my)
			{
				Rotate(delta_my * 0.5f, delta_mx, 0.0f);

				XMFLOAT3 myRight = GetRight();
				//TODO

				if (GetFocus() == Window)
				{
					SetCursorPos(Cursor.x, Cursor.y);
					GetCursorPos(&temp_cursor);
				}
			}

			Cursor = temp_cursor;
		}

		if (PLAYER_STATES::NORMAL == myStatus)
		{
			if (Orientation)
			{
				Crawl(Orientation, 10.0f * elapsed_time);
			}
		}

		if (0 < shootDelay)
		{
			shootDelay -= elapsed_time;
		}

		if (shootLocking)
		{
			if (shootDelay <= 0 && myBulletShooted < myBulletMax)
			{
				auto bullet = FindLastBullet();
				if (bullet)
				{
					bullet->Activate();
					bullet->SetWorldMatrix(Transform.GetWorldMatrix());
					bullet->SetDirection(XMFLOAT3(Transform.GetLook()));
					bullet->SetSpeed(1.0f);
					bullet->Ready();

					shootDelay = shootCooldown;
					myBulletShooted++;
				}
			}
		}
	}

	GameObject::Update(elapsed_time);
	Camera->Update(elapsed_time);
	Camera->GenerateViewMatrix();
}

void Player::OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		{
			if (PLAYER_STATES::IDLE == myStatus)
			{
				myStatus = PLAYER_STATES::NORMAL;
			}
			else if (PLAYER_STATES::DEAD != myStatus)
			{
				shootLocking = true;
				SetCapture(hwnd);
			}
		}
		break;

		case WM_RBUTTONDOWN:
		{
		}
		break;

		case WM_LBUTTONUP:
		{
			if (PLAYER_STATES::IDLE != myStatus && PLAYER_STATES::DEAD != myStatus)
			{
				shootLocking = false;

				if (GetCapture() == hwnd)
				{
					ReleaseCapture();
				}
			}
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
				case VK_ESCAPE:
				{
					Orientation = 0;
					shootLocking = false;
					GetCursorPos(&Cursor);
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
				case VK_CONTROL:
				{
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
				//ReleaseCapture();
			}
			else
			{
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

PlayerBullet* Player::FindLastBullet()
{
	for (const auto& bullet : myBulletPool)
	{
		if (bullet && !bullet->IsActivated())
		{
			return bullet;
		}
	}

	return nullptr;
}
