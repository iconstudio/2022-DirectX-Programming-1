#pragma once
#include "Object.h"

class GameEntity : public GameObject
{
public:
	GameEntity();
	virtual ~GameEntity();

	virtual void Update(float delta_time) override;

	void SetVelocity(const XMFLOAT3& vector);
	void SetVelocity(XMFLOAT3&& vector);
	void SetDirection(const XMFLOAT3& direction);
	void SetDirection(XMFLOAT3&& direction);
	void SetSpeed(const float value);
	void SetRotationAxis(const XMFLOAT3& axis);
	void SetRotationAxis(XMFLOAT3&& axis);
	void SetRotationSpeed(float speed);
	void SetFriction(float friction);
	void SetGravity(const XMFLOAT3& gravity);
	void SetMaxXZVelocity(float value);
	void SetMaxYVelocity(float value);

	void AddSpeed(const float value, const float max);
	void AddSpeed(const float value);
	void Accelerate(const XMFLOAT3& accel);
	void Accelerate(const XMFLOAT3& dir, float speed);

	float GetSpeed() const;
	const XMFLOAT3& GetVelocity() const;
	XMFLOAT3& GetVelocity();
	const XMFLOAT3& GetGravity() const;
	XMFLOAT3& GetGravity();

	XMFLOAT3 myVelocity;
	XMFLOAT3 myGravity;

	float myFriction;
	float m_fMaxVelocityXZ;
	float m_fMaxVelocityY;

	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
};
