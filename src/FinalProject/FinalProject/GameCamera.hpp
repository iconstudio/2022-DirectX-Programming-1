#pragma once
#include "GameObject.hpp"

class GameCamera : public GameObject
{
public:
	GameCamera();
	virtual ~GameCamera();

	void Awake() override;
	void Start() override;
	void Reset() override;
	void Update(float delta_time) override;
	void PrepareRendering(P3DGrpCommandList cmdlist);
	void Render(P3DGrpCommandList cmdlist);
	void Release();

	void Init(P3DDevice device, P3DGrpCommandList cmdlist);
	void SetViewport(float width, float height);
	void SetFOVAngle(float angle);

	void CreatePerspectiveProjectionMatrix(float znear, float zfar);
	void CreateOrthographicProjectionMatrix(float znear, float zfar, float width, float height);

	void GenerateViewMatrix();

	bool IsInFrustum(const BoundingBox& collider) const;
	bool IsInFrustum(const BoundingOrientedBox& collider) const;
	bool IsInFrustum(const BoundingSphere& collider) const;

private:
	float myRoll, myYaw, myPitch;

	P3DDevice dxDevice;
	P3DGrpCommandList dxTaskList;

	ID3D12Resource* m_pd3dcbCamera;
	GameCameraBlob* m_pcbMappedCamera;

	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;
	float myFieldOfView;

	XMFLOAT4X4 projectionView;
	//XMFLOAT4X4 m_xmf4x4InverseView;

	XMFLOAT4X4 m_xmf4x4PerspectiveProject;
	//XMFLOAT4X4 projectionPerspective;

	XMFLOAT4X4 m_xmf4x4ViewOrthographicProject;
	//XMFLOAT4X4 projectionOrthographic;

	BoundingFrustum	staticCollider;
	BoundingFrustum	myCollider;
};
