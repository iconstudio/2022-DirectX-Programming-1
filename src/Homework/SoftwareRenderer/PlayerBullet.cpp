#include "stdafx.hpp"
#include "PlayerBullet.hpp"
#include "GameScene.hpp"
#include "GameObject.hpp"
#include "CubeMesh.hpp"

PlayerBullet::PlayerBullet(GameScene& scene)
	: PlayerBullet(scene, XMFLOAT3(0, 0, 0))
{}

PlayerBullet::PlayerBullet(GameScene& scene, const XMFLOAT3& pos)
	: GameObject(scene, pos)
	, myParent(nullptr)
	, myDamage(0.0f)
	, myDuration(3.0f), myLifetime(myDuration)
{
	SetMesh(scene.meshPlayerBullet);
}

void PlayerBullet::SetDamage(const float value)
{
	myDamage = value;
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
		Deactivate();

		if (myParent)
		{
			myParent->ReturnBullet(this);
		}
	}

	GameObject::Update(elapsed_time);
}

void PlayerBullet::OnCollisionWall(const XMFLOAT3& reflection)
{}

void PlayerBullet::OnCollision(GameObject * other)
{}
