#pragma once
#include "Transformer.hpp"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void ReleaseUploadBuffers();

	void SetMesh(CMesh* pMesh);
	void SetOriginalCollider(const shared_ptr<BoundingOrientedBox>& box);
	void SetOriginalCollider(shared_ptr<BoundingOrientedBox>&& box);
	void Attach(GameObject* pChild);
	void BuildCollider();

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist);
	void Animate(float delta_time);
	void Animate(float delta_time, const XMFLOAT4X4& parent);
	virtual void Update(float delta_time);
	void UpdateTransform();
	void UpdateTransform(const XMFLOAT4X4& parent);
	void UpdateTransform(XMFLOAT4X4&& parent);
	void EnumerateTransforms();
	void EnumerateTransforms(const XMFLOAT4X4& parent);
	void EnumerateTransforms(XMFLOAT4X4&& parent);
	void UpdateCollider();
	virtual void PrepareRendering(P3DGrpCommandList cmdlist) const;
	virtual void Render(P3DGrpCommandList cmdlist, GameCamera* camera) const;
	virtual void ReleaseUniforms();

	bool CheckCollisionWith(GameObject* other) const;
	virtual void CollideWith(GameObject* other);

	void OnTransformUpdate();

	virtual constexpr COLLISION_TAGS GetTag() const noexcept;

	void SetMatrix(const XMFLOAT4X4& mat);
	void SetMatrix(XMFLOAT4X4&& mat);
	void SetScale(float x, float y, float z);
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& pos);
	void SetPosition(XMFLOAT3&& pos);
	void SetRotation(const XMFLOAT4X4& tfrm);
	void SetRotation(XMFLOAT4X4&& tfrm);

	void Translate(float x, float y, float z);
	void Translate(const XMFLOAT3& shift);
	void Translate(XMFLOAT3&& shift);
	void Move(const XMFLOAT3& dir, float distance);
	void Move(XMFLOAT3&& dir, float distance);
	void MoveStrafe(float distance);
	void MoveForward(float distance);
	void MoveUp(float distance);

	void Rotate(const XMFLOAT4X4& tfrm);
	void Rotate(XMFLOAT4X4&& tfrm);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(const XMFLOAT3& axis, float angle);
	void Rotate(const XMFLOAT4& quaternion);
	void Rotate(XMFLOAT4&& quaternion);

	void LookTo(const XMFLOAT3& look, const XMFLOAT3& up);
	void LookAt(const XMFLOAT3& look, const XMFLOAT3& up);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	const GameObject* FindFrame(const char* name) const;
	GameObject* FindFrame(const char* name);
	const GameObject* GetParent() const;
	GameObject* GetParent();
	UINT GetMeshType() const;

	void PrintFrameInfo() const;
	void PrintFrameInfo(const GameObject* parent) const;

	std::string myName;

	Transformer localTransform;
	Transformer worldTransform;
	XMFLOAT4X4& localMatrix;
	XMFLOAT4X4& worldMatrix;
	bool isTransformModified;

	CMesh* m_pMesh;
	GameObject* m_pParent = NULL;
	GameObject* myChild = NULL;
	GameObject* mySibling = NULL;

	shared_ptr<BoundingOrientedBox> staticCollider;
	unique_ptr<BoundingOrientedBox> myCollider;
};

enum class COLLISION_TAGS
{
	NONE = 0,
	ROCK,
	TREE,
	CAR,
	PLAYER
};

class CHellicopterObject : public GameObject
{
public:
	CHellicopterObject();
	virtual ~CHellicopterObject();

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
	virtual void Update(float delta_time) override;

protected:
	GameObject* m_pMainRotorFrame = NULL;
	GameObject* m_pTailRotorFrame = NULL;
};

class CApacheObject : public CHellicopterObject
{
public:
	CApacheObject();
	virtual ~CApacheObject();

public:
	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
	virtual void Update(float delta_time) override;
};

class CGunshipObject : public CHellicopterObject
{
public:
	CGunshipObject();
	virtual ~CGunshipObject();

public:
	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
};

class CSuperCobraObject : public CHellicopterObject
{
public:
	CSuperCobraObject();
	virtual ~CSuperCobraObject();

public:
	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
};

class CMi24Object : public CHellicopterObject
{
public:
	CMi24Object();
	virtual ~CMi24Object();

public:
	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
};
