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
	CPlayer();
	virtual ~CPlayer();

	void SetCamera(GameCamera* camera);
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

	virtual void Move(DWORD nDirection, float fDistance, bool bVelocity = false);
	virtual void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	virtual void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	virtual void MoveForward(float fDistance = 1.0f);
	virtual void Rotate(float x, float y, float z);

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
	virtual void Update(float fTimeElapsed) override;
	virtual void PrepareRendering(P3DGrpCommandList cmdlist) const override;
	virtual void Render(P3DGrpCommandList cmdlist, GameCamera* camera = NULL) const override;
	virtual void ReleaseUniforms() override;

	virtual void CollideWith(GameObject* other) override;

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) {}
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }
	virtual void OnCameraUpdateCallback(float fTimeElapsed) {}
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }
	GameCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual GameCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }

	virtual constexpr COLLISION_TAGS GetTag() const noexcept override;
	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }
	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	const GameCamera* GetCamera() const;
	GameCamera* GetCamera();

public:
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

	GameCamera* myCamera;
};

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer();
	virtual ~CAirplanePlayer();

	void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
	void Update(float delta_time) override;

	virtual GameCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

private:
	GameObject* m_pMainRotorFrame = NULL;
	GameObject* m_pTailRotorFrame = NULL;
};


