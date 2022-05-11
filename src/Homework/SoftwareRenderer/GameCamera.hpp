#pragma once
#include "stdafx.hpp"
#include "GameViewport.hpp"
#include "GameTransform.hpp"

class GameCamera
{
public:
	GameCamera();
	virtual ~GameCamera();

	void SetViewport(int left, int top, int width, int height);
	void SetFOVAngle(float angle);
	void GeneratePerspectiveProjectionMatrix(float znear, float zfar, float fov);
	void GenerateOrthographicProjectionMatrix(float znear, float zfar, float width, float height);

	void SetLocalPosition(const XMFLOAT3& pos);
	void SetLocalPosition(XMFLOAT3&& pos);
	void SetRotation(const XMFLOAT4X4& tfrm);
	void SetRotation(XMFLOAT4X4&& tfrm);

	void Translate(float x, float y, float z);
	void Translate(const XMFLOAT3& shift);
	void Translate(XMFLOAT3&& shift);
	void Move(const XMFLOAT3& dir, float distance);
	void Move(XMFLOAT3&& dir, float distance);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(const XMFLOAT3& axis, float angle);

	void LookAt(const XMFLOAT3 look, const XMFLOAT3 up);
	void LookAt(const XMFLOAT3 pos, const XMFLOAT3 look, const XMFLOAT3 up);

	void GenerateViewMatrix();
	void Update(const XMFLOAT3& look_at, const GameTransform& follower, const XMFLOAT3& up, float elapsed_time);

	bool IsInFrustum(const BoundingBox& collider) const;
	bool IsInFrustum(const BoundingOrientedBox& collider) const;
	bool IsInFrustum(const BoundingSphere& collider) const;

	GameTransform Transform;
	XMFLOAT3 localPosition;
	float myRoll, myYaw, myPitch;

	// 뷰 포트
	GameViewport m_Viewport;

	// 카메라를 위한 카메라 변환 행렬
	XMFLOAT4X4 projectionView = Matrix4x4::Identity();
	XMFLOAT4X4 m_xmf4x4InverseView = Matrix4x4::Identity();

	XMFLOAT4X4 m_xmf4x4PerspectiveProject = Matrix4x4::Identity();
	XMFLOAT4X4 projectionPerspective = Matrix4x4::Identity();

	XMFLOAT4X4 m_xmf4x4ViewOrthographicProject = Matrix4x4::Identity();
	XMFLOAT4X4 projectionOrthographic = Matrix4x4::Identity();

	BoundingFrustum	StaticCollider;
	BoundingFrustum	Collider;

	float m_fFOVAngle = 60.0f;
	float m_fProjectRectDistance = 1.0f;
	float m_fAspectRatio = float(FRAMEBUFFER_WIDTH) / float(FRAMEBUFFER_HEIGHT);
};
