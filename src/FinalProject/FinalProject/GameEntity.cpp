#include "pch.hpp"
#include "GameEntity.hpp"
#include "Arithmetics.hpp"

GameEntity::GameEntity()
	: GameObject()
	, Direction(XMFLOAT3(0.0f, 0.0f, 0.0f)), Speed(0.0f), Friction(0.0f)
	, staticCollider(nullptr), myCollider()
{}

GameEntity::GameEntity(float x, float y, float z)
	: GameEntity()
{
	SetPosition(x, y, z, true);
}

GameEntity::GameEntity(float list[3])
	: GameEntity()
{
	SetPosition(list[0], list[1], list[2], true);
}

GameEntity::GameEntity(const XMFLOAT3& position)
	: GameEntity()
{
	SetPosition(position, true);
}

GameEntity::GameEntity(XMFLOAT3&& position)
	: GameEntity()
{
	SetPosition(position, true);
}

GameEntity::~GameEntity()
{}

void GameEntity::Awake()
{
	GameObject::Awake();
}

void GameEntity::Start()
{
	GameObject::Start();
}

void GameEntity::Reset()
{
	GameObject::Reset();
}

void GameEntity::Update(float delta_time)
{
	if (m_fRotationSpeed != 0.0f)
	{
		Rotate(m_xmf3RotationAxis, m_fRotationSpeed * delta_time, false);
	}

	Move(Direction, Speed, true);

	if (0.0f != Friction)
	{
		float deceleration = Friction * delta_time;
		if (Speed < deceleration)
		{
			Speed = 0.0f;
		}
		else
		{
			Speed -= deceleration;
		}
	}

	GameObject::Update(delta_time);
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

void GameEntity::SetFriction(const float value)
{
	Friction = value;
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

void GameEntity::UpdateTransform()
{
	GameObject::UpdateTransform();
	UpdateBoundingBox();
}

void GameEntity::UpdateTransform(const XMFLOAT4X4& parent)
{
	GameObject::UpdateTransform(parent);
	UpdateBoundingBox();
}

void GameEntity::UpdateBoundingBox()
{
	const auto& my_mat = worldTransform.GetWorldMatrix();
	const auto float4x4 = DirectX::XMLoadFloat4x4(&my_mat);

	const auto& original_collider = staticCollider;
	if (original_collider)
	{
		original_collider->Transform(myCollider, float4x4);

		auto& angle = myCollider.Orientation;

		const auto orientation = DirectX::XMLoadFloat4(&angle);
		const auto quaternion = DirectX::XMQuaternionNormalize(orientation);
		DirectX::XMStoreFloat4(&angle, quaternion);
	}
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

bool GameEntity::CheckCollideWith(const GameEntity* other) const
{
	const auto& other_collision = other->myCollider;
	return other_collision.Intersects(myCollider);
}

void GameEntity::OnCollisionEnter(GameEntity* other)
{

}
