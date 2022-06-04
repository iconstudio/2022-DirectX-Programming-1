#include "pch.hpp"
#include "GameCamera.hpp"
#include "Arithmetics.hpp"
#include "GraphicsCore.hpp"

GameCamera::GameCamera()
	: GameObject()
	, myRoll(0.0f), myYaw(0.0f), myPitch(0.0f)
	, dxDevice(nullptr), dxTaskList(nullptr)
	, m_d3dViewport(), m_d3dScissorRect(), myFieldOfView(60.0f)
	, projectionView(Matrix4x4::Identity())
	//, m_xmf4x4InverseView(Matrix4x4::Identity())
	, m_xmf4x4PerspectiveProject(Matrix4x4::Identity())
	//, projectionPerspective(Matrix4x4::Identity())
	, m_xmf4x4ViewOrthographicProject(Matrix4x4::Identity())
	//, projectionOrthographic(Matrix4x4::Identity())
	, staticCollider(), myCollider()
	, m_pd3dcbCamera(nullptr), m_pcbMappedCamera(nullptr)
{}

GameCamera::~GameCamera()
{}

void GameCamera::Awake()
{}

void GameCamera::Start()
{
	UINT aligned_sz = ((sizeof(GameCameraBlob) + 255) & ~255);
	m_pd3dcbCamera = ::CreateBufferResource(dxDevice, dxTaskList
		, NULL, aligned_sz
		, D3D12_HEAP_TYPE_UPLOAD
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, NULL);

	auto place = reinterpret_cast<void**>(&m_pcbMappedCamera);
	auto valid = m_pd3dcbCamera->Map(0, NULL, place);
	if (FAILED(valid))
	{
		throw "카메라 초기화 실패!";
	}
}

void GameCamera::Reset()
{}

void GameCamera::Update(float delta_time)
{
	GenerateViewMatrix();
}

void GameCamera::PrepareRendering(P3DGrpCommandList cmdlist)
{
	cmdlist->RSSetViewports(1, &m_d3dViewport);
	cmdlist->RSSetScissorRects(1, &m_d3dScissorRect);

	const auto myPosition = GetPosition();

	XMFLOAT4X4 xmf4x4View{};
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&projectionView)));

	memcpy(&m_pcbMappedCamera->m_xmf4x4View, &xmf4x4View, sizeof(XMFLOAT4X4));

	XMFLOAT4X4 xmf4x4Projection{};
	XMStoreFloat4x4(&m_xmf4x4PerspectiveProject, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4PerspectiveProject)));

	memcpy(&m_pcbMappedCamera->m_xmf4x4Projection, &xmf4x4Projection, sizeof(XMFLOAT4X4));

	memcpy(&m_pcbMappedCamera->m_xmf3Position, &myPosition, sizeof(XMFLOAT3));

	const auto gpu_address = m_pd3dcbCamera->GetGPUVirtualAddress();
	cmdlist->SetGraphicsRootConstantBufferView(0, gpu_address);
}

void GameCamera::Render(P3DGrpCommandList cmdlist)
{}

void GameCamera::Release()
{
	if (m_pd3dcbCamera)
	{
		m_pd3dcbCamera->Unmap(0, NULL);
		m_pd3dcbCamera->Release();
	}
}

void GameCamera::Init(P3DDevice device, P3DGrpCommandList cmdlist)
{
	dxDevice = device;
	dxTaskList = cmdlist;
}

void GameCamera::SetViewport(float width, float height)
{
	m_d3dViewport = { 0, 0, width, height, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, (long)width , (long)height };
}

void GameCamera::SetFOVAngle(float angle)
{
	myFieldOfView = angle;
}

void GameCamera::CreatePerspectiveProjectionMatrix(float znear, float zfar)
{
	const auto rad = XMConvertToRadians(myFieldOfView);
	XMMATRIX projection = XMMatrixPerspectiveFovLH(rad, FRAME_BUFFER_ASPECT, znear, zfar);

	XMStoreFloat4x4(&m_xmf4x4PerspectiveProject, projection);

	BoundingFrustum::CreateFromMatrix(staticCollider, projection);
}

void GameCamera::CreateOrthographicProjectionMatrix(float znear, float zfar, float w, float h)
{
	XMMATRIX projection = XMMatrixOrthographicLH(w, h, znear, zfar);
	XMStoreFloat4x4(&m_xmf4x4ViewOrthographicProject, projection);
}

void GameCamera::GenerateViewMatrix()
{
	auto& myRight = worldTransform.GetRight();
	auto& myUp = worldTransform.GetUp();
	auto& myLook = worldTransform.GetLook();
	auto myPosition = XMFLOAT3(worldTransform.GetPosition());

	worldTransform.myRight = Vector3::Normalize(XMFLOAT3(myRight));
	worldTransform.myUp = Vector3::Normalize(XMFLOAT3(myUp));
	worldTransform.myLook = Vector3::Normalize(XMFLOAT3(myLook));

	// 카메라를 위한 카메라 변환 행렬
	// 1행
	projectionView._11 = myRight.x;
	projectionView._12 = myUp.x;
	projectionView._13 = myLook.x;

	// 2행
	projectionView._21 = myRight.y;
	projectionView._22 = myUp.y;
	projectionView._23 = myLook.y;

	// 3행
	projectionView._31 = myRight.z;
	projectionView._32 = myUp.z;
	projectionView._33 = myLook.z;

	// 4행
	projectionView._41 = -Vector3::DotProduct(myPosition, XMFLOAT3(myRight));
	projectionView._42 = -Vector3::DotProduct(myPosition, XMFLOAT3(myUp));
	projectionView._43 = -Vector3::DotProduct(myPosition, XMFLOAT3(myLook));

	// 원근 투영 행렬
	//projectionPerspective = Matrix4x4::Multiply(projectionView, m_xmf4x4PerspectiveProject);

	// 직교 투영 행렬
	//projectionOrthographic = Matrix4x4::Multiply(projectionView, m_xmf4x4ViewOrthographicProject);

	// 카메라를 위한 월드 변환 행렬
	// 충돌체를 월드 위치로 옮긴다.
	const auto& world_mat = worldTransform.GetWorldMatrix();

	staticCollider.Transform(myCollider, XMLoadFloat4x4(&world_mat));
}

bool GameCamera::IsInFrustum(const BoundingBox& collider) const
{
	return (myCollider.Intersects(collider));
}

bool GameCamera::IsInFrustum(const BoundingOrientedBox& collider) const
{
	return (myCollider.Intersects(collider));
}

bool GameCamera::IsInFrustum(const BoundingSphere& collider) const
{
	return (myCollider.Intersects(collider));
}
