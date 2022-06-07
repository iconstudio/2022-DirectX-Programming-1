#pragma once
#include "GameObject.hpp"

class GameEntity : public GameObject
{
public:
	GameEntity();
	GameEntity(float x, float y, float z);
	GameEntity(float list[3]);
	GameEntity(const XMFLOAT3& position);
	GameEntity(XMFLOAT3&& position);
	virtual ~GameEntity();

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Update(float delta_time) override;

	void SetVelocity(const XMFLOAT3& vector);
	void SetVelocity(XMFLOAT3&& vector);
	void SetDirection(const XMFLOAT3& direction);
	void SetDirection(XMFLOAT3&& direction);
	void SetSpeed(const float value);
	void SetFriction(const float value);
	void SetRotationAxis(const XMFLOAT3& axis);
	void SetRotationAxis(XMFLOAT3&& axis);
	void SetRotationSpeed(float speed);
	float GetSpeed() const;

	void AddSpeed(const float value, const float max);
	void AddSpeed(const float value);

	virtual void UpdateTransform();
	virtual void UpdateTransform(const XMFLOAT4X4& parent);
	virtual void UpdateBoundingBox();

	bool CheckCollideWith(const GameEntity* other) const;
	virtual void OnCollisionEnter(GameEntity* other);

	bool isKilled = false;
	shared_ptr<BoundingOrientedBox> staticCollider;
	BoundingOrientedBox myCollider;

	XMFLOAT3 Direction;
	float Speed;
	float Friction;

	XMFLOAT3 m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float m_fRotationSpeed = 0.0f;
};
