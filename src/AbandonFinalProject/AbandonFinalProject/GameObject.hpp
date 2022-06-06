#pragma once
#include "PlayableObjet.hpp"
#include "Transformer.hpp"

class GameObject : public PlayableObjet
{
public:
	GameObject();
	GameObject(float x, float y, float z);
	GameObject(float list[3]);
	GameObject(const XMFLOAT3& position);
	GameObject(XMFLOAT3&& position);
	virtual ~GameObject();

	void SetActive(bool flag);
	void Activate();
	void Deactivate();

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Update(float delta_time) override;
	virtual void PrepareRendering(P3DGrpCommandList cmdlist) const;
	virtual void Render(P3DGrpCommandList cmdlist) const;

	virtual void SetWorldMatrix(const XMFLOAT4X4& tfrm, bool begin = true);
	virtual void SetWorldMatrix(XMFLOAT4X4&& tfrm, bool begin = true);
	virtual void SetPosition(float x, float y, float z, bool begin = true);
	virtual void SetPosition(const XMFLOAT3& pos, bool begin = true);
	virtual void SetPosition(XMFLOAT3&& pos, bool begin = true);
	virtual void SetRotation(const XMFLOAT4X4& tfrm, bool begin = true);
	virtual void SetRotation(XMFLOAT4X4&& tfrm, bool begin = true);
	virtual void LookTo(XMFLOAT3& to, XMFLOAT3& up, bool begin = true);
	virtual void LookAt(XMFLOAT3& from, XMFLOAT3& up, bool begin = true);

	virtual void AddPosition(XMFLOAT3&& vector, bool begin = true);
	virtual void Move(const XMFLOAT3& direction, float distance, bool begin = true);
	virtual void MoveStrafe(float distance, bool begin = true);
	virtual void MoveUp(float distance, bool begin = true);
	virtual void MoveForward(float distance, bool begin = true);
	virtual void Rotate(float pitch, float yaw, float roll, bool begin = true);
	virtual void Rotate(const XMFLOAT3& axis, float angle, bool begin = true);

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetLook() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetRight() const;
	bool IsActivated() const;

	void SetMesh(Mesh* handle);
	void Attach(GameObject* child);
	GameObject* FindFrame(const char* frame_name);

	void OnUpdateTransform();
	virtual void UpdateTransform();
	virtual void UpdateTransform(const XMFLOAT4X4& parent);

	std::string myName;

	Transformer localTransform, worldTransform;
	GameObject *mySibling, *myChild, *myParent;
	Mesh* m_pMesh;

	bool isActivated;
	bool transformModified;
};
