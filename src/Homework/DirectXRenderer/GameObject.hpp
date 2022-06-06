#pragma once
#include "GameTransform.hpp"
#include "GameMesh.hpp"

class GameObject
{
public:
	GameObject();
	GameObject(float x, float y, float z);
	GameObject(float list[3]);
	GameObject(const XMFLOAT3& position);
	GameObject(XMFLOAT3&& position);
	virtual ~GameObject();

	virtual constexpr GOBJECT_TAGS GetTag() const = 0;

	void SetActive(bool flag);
	void SetMesh(const shared_ptr<CMesh>& mesh);
	void SetColor(COLORREF color);
	void Activate();
	void Deactivate();

	bool IsActivated() const;
	virtual constexpr bool IsStatic() const = 0;

	virtual void SetWorldMatrix(const XMFLOAT4X4& tfrm);
	virtual void SetWorldMatrix(XMFLOAT4X4&& tfrm);
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(const XMFLOAT3& pos);
	virtual void SetPosition(XMFLOAT3&& pos);
	virtual void SetRotation(const XMFLOAT4X4& tfrm);
	virtual void SetRotation(XMFLOAT4X4&& tfrm);
	virtual void LookTo(XMFLOAT3& to, XMFLOAT3& up);
	virtual void LookAt(XMFLOAT3& from, XMFLOAT3& up);

	virtual void AddPosition(XMFLOAT3&& vector);
	virtual void Move(const XMFLOAT3& vDirection, float distance);
	virtual void MoveStrafe(float distance);
	virtual void MoveUp(float distance);
	virtual void MoveForward(float distance);
	virtual void Rotate(float pitch, float yaw, float roll);
	virtual void Rotate(const XMFLOAT3& axis, float angle);

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetLook() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetRight() const;

	virtual void PrepareRendering(GameScene& scene) const;
	virtual void Render(HDC surface) const;

	virtual void UpdateBoundingBox();
	
	bool isActivated;

	GameMesh myMesh;
	GameTransform Transform;
	BoundingOrientedBox myCollider;
};
