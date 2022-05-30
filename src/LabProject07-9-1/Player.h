#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#include "Object.h"
#include "Camera.h"

class CPlayer : public GameObject
{
public:
	virtual constexpr COLLISION_TAGS GetTag() const noexcept override;

	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	XMFLOAT3					m_xmf3Velocity;
	XMFLOAT3     				m_xmf3Gravity;
	float           			m_fMaxVelocityXZ;
	float           			m_fMaxVelocityY;
	float           			m_fFriction;

	LPVOID						m_pPlayerUpdatedContext;
	LPVOID						m_pCameraUpdatedContext;

	GameCamera* myCamera = NULL;

public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	virtual void SetVelocity(const XMFLOAT3& xmf3Velocity)
	{
		m_xmf3Velocity = xmf3Velocity;
	}
	virtual void SetPosition(const XMFLOAT3& xmf3Position)
	{
		Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x
			, xmf3Position.y - m_xmf3Position.y
			, xmf3Position.z - m_xmf3Position.z)
			, false);
		GameObject::SetPosition(xmf3Position);
	}

	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	virtual GameCamera* GetCamera() { return(myCamera); }
	virtual void SetCamera(GameCamera* pCamera) { myCamera = pCamera; }

	virtual void Move(DWORD nDirection, float fDistance, bool bVelocity = false);
	virtual void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	virtual void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	virtual void MoveForward(float fDistance = 1.0f);
	virtual void Rotate(float x, float y, float z);

	virtual void Update(float fTimeElapsed);

	virtual void CollideWith(GameObject* other) override;

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) {}
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) {}
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void InitializeUniforms(P3DDevice device, P3DGrpCommandList cmd_list);
	virtual void ReleaseUniforms();
	virtual void UpdateUniforms(P3DGrpCommandList cmd_list);

	GameCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual GameCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }
	virtual void OnPrepareRender();
	virtual void Render(P3DGrpCommandList cmd_list, GameCamera* pCamera = NULL);

	BoundingOrientedBox myCollider;
};

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer(P3DDevice device, P3DGrpCommandList cmd_list, P3DSignature signature);
	virtual ~CAirplanePlayer();

	virtual void Awake();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual GameCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void OnPrepareRender();

private:
	GameObject* m_pMainRotorFrame = NULL;
	GameObject* m_pTailRotorFrame = NULL;

};


