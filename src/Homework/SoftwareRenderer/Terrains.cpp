#include "stdafx.hpp"
#include "Terrains.hpp"
#include "GameObject.hpp"
#include "GameScene.hpp"
#include "GameMesh.hpp"

Rail::Rail(GameScene& scene, const XMFLOAT3& pos)
	: GameObject(scene, pos)
{
	SetMesh(scene.meshRail);
}

Pillar::Pillar(GameScene& scene, const XMFLOAT3& pos)
	: GameObject(scene, pos)
{
	SetMesh(scene.meshPlayer);
}
