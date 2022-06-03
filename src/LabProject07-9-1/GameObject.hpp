#pragma once
#include "Mesh.h"
#include "Material.hpp"
#include "Camera.h"

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
	virtual ~GameObject();

	void SetMesh(CMesh* pMesh);
	void SetShader(CShader* pShader);
	void SetShader(int nMaterial, CShader* pShader);
	void SetMaterial(int nMaterial, CMaterial* pMaterial);
	void Attach(GameObject* pChild, bool bReferenceUpdate = false);

	virtual constexpr COLLISION_TAGS GetTag() const noexcept;

	void SetOriginalCollider(const shared_ptr<BoundingOrientedBox>& box);
	virtual void BuildCollider();
	void UpdateCollider(const XMFLOAT4X4* mat);

	virtual void BuildMaterials(P3DDevice device, P3DGrpCommandList cmd_list);
	virtual void ReleaseUploadBuffers();

	// °»½Å
	virtual void Animate(float time_elapsed, XMFLOAT4X4* parent = nullptr);

	virtual void OnPrepareRender() {}
	virtual void Render(P3DGrpCommandList cmd_list, GameCamera* pCamera = NULL);

	virtual void InitializeUniforms(P3DDevice device, P3DGrpCommandList cmd_list);
	virtual void UpdateUniforms(P3DGrpCommandList cmd_list);
	virtual void UpdateUniforms(P3DGrpCommandList cmd_list, XMFLOAT4X4* pxmf4x4World);
	virtual void UpdateUniforms(P3DGrpCommandList cmd_list, CMaterial* pMaterial);
	virtual void ReleaseUniforms();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4* pxmf4Quaternion);

	virtual bool CheckCollisionWith(GameObject* other) const;
	virtual void CollideWith(GameObject* other);

	GameObject* GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	GameObject* FindFrame(const char* pstrFrameName);

	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0); }

	char m_pstrFrameName[256];

	//shared_ptr<CMesh> m_pMesh;
	CMesh* m_pMesh = nullptr;

	int m_nMaterials = 0;
	CMaterial** m_ppMaterials = NULL;

	XMFLOAT4X4 localTransform;
	XMFLOAT4X4 worldTransform;

	GameObject* m_pParent = NULL;
	GameObject* myChild = NULL;
	GameObject* mySibling = NULL;

	shared_ptr<BoundingOrientedBox> staticCollider;
	unique_ptr<BoundingOrientedBox> myCollider;

	static void PrintFrameInfo(GameObject* pGameObject, GameObject* pParent);
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

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void Render(P3DGrpCommandList cmd_list, GameCamera* pCamera = NULL);
};

class CRevolvingObject : public GameObject
{
public:
	CRevolvingObject();
	virtual ~CRevolvingObject();

private:
	XMFLOAT3					m_xmf3RevolutionAxis;
	float						m_fRevolutionSpeed;

public:
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	void SetRevolutionAxis(XMFLOAT3 xmf3RevolutionAxis) { m_xmf3RevolutionAxis = xmf3RevolutionAxis; }

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
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
