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
	, Damage(0.0f)
{}

void PlayerBullet::SetDamage(const float value)
{
	Damage = value;
}

void PlayerBullet::OnCollisionWall(const XMFLOAT3& reflection)
{}

void PlayerBullet::OnCollision(GameObject * other)
{}
