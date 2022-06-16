#pragma once
#include "Transformer.hpp"

class CPlayer;

struct CBufferCamera
{
	XMFLOAT4X4						m_xmf4x4View;
	XMFLOAT4X4						m_xmf4x4Projection;
	XMFLOAT3						m_xmf3Position;
};

class GameCamera
{
protected:
	Transformer localTransform, worldTransform;
	XMFLOAT4X4 &localMatrix, &worldMatrix;

	XMFLOAT3						m_xmf3Position;
	XMFLOAT3						m_xmf3Right;
	XMFLOAT3						m_xmf3Up;
	XMFLOAT3						m_xmf3Look;

	float           				m_fPitch;
	float           				m_fRoll;
	float           				m_fYaw;

	DWORD m_nMode;

	XMFLOAT3 m_xmf3LookAtWorld;
	XMFLOAT3 m_xmf3Offset;
	XMFLOAT3 addtionalPositon;
	float m_fTimeLag;

	XMFLOAT4X4						m_xmf4x4View;
	XMFLOAT4X4						m_xmf4x4Projection;

	D3D12_VIEWPORT					m_d3dViewport;
	D3D12_RECT						m_d3dScissorRect;

	CPlayer* myPlayer = NULL;

	ID3D12Resource* m_pd3dcbCamera = NULL;
	CBufferCamera* m_pcbMappedCamera = NULL;

public:
	GameCamera();
	GameCamera(const GameCamera& cam) = default;
	GameCamera(GameCamera&& cam) = default;
	virtual ~GameCamera();

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist);
	virtual void UpdateOffset(const XMFLOAT3 addition);
	virtual void Update(const XMFLOAT3& look, float delta_time);
	virtual void PrepareRendering(P3DGrpCommandList cmdlist);
	virtual void Release();

	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void RegenerateViewMatrix();

	void BuildProjectionMatrix(float znear, float zfar, float aspect, float degfov);

	void SetViewport(int left, int top, int w, int h, float dmin = 0.0f, float dmax = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	void SetPlayer(CPlayer* pPlayer) { myPlayer = pPlayer; }
	CPlayer* GetPlayer() { return(myPlayer); }

	void SetMode(DWORD nMode) { m_nMode = nMode; }
	DWORD GetMode() const { return(m_nMode); }

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	XMFLOAT3& GetPosition() { return(m_xmf3Position); }

	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }
	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }

	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; }
	XMFLOAT3& GetOffset() { return(m_xmf3Offset); }

	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }

	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }
	D3D12_VIEWPORT GetViewport() { return(m_d3dViewport); }
	D3D12_RECT GetScissorRect() { return(m_d3dScissorRect); }

	virtual void Move(const XMFLOAT3& xmf3Shift) { m_xmf3Position.x += xmf3Shift.x; m_xmf3Position.y += xmf3Shift.y; m_xmf3Position.z += xmf3Shift.z; }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) {}
	virtual void SetLookAt(const XMFLOAT3& look) {}
};

class CSpaceShipCamera : public GameCamera
{
public:
	CSpaceShipCamera();
	CSpaceShipCamera(const GameCamera& pCamera);
	virtual ~CSpaceShipCamera();

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CFirstPersonCamera : public GameCamera
{
public:
	CFirstPersonCamera();
	CFirstPersonCamera(const GameCamera& pCamera);
	virtual ~CFirstPersonCamera();

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CThirdPersonCamera : public GameCamera
{
public:
	CThirdPersonCamera();
	CThirdPersonCamera(const GameCamera& pCamera);
	virtual ~CThirdPersonCamera();

	virtual void Update(const XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(const XMFLOAT3& vLookAt);
};

