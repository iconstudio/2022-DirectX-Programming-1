#include "pch.hpp"
#include "GameEntity.hpp"
#include "Arithmetics.hpp"

GameEntity::GameEntity()
	: GameObject()
	, transformModified(false)
	, Direction(XMFLOAT3(0.0f, 0.0f, 0.0f)), Speed(0.0f), Friction(0.0f)
{}

GameEntity::GameEntity(float x, float y, float z)
	: GameEntity()
{
	SetPosition(x, y, z);
}

GameEntity::GameEntity(float list[3])
	: GameEntity()
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

void GameEntity::SetWorldMatrix(XMFLOAT4X4&& tfrm)
{
	GameObject::SetWorldMatrix(std::forward<XMFLOAT4X4>(tfrm));
	OnUpdateTransform();
}

void GameEntity::SetWorldMatrix(const XMFLOAT4X4& tfrm)
{
	GameObject::SetWorldMatrix(tfrm);
	OnUpdateTransform();
}

void GameEntity::SetPosition(float x, float y, float z)
{
	GameObject::SetPosition(x, y, z);
	OnUpdateTransform();
}

void GameEntity::SetPosition(const XMFLOAT3& pos)
{
	SetPosition(std::move(XMFLOAT3(pos)));
}

void GameEntity::SetPosition(XMFLOAT3&& pos)
{
	GameObject::SetPosition(std::forward<XMFLOAT3>(pos));
	OnUpdateTransform();
}

void GameEntity::SetRotation(const XMFLOAT4X4& tfrm)
{
	GameObject::SetRotation(tfrm);
	OnUpdateTransform();
}

void GameEntity::SetRotation(XMFLOAT4X4&& tfrm)
{
	GameObject::SetRotation(std::forward<XMFLOAT4X4>(tfrm));
	OnUpdateTransform();
}

void GameEntity::AddPosition(XMFLOAT3&& vector)
{
	GameObject::AddPosition(std::forward<XMFLOAT3>(vector));
	OnUpdateTransform();
}

constexpr bool GameEntity::IsStatic() const
{
	return true;
}

void GameEntity::Move(const XMFLOAT3& dir, float distance)
{
	GameObject::Move(dir, distance);
	OnUpdateTransform();
}

void GameEntity::MoveStrafe(float distance)
{
	GameObject::MoveStrafe(distance);
	OnUpdateTransform();
}

void GameEntity::MoveUp(float distance)
{
	GameObject::MoveUp(distance);
	OnUpdateTransform();
}

void GameEntity::MoveForward(float distance)
{
	GameObject::MoveForward(distance);
	OnUpdateTransform();
}

void GameEntity::Rotate(float pitch, float yaw, float roll)
{
	GameObject::Rotate(pitch, yaw, roll);
	OnUpdateTransform();
}

void GameEntity::Rotate(const XMFLOAT3& axis, float angle)
{
	GameObject::Rotate(axis, angle);
	OnUpdateTransform();
}

void GameEntity::LookTo(XMFLOAT3& to, XMFLOAT3& up)
{
	GameObject::LookTo(to, up);
	OnUpdateTransform();
}

void GameEntity::LookAt(XMFLOAT3& from, XMFLOAT3& up)
{
	GameObject::LookAt(from, up);
	OnUpdateTransform();
}

void GameEntity::SetVelocity(const XMFLOAT3& vector)
{
	SetVelocity(std::move(XMFLOAT3(vector)));
}

void GameEntity::SetVelocity(XMFLOAT3&& vector)
{
	const XMFLOAT3 vel = std::forward<XMFLOAT3>(vector);
	Direction = Vector3::Normalize(vel);
	Speed = Vector3::Length(vel);
}

void GameEntity::SetDirection(const XMFLOAT3& direction)
{
	SetDirection(std::move(XMFLOAT3(direction)));
}

void GameEntity::SetDirection(XMFLOAT3&& direction)
{
	Direction = Vector3::Normalize(std::forward<XMFLOAT3>(direction));
}

void GameEntity::SetSpeed(const float value)
{
	Speed = value;
}

void GameEntity::AddSpeed(const float value, const float max)
{
	auto speed = GetSpeed() + value;
	if (max < speed) speed = max;

	SetSpeed(speed);
}

void GameEntity::AddSpeed(const float value)
{
	SetSpeed(GetSpeed() + value);
}

float GameEntity::GetSpeed() const
{
	return Speed;
}

void GameEntity::SetRotationAxis(const XMFLOAT3& axis)
{
	SetRotationAxis(std::move(XMFLOAT3(axis)));
}

void GameEntity::SetRotationAxis(XMFLOAT3&& axis)
{
	m_xmf3RotationAxis = Vector3::Normalize(std::forward<XMFLOAT3>(axis));
}

void GameEntity::SetRotationSpeed(float speed)
{
	m_fRotationSpeed = speed;
}

void GameEntity::Update(float elapsed_time)
{
	if (m_fRotationSpeed != 0.0f)
	{
		Rotate(m_xmf3RotationAxis, m_fRotationSpeed * elapsed_time);
	}

	Move(Direction, Speed);

	if (0.0f != Friction)
	{
		float deceleration = Friction * elapsed_time;
		if (Speed < deceleration)
		{
			Speed = 0.0f;
		}
		else
		{
			Speed -= deceleration;
		}
	}

	if (0.0f != elapsed_time)
	{
		if (transformModified)
		{
			UpdateBoundingBox();
			transformModified = false;
		}
	}
}

void GameEntity::CreateRay(XMVECTOR& pick_pos, XMMATRIX& view, XMVECTOR& ray_pos, XMVECTOR& ray_dir)
{
	const auto& mat = Transform.GetWorldMatrix();
	const auto float4x4 = DirectX::XMLoadFloat4x4(&mat);
	XMMATRIX xmmtxToModel = XMMatrixInverse(NULL, float4x4 * view);

	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	const auto origin = DirectX::XMLoadFloat3(&xmf3CameraOrigin);
	ray_pos = XMVector3TransformCoord(origin, xmmtxToModel);
	ray_dir = XMVector3TransformCoord(pick_pos, xmmtxToModel);
	ray_dir = XMVector3Normalize(ray_dir - ray_pos);
}

int GameEntity::Raycast(XMVECTOR& pick_pos, XMMATRIX& view, float* max_distance)
{
	int nIntersected = 0;
	if (myMesh.IsAvailable())
	{
		XMVECTOR ray_pos, ray_dir;
		CreateRay(pick_pos, view, ray_pos, ray_dir);

		nIntersected = myMesh.GetMesh()->Raycast(ray_pos, ray_dir, max_distance);
	}

	return (nIntersected);
}

bool GameEntity::CheckCollideWith(const GameObject* other) const
{
	return myCollider.Intersects(other->myCollider);
}

void GameEntity::OnUpdateTransform()
{
	transformModified = true;
}

void GameEntity::OnCollisionEnter(GameEntity* other)
{}
