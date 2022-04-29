#pragma once
#include "stdafx.hpp"
#include "GameTransform.hpp"

class GameObject
{
public:
	GameObject(const GameObject&) = default;
	GameObject(GameObject&&) = default;
	GameObject(GameScene& scene);
	GameObject(GameScene& scene, float x, float y, float z);
	GameObject(GameScene& scene, const XMFLOAT3& position);
	GameObject(GameScene& scene, XMFLOAT3&& position);
	virtual ~GameObject();

	void SetActive(bool bActive);
	void SetMesh(std::shared_ptr<CMesh>& pMesh);
	void SetColor(DWORD dwColor);
	void SetCamera(std::shared_ptr<GameCamera>& cam);

	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& xmf3Position);
	void SetPosition(XMFLOAT3&& xmf3Position);
	void SetRotation(const XMFLOAT4X4& pmxf4x4Transform);
	void AddPosition(XMFLOAT3&& xmf3Position);

	XMFLOAT3&& GetPosition();
	XMFLOAT3&& GetLook();
	XMFLOAT3&& GetUp();
	XMFLOAT3&& GetRight();

	virtual void Move(const XMFLOAT3& vDirection, float distance);
	virtual void MoveStrafe(float distance);
	virtual void MoveUp(float distance);
	virtual void MoveForward(float distance);
	virtual void Rotate(float fPitch, float fYaw, float fRoll);
	virtual void Rotate(const XMFLOAT3& axis, float angle);

	void LookTo(XMFLOAT3& to, XMFLOAT3& up);
	void LookAt(XMFLOAT3& from, XMFLOAT3& up);

	void SetVelocity(const XMFLOAT3& vector);
	void SetVelocity(XMFLOAT3&& vector);
	void SetDirection(const XMFLOAT3& direction);
	void SetDirection(XMFLOAT3&& direction);
	void SetSpeed(const float value);
	void AddSpeed(const float value, const float max);
	void AddSpeed(const float value);

	float GetSpeed() const;

	void SetRotationAxis(const XMFLOAT3& xmf3RotationAxis);
	void SetRotationAxis(XMFLOAT3&& xmf3RotationAxis);
	void SetRotationSpeed(float fSpeed);

	virtual void Update(float elapsed_time);
	virtual void PrepareRendering(GameScene& scene);
	virtual void Render(HDC surface) const;

	virtual bool CheckCameraBounds() const;
	virtual void UpdateBoundingBox();

	void GenerateRayForPicking(XMVECTOR& pick_pos, XMMATRIX& view, XMVECTOR& ray_pos, XMVECTOR& ray_dir);
	int PickObjectByRayIntersection(XMVECTOR& pick_pos, XMMATRIX& view, float* max_distance);

	GameScene& Scene;
	std::shared_ptr<GameCamera> Camera;

	GameMesh myMesh;
	DWORD myColour;
	HPEN myPen;

	bool isActivated = true;

	GameTransform Transform;
	BoundingOrientedBox Collider;

	XMFLOAT3 Direction;
	float Speed;
	float Friction;

	XMFLOAT3 m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float m_fRotationSpeed = 0.0f;
};
