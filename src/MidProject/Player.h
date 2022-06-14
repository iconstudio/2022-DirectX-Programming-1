#pragma once
#include "GameEntity.hpp"
#include "Camera.h"

class CPlayer : public GameEntity
{
public:
	CPlayer();
	virtual ~CPlayer();

	void SetCamera(GameCamera* camera);

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
	virtual void Update(float delta_time) override;
	virtual void PrepareRendering(P3DGrpCommandList cmdlist) const override;
	virtual void Render(P3DGrpCommandList cmdlist, GameCamera* camera = NULL) const override;
	virtual void Release() override;

	virtual void CollideWith(GameObject* other) override;

	void SetPlayerUpdatedContext(LPVOID pContext)
	{
		m_pPlayerUpdatedContext = pContext;
	}
	void SetCameraUpdatedContext(LPVOID pContext)
	{
		m_pCameraUpdatedContext = pContext;
	}

	virtual GameCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return nullptr;  };
	GameCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual constexpr COLLISION_TAGS GetTag() const noexcept override;

	void Rotate(float x, float y, float z);
	void Accelerate(DWORD direction, float dist);

	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }
	const GameCamera* GetCamera() const;
	GameCamera* GetCamera();

public:
	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

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

	virtual GameCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) override;

private:
	GameObject* m_pMainRotorFrame = NULL;
	GameObject* m_pTailRotorFrame = NULL;
};


