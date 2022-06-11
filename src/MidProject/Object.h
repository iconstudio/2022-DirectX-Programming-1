#pragma once
#include "Transformer.hpp"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void ReleaseUploadBuffers();

	void SetMesh(CMaterialMesh* pMesh);
	void SetOriginalCollider(const shared_ptr<BoundingOrientedBox>& box);
	void SetOriginalCollider(shared_ptr<BoundingOrientedBox>&& box);
	void Attach(GameObject* pChild);
	void BuildCollider();

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist);
	void Animate(float time_elapsed, XMFLOAT4X4* parent = nullptr);
	void Animate(float time_elapsed, XMFLOAT4X4* parent = nullptr);
	virtual void Update(float time_elapsed);
	virtual void UpdateTransform(const XMFLOAT4X4* parent);
	void EnumerateTransforms(const XMFLOAT4X4* parent);
	void UpdateCollider();
	virtual void PrepareRendering(P3DGrpCommandList cmdlist) const;
	virtual void Render(P3DGrpCommandList cmdlist, GameCamera* camera = NULL) const;
	virtual void ReleaseUniforms();

	bool CheckCollisionWith(GameObject* other) const;
	virtual void CollideWith(GameObject* other);

	void OnTransformUpdate();

	virtual constexpr COLLISION_TAGS GetTag() const noexcept;

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 position);
	void SetScale(float x, float y, float z);
	void MoveStrafe(float dist = 1.0f);
	void MoveUp(float dist = 1.0f);
	void MoveForward(float dist = 1.0f);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4* pxmf4Quaternion);

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

	Transformer myTransform;
	XMFLOAT4X4 localTransform;
	XMFLOAT4X4 worldTransform;
	bool isTransformModified;

	CMaterialMesh* m_pMesh = nullptr;
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

class CRotatingObject : public GameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

private:
	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	virtual void Update(float delta_time) override;
};

class CRevolvingObject : public GameObject
{
public:
	CRevolvingObject();
	virtual ~CRevolvingObject();

private:
	XMFLOAT3 m_xmf3RevolutionAxis;
	float m_fRevolutionSpeed;

public:
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	void SetRevolutionAxis(XMFLOAT3 xmf3RevolutionAxis) { m_xmf3RevolutionAxis = xmf3RevolutionAxis; }

	virtual void Update(float delta_time) override;
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
