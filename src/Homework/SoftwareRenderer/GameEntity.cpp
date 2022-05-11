#include "stdafx.hpp"
#include "GameEntity.hpp"
#include "GamePipeline.hpp"
#include "GameTransform.hpp"
#include "GameMesh.hpp"

GameEntity::GameEntity()
	: isActivated(true)
	, myMesh(), Transform(), Collider()
{}

GameEntity::GameEntity(float x, float y, float z)
	: GameEntity()
{
	SetPosition(x, y, z);
}

GameEntity::GameEntity(float list[3])
{
	SetPosition(list[0], list[1], list[2]);
}

GameEntity::GameEntity(const XMFLOAT3& position)
	: GameEntity()
{
	SetPosition(position);
}

GameEntity::GameEntity(XMFLOAT3&& position)
	: GameEntity()
{
	SetPosition(position);
}

GameEntity::~GameEntity()
{}

void GameEntity::SetActive(bool flag)
{
	isActivated = flag;
	if (flag)
	{
		UpdateBoundingBox();
	}
}

void GameEntity::SetMesh(const shared_ptr<CMesh>& mesh)
{
	myMesh.SetMesh(mesh);
}

void GameEntity::SetColor(COLORREF color)
{
	myMesh.SetColor(color);
}

void GameEntity::Activate()
{
	SetActive(true);
}

void GameEntity::Deactivate()
{
	SetActive(false);
}

bool GameEntity::IsActivated() const
{
	return isActivated;
}

void GameEntity::SetWorldMatrix(XMFLOAT4X4&& tfrm)
{
	Transform.SetWorldMatrix(std::forward<XMFLOAT4X4>(tfrm));
}

void GameEntity::SetWorldMatrix(const XMFLOAT4X4& tfrm)
{
	Transform.SetWorldMatrix(tfrm);
}

void GameEntity::SetPosition(float x, float y, float z)
{
	Transform.SetPosition(x, y, z);
}

void GameEntity::SetPosition(const XMFLOAT3& pos)
{
	SetPosition(std::move(XMFLOAT3(pos)));
}

void GameEntity::SetPosition(XMFLOAT3&& pos)
{
	Transform.SetPosition(std::forward<XMFLOAT3>(pos));
}

void GameEntity::SetRotation(const XMFLOAT4X4& tfrm)
{
	SetRotation(std::move(XMFLOAT4X4(tfrm)));
}

void GameEntity::SetRotation(XMFLOAT4X4&& tfrm)
{
	Transform.SetRotation(std::forward<XMFLOAT4X4>(tfrm));
}

void GameEntity::LookTo(XMFLOAT3& to, XMFLOAT3& up)
{
	Transform.LookTo(to, up);
}

void GameEntity::LookAt(XMFLOAT3& from, XMFLOAT3& up)
{
	Transform.LookAt(from, up);
}

void GameEntity::AddPosition(XMFLOAT3&& vector)
{
	Transform.Translate(vector);
}

void GameEntity::Move(const XMFLOAT3& dir, float distance)
{
	Transform.Move(dir, distance);
}

void GameEntity::MoveStrafe(float distance)
{
	Transform.MoveStrafe(distance);
}

void GameEntity::MoveUp(float distance)
{
	Transform.MoveUp(distance);
}

void GameEntity::MoveForward(float distance)
{
	Transform.MoveForward(distance);
}

void GameEntity::Rotate(float pitch, float yaw, float roll)
{
	Transform.Rotate(pitch, yaw, roll);
}

void GameEntity::Rotate(const XMFLOAT3& axis, float angle)
{
	Transform.Rotate(axis, angle);
}

XMFLOAT3&& GameEntity::GetPosition() const
{
	return std::move(XMFLOAT3(Transform.GetPosition()));
}

XMFLOAT3&& GameEntity::GetRight() const
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetRight()));
}

XMFLOAT3&& GameEntity::GetUp() const
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetUp()));
}

XMFLOAT3&& GameEntity::GetLook() const
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetLook()));
}

void GameEntity::PrepareRendering(GameScene& scene) const
{
	if (myMesh.IsAvailable())
	{
		GamePipeline::SetWorldTransform(Transform.GetWorldMatrix());

		myMesh.PrepareRendering(scene);
	}
}

void GameEntity::Render(HDC surface) const
{
	if (myMesh.IsAvailable())
	{
		GamePipeline::SetWorldTransform(Transform.GetWorldMatrix());

		myMesh.RenderByFragments(surface);
	}
}

void GameEntity::UpdateBoundingBox()
{
	if (myMesh.IsAvailable())
	{
		const auto& mat = Transform.GetWorldMatrix();
		const auto float4x4 = DirectX::XMLoadFloat4x4(&mat);

		const auto& original_collider = myMesh.GetCollider();
		original_collider.Transform(Collider, float4x4);

		const auto orientation = DirectX::XMLoadFloat4(&Collider.Orientation);
		const auto quaternion = DirectX::XMQuaternionNormalize(orientation);
		DirectX::XMStoreFloat4(&Collider.Orientation, quaternion);
	}
}
