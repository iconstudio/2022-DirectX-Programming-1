#pragma once
#include "stdafx.hpp"
#include "GameTransform.hpp"
#include "Mesh.hpp"

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
	void SetMesh(std::shared_ptr<CMesh> pMesh);
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

	void Move(const XMFLOAT3& vDirection, float fSpeed);
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(const XMFLOAT3& axis, float angle);

	void LookTo(XMFLOAT3& to, XMFLOAT3& up);
	void LookAt(XMFLOAT3& from, XMFLOAT3& up);

	void SetDirection(const XMFLOAT3& xmf3MovingDirection);
	void SetDirection(XMFLOAT3&& xmf3MovingDirection);
	void SetSpeed(float fSpeed);

	void SetRotationAxis(const XMFLOAT3& xmf3RotationAxis);
	void SetRotationAxis(XMFLOAT3&& xmf3RotationAxis);
	void SetRotationSpeed(float fSpeed);

	virtual void Update(float fElapsedTime);
	virtual void Render(HDC surface, const XMFLOAT4X4& world, CMesh* mesh);
	virtual void Render(HDC surface);

	void UpdateBoundingBox();
	void GenerateRayForPicking(XMVECTOR& pick_pos, XMMATRIX& view, XMVECTOR& ray_pos, XMVECTOR& xmvPickRayDirection);
	int PickObjectByRayIntersection(XMVECTOR& pick_pos, XMMATRIX& view, float* max_distance);

	bool m_bActive = true;
	GameScene& Scene;
	std::shared_ptr<GameCamera> Camera;

	std::shared_ptr<CMesh> MeshPtr;
	DWORD m_dwColor;
	HPEN m_Pen;

	GameTransform Transform;
	BoundingOrientedBox Collider;

	XMFLOAT3 m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	float m_fMovingSpeed = 0.0f;

	XMFLOAT3 m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float m_fRotationSpeed = 0.0f;
};
