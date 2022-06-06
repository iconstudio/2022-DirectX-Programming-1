#pragma once
#include "Mesh.h"
#include "Material.hpp"
#include "Transformer.hpp"

enum class COLLISION_TAGS
{
	NONE = 0,
	ROCK,
	TREE,
	CAR,
	PLAYER
};

class GameObject
{
public:
	GameObject();
	GameObject(float x, float y, float z);
	GameObject(float posiion[3]);
	GameObject(const XMFLOAT3& position);
	GameObject(XMFLOAT3&& position);
	virtual ~GameObject();

	virtual constexpr COLLISION_TAGS GetTag() const noexcept;

	void SetMesh(CMesh* mesh);
	void SetMaterial(int index, CMaterial* material);
	void Attach(GameObject* child, bool bReferenceUpdate = false);

	virtual void SetWorldMatrix(const XMFLOAT4X4& tfrm);
	virtual void SetWorldMatrix(XMFLOAT4X4&& tfrm);
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(const XMFLOAT3& pos);
	virtual void SetPosition(XMFLOAT3&& pos);
	virtual void SetScale(float x, float y, float z);
	virtual void SetRotation(const XMFLOAT4X4& tfrm);
	virtual void SetRotation(XMFLOAT4X4&& tfrm);
	virtual void LookTo(XMFLOAT3& to, XMFLOAT3& up);
	virtual void LookAt(XMFLOAT3& from, XMFLOAT3& up);

	virtual void AddPosition(const XMFLOAT3& vector);
	virtual void AddPosition(XMFLOAT3&& vector);
	virtual void Move(const XMFLOAT3& direction, float distance);
	virtual void Move(XMFLOAT3&& direction, float distance);
	virtual void MoveStrafe(float distance);
	virtual void MoveUp(float distance);
	virtual void MoveForward(float distance);
	virtual void Rotate(float pitch, float yaw, float roll);
	virtual void Rotate(const XMFLOAT3& axis, float angle);
	virtual void Rotate(const XMFLOAT4& quaternion);

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetLook() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetRight() const;

	void BeginUpdateTransform();
	void UpdateTransform(const Transformer& parent);
	void UpdateCollider(const XMFLOAT4X4& mat);
	virtual void OnUpdateTransform();
	virtual void UpdateBoundingBox();

	virtual void ReleaseUploadBuffers();

	virtual void SetOriginalCollider(const shared_ptr<BoundingOrientedBox>& box);
	virtual void BuildCollider();
	virtual void Animate(float time_elapsed, XMFLOAT4X4* parent = nullptr);

	virtual void OnPrepareRender() {}
	virtual void Render(P3DGrpCommandList cmd_list, GameCamera* pCamera = NULL);

	virtual void InitializeUniforms(P3DDevice device, P3DGrpCommandList cmd_list);
	virtual void UpdateUniforms(P3DGrpCommandList cmd_list);
	virtual void UpdateUniforms(P3DGrpCommandList cmd_list, XMFLOAT4X4* pxmf4x4World);
	virtual void UpdateUniforms(P3DGrpCommandList cmd_list, CMaterial* pMaterial);
	virtual void ReleaseUniforms();

	virtual bool CheckCollisionWith(GameObject* other) const;
	virtual void CollideWith(GameObject* other);

	GameObject* GetParent() { return(m_pParent); }
	GameObject* FindFrame(const char* pstrFrameName);

	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0); }

	char m_pstrFrameName[256];

	CMesh* m_pMesh = nullptr;

	int m_nMaterials = 0;
	CMaterial** m_ppMaterials = NULL;

	Transformer localTransform, worldTransform;

	GameObject* m_pParent = NULL;
	GameObject* myChild = NULL;
	GameObject* mySibling = NULL;

	shared_ptr<BoundingOrientedBox> staticCollider;
	unique_ptr<BoundingOrientedBox> myCollider;

	static void PrintFrameInfo(GameObject* pGameObject, GameObject* pParent);
};

class CHellicopterObject : public GameObject
{
public:
	CHellicopterObject();
	virtual ~CHellicopterObject();

protected:
	GameObject* m_pMainRotorFrame = NULL;
	GameObject* m_pTailRotorFrame = NULL;

public:
	virtual void Awake();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

class CApacheObject : public CHellicopterObject
{
public:
	CApacheObject();
	virtual ~CApacheObject();

public:
	virtual void Awake();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

class CGunshipObject : public CHellicopterObject
{
public:
	CGunshipObject();
	virtual ~CGunshipObject();

public:
	virtual void Awake();
};

class CSuperCobraObject : public CHellicopterObject
{
public:
	CSuperCobraObject();
	virtual ~CSuperCobraObject();

public:
	virtual void Awake();
};

class CMi24Object : public CHellicopterObject
{
public:
	CMi24Object();
	virtual ~CMi24Object();

public:
	virtual void Awake();
};
