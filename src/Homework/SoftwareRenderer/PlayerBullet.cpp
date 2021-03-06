#include "stdafx.hpp"
#include "PlayerBullet.hpp"
#include "Player.hpp"
#include "GameObject.hpp"

PlayerBullet::PlayerBullet()
	: PlayerBullet(XMFLOAT3(0, 0, 0))
{}

PlayerBullet::PlayerBullet(const XMFLOAT3& pos)
	: GameObject(pos)
	, myParent(nullptr)
	, myDamage(0.0f)
	, myDuration(3.0f), myLifetime(3.0f)
{
	SetColor(RGB(50, 50, 100));
}

void PlayerBullet::SetDamage(const float value)
{
	myDamage = value;
}

void PlayerBullet::Ready()
{
	Activate();
	myLifetime = myDuration;
}

void PlayerBullet::Return()
{
	Deactivate();

	if (myParent)
	{
		myParent->ReturnBullet(this);
	}
}

void PlayerBullet::SetParent(Player* parent)
{
	myParent = parent;
}

void PlayerBullet::Update(float elapsed_time)
{
	if (0 < myLifetime)
	{
		myLifetime -= elapsed_time;
	}
	else
	{
		Return();
	}

	GameObject::Update(elapsed_time);
}

void PlayerBullet::OnCollisionWall(const XMFLOAT3& reflection)
{}

void PlayerBullet::OnCollisionEnter(GameObject* other)
{
	const auto tag = other->GetTag();
	if (GOBJECT_TAGS::ENEMY == tag)
	{
		Return();
	}
}
