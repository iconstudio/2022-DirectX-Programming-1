#pragma once
#include "stdafx.hpp"
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

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(const XMFLOAT3& xmf3Position);
	virtual void SetPosition(XMFLOAT3&& xmf3Position);
	virtual void SetRotationTransform(XMFLOAT4X4* pmxf4x4Transform);
	virtual void AddPosition(XMFLOAT3&& xmf3Position);

	XMFLOAT3&& GetPosition();
	XMFLOAT3&& GetLook();
	XMFLOAT3&& GetUp();
	XMFLOAT3&& GetRight();

	void SetMovingDirection(const XMFLOAT3& xmf3MovingDirection);
	void SetMovingDirection(XMFLOAT3&& xmf3MovingDirection);
	void SetMovingSpeed(float fSpeed);
	void SetMovingRange(float fRange);

	void Move(XMFLOAT3& vDirection, float fSpeed);
	void MoveForward(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveStrafe(float fDistance = 1.0f);

	void SetRotationAxis(const XMFLOAT3& xmf3RotationAxis);
	void SetRotationAxis(XMFLOAT3&& xmf3RotationAxis);
	void SetRotationSpeed(float fSpeed);

	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	virtual void Rotate(XMFLOAT3& xmf3Axis, float fAngle);

	void LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	virtual void Update(float fElapsedTime);
	virtual void Render(HDC surface, XMFLOAT4X4* world, CMesh* mesh);
	virtual void Render(HDC surface);

	void UpdateBoundingBox();
	void GenerateRayForPicking(XMVECTOR& pick_pos, XMMATRIX& view, XMVECTOR& ray_pos, XMVECTOR& xmvPickRayDirection);
	int PickObjectByRayIntersection(XMVECTOR& pick_pos, XMMATRIX& view, float* max_distance);

	bool m_bActive = true;
	GameScene& Scene;
	std::shared_ptr<GameCamera> Camera;

	std::shared_ptr<CMesh> m_pMesh;
	DWORD m_dwColor;
	HPEN m_Pen;

	XMFLOAT4X4 m_xmf4x4World;
	BoundingOrientedBox Collider;
	GameObject* m_pObjectCollided = NULL;

	XMFLOAT3					m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	float						m_fMovingSpeed = 0.0f;
	float						m_fMovingRange = 0.0f;

	XMFLOAT3					m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float						m_fRotationSpeed = 0.0f;
};
