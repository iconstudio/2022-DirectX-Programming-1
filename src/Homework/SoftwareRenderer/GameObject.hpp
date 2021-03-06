#pragma once
#include "stdafx.hpp"
#include "GameEntity.hpp"

class GameObject : public GameEntity
{
public:
	GameObject();
	GameObject(float x, float y, float z);
	GameObject(float list[3]);
	GameObject(const XMFLOAT3& position);
	GameObject(XMFLOAT3&& position);
	virtual ~GameObject();

	virtual constexpr GOBJECT_TAGS GetTag() const { return GOBJECT_TAGS::NORMAL; }

	constexpr bool IsStatic() const override;

	virtual void SetWorldMatrix(const XMFLOAT4X4& tfrm) override;
	virtual void SetWorldMatrix(XMFLOAT4X4&& tfrm) override;
	virtual void SetPosition(float x, float y, float z) override;
	virtual void SetPosition(const XMFLOAT3& pos) override;
	virtual void SetPosition(XMFLOAT3&& pos) override;
	virtual void SetRotation(const XMFLOAT4X4& tfrm) override;
	virtual void SetRotation(XMFLOAT4X4&& tfrm) override;
	virtual void LookTo(XMFLOAT3& to, XMFLOAT3& up);
	virtual void LookAt(XMFLOAT3& from, XMFLOAT3& up);

	virtual void AddPosition(XMFLOAT3&& vector) override;
	virtual void Move(const XMFLOAT3& direction, float distance) override;
	virtual void MoveStrafe(float distance) override;
	virtual void MoveUp(float distance) override;
	virtual void MoveForward(float distance) override;
	virtual void Rotate(float pitch, float yaw, float roll) override;
	virtual void Rotate(const XMFLOAT3& axis, float angle) override;

	void SetVelocity(const XMFLOAT3& vector);
	void SetVelocity(XMFLOAT3&& vector);
	void SetDirection(const XMFLOAT3& direction);
	void SetDirection(XMFLOAT3&& direction);
	void SetSpeed(const float value);
	void SetRotationAxis(const XMFLOAT3& axis);
	void SetRotationAxis(XMFLOAT3&& axis);
	void SetRotationSpeed(float speed);
	float GetSpeed() const;

	void AddSpeed(const float value, const float max);
	void AddSpeed(const float value);

	virtual void Update(float elapsed_time);

	void CreateRay(XMVECTOR& pick_pos, XMMATRIX& view, XMVECTOR& ray_pos, XMVECTOR& ray_dir);
	int Raycast(XMVECTOR& pick_pos, XMMATRIX& view, float* max_distance);
	bool CheckCollideWith(const GameEntity* other) const;

	void OnUpdateTransform();

	virtual void OnCollisionEnter(GameObject* other);

	bool isKilled = false;
	bool transformModified;

	XMFLOAT3 Direction;
	float Speed;
	float Friction;

	XMFLOAT3 m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float m_fRotationSpeed = 0.0f;
};
