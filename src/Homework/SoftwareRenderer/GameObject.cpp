#include "stdafx.hpp"
#include "GameObject.hpp"

GameObject::GameObject()
	: GameEntity()
	, transformModified(false)
	, Direction(XMFLOAT3(0.0f, 0.0f, 0.0f)), Speed(0.0f), Friction(0.0f)
{}

GameObject::GameObject(float x, float y, float z)
	: GameObject()
{
	SetPosition(x, y, z);
}

GameObject::GameObject(float list[3])
	: GameObject()
{
	SetPosition(list[0], list[1], list[2]);
}

GameObject::GameObject(const XMFLOAT3& position)
	: GameObject()
{
	SetPosition(position);
}

GameObject::GameObject(XMFLOAT3&& position)
	: GameObject()
{
	SetPosition(position);
}

GameObject::~GameObject()
{}

void GameObject::SetWorldMatrix(XMFLOAT4X4&& tfrm)
{
	GameEntity::SetWorldMatrix(std::forward<XMFLOAT4X4>(tfrm));
	OnUpdateTransform();
}

void GameObject::SetWorldMatrix(const XMFLOAT4X4& tfrm)
{
	GameEntity::SetWorldMatrix(tfrm);
	OnUpdateTransform();
}

void GameObject::SetPosition(float x, float y, float z)
{
	GameEntity::SetPosition(x, y, z);
	OnUpdateTransform();
}

void GameObject::SetPosition(const XMFLOAT3& pos)
{
	SetPosition(std::move(XMFLOAT3(pos)));
}

void GameObject::SetPosition(XMFLOAT3&& pos)
{
	GameEntity::SetPosition(std::forward<XMFLOAT3>(pos));
	OnUpdateTransform();
}

void GameObject::SetRotation(const XMFLOAT4X4& tfrm)
{
	GameEntity::SetRotation(tfrm);
	OnUpdateTransform();
}

void GameObject::SetRotation(XMFLOAT4X4&& tfrm)
{
	GameEntity::SetRotation(std::forward<XMFLOAT4X4>(tfrm));
	OnUpdateTransform();
}

void GameObject::AddPosition(XMFLOAT3&& vector)
{
	GameEntity::AddPosition(std::forward<XMFLOAT3>(vector));
	OnUpdateTransform();
}

constexpr bool GameObject::IsStatic() const
{
	return true;
}

void GameObject::Move(const XMFLOAT3& dir, float distance)
{
	GameEntity::Move(dir, distance);
	OnUpdateTransform();
}

void GameObject::MoveStrafe(float distance)
{
	GameEntity::MoveStrafe(distance);
	OnUpdateTransform();
}

void GameObject::MoveUp(float distance)
{
	GameEntity::MoveUp(distance);
	OnUpdateTransform();
}

void GameObject::MoveForward(float distance)
{
	GameEntity::MoveForward(distance);
	OnUpdateTransform();
}

void GameObject::Rotate(float pitch, float yaw, float roll)
{
	GameEntity::Rotate(pitch, yaw, roll);
	OnUpdateTransform();
}

void GameObject::Rotate(const XMFLOAT3& axis, float angle)
{
	GameEntity::Rotate(axis, angle);
	OnUpdateTransform();
}

void GameObject::LookTo(XMFLOAT3& to, XMFLOAT3& up)
{
	GameEntity::LookTo(to, up);
	OnUpdateTransform();
}

void GameObject::LookAt(XMFLOAT3& from, XMFLOAT3& up)
{
	GameEntity::LookAt(from, up);
	OnUpdateTransform();
}

void GameObject::SetVelocity(const XMFLOAT3& vector)
{
	SetVelocity(std::move(XMFLOAT3(vector)));
}

void GameObject::SetVelocity(XMFLOAT3&& vector)
{
	const XMFLOAT3 vel = std::forward<XMFLOAT3>(vector);
	Direction = Vector3::Normalize(vel);
	Speed = Vector3::Length(vel);
}

void GameObject::SetDirection(const XMFLOAT3& direction)
{
	SetDirection(std::move(XMFLOAT3(direction)));
}

void GameObject::SetDirection(XMFLOAT3&& direction)
{
	Direction = Vector3::Normalize(std::forward<XMFLOAT3>(direction));
}

void GameObject::SetSpeed(const float value)
{
	Speed = value;
}

void GameObject::AddSpeed(const float value, const float max)
{
	auto speed = GetSpeed() + value;
	if (max < speed) speed = max;

	SetSpeed(speed);
}

void GameObject::AddSpeed(const float value)
{
	SetSpeed(GetSpeed() + value);
}

float GameObject::GetSpeed() const
{
	return Speed;
}

void GameObject::SetRotationAxis(const XMFLOAT3& axis)
{
	SetRotationAxis(std::move(XMFLOAT3(axis)));
}

void GameObject::SetRotationAxis(XMFLOAT3&& axis)
{
	m_xmf3RotationAxis = Vector3::Normalize(std::forward<XMFLOAT3>(axis));
}

void GameObject::SetRotationSpeed(float speed)
{
	m_fRotationSpeed = speed;
}

void GameObject::Update(float elapsed_time)
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

void GameObject::CreateRay(XMVECTOR& pick_pos, XMMATRIX& view, XMVECTOR& ray_pos, XMVECTOR& ray_dir)
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

int GameObject::Raycast(XMVECTOR& pick_pos, XMMATRIX& view, float* max_distance)
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

bool GameObject::CheckCollideWith(const GameEntity* other) const
{
	return Collider.Intersects(other->Collider);
}

void GameObject::OnUpdateTransform()
{
	transformModified = true;
}

void GameObject::OnCollisionEnter(GameObject* other)
{}
