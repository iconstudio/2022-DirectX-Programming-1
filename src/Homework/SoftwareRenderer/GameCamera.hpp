#pragma once
#include "stdafx.hpp"
#include "GameTransform.hpp"
#include "GameObject.hpp"

class GameViewport
{
public:
	GameViewport() {}
	virtual ~GameViewport() {}

	int m_nLeft = 0;
	int m_nTop = 0;
	int m_nWidth = 0;
	int m_nHeight = 0;

	void SetViewport(int nLeft, int nTop, int nWidth, int nHeight);
};

class GameCamera
{
public:
	GameCamera();
	virtual ~GameCamera();

	void SetFollower(GameObject* target);
	void SetFOVAngle(float fFOVAngle);

	void GenerateViewMatrix();
	void GeneratePerspectiveProjectionMatrix(float plane_near, float plane_far, float angle);
	void GenerateOrthographicProjectionMatrix(float plane_near, float plane_far, float width, float height);
	void SetViewport(int left, int top, int width, int height);

	void SetLookAt(const XMFLOAT3 xmf3LookAt, const XMFLOAT3 xmf3Up);
	void SetLookAt(const XMFLOAT3 vPosition, const XMFLOAT3 xmf3LookAt, const XMFLOAT3 xmf3Up);

	void Translate(const XMFLOAT3& shift);
	void Translate(XMFLOAT3&& shift);
	void Move(const XMFLOAT3& dir, float distance);
	void Move(XMFLOAT3&& dir, float distance);
	void Move(float x, float y, float z);
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

	void Update(const XMFLOAT3& offset, float fTimeElapsed = 0.016f);

	bool IsInFrustum(const BoundingBox& collider) const;
	bool IsInFrustum(const BoundingOrientedBox& collider) const;
	bool IsInFrustum(const BoundingSphere& collider) const;

	XMFLOAT4X4		projectionView = Matrix4x4::Identity();
	XMFLOAT4X4		m_xmf4x4PerspectiveProject = Matrix4x4::Identity();
	XMFLOAT4X4		projectionPerspective = Matrix4x4::Identity();

	XMFLOAT4X4		projectionOrthographic = Matrix4x4::Identity();
	XMFLOAT4X4		m_xmf4x4ViewOrthographicProject = Matrix4x4::Identity();

	GameTransform Transform;
	GameViewport m_Viewport;

private:
	BoundingFrustum	StaticCollider;
	BoundingFrustum	Collider;

	XMFLOAT4X4 m_xmf4x4InverseView = Matrix4x4::Identity();
	//XMFLOAT3 myPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//XMFLOAT3 myRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
	//XMFLOAT3 myUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//XMFLOAT3 myLook = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float m_fFOVAngle = 90.0f;
	float m_fProjectRectDistance = 1.0f;
	float m_fAspectRatio = float(FRAMEBUFFER_WIDTH) / float(FRAMEBUFFER_HEIGHT);

	GameObject* Follower;
};
