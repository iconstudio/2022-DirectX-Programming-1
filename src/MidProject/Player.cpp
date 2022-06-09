#include "pch.hpp"
#include "Player.h"
#include "Pipeline.hpp"
#include "Model.hpp"
#include "Arithmetics.hpp"

constexpr COLLISION_TAGS CPlayer::GetTag() const noexcept
{
	return COLLISION_TAGS::PLAYER;
}

CPlayer::CPlayer()
	: GameObject()
{
	myCamera = NULL;

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	ReleaseUniforms();

	if (myCamera)
	{
		delete myCamera;
	}
}

void CPlayer::InitializeUniforms(P3DDevice device, P3DGrpCommandList cmdlist)
{
	if (myCamera)
	{
		myCamera->InitializeUniforms(device, cmdlist);
	}
}

void CPlayer::UpdateUniforms(P3DGrpCommandList cmdlist)
{}

void CPlayer::ReleaseUniforms()
{
	if (myCamera) myCamera->ReleaseUniforms();
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		myCamera->Move(xmf3Shift);
	}
}

void CPlayer::Move(float fxOffset, float fyOffset, float fzOffset)
{
	m_xmf3Position.x += fxOffset;
	m_xmf3Position.y += fyOffset;
	m_xmf3Position.z += fzOffset;
}

void CPlayer::MoveForward(float fDistance)
{
	const auto look = GetLookVector();
	const auto velocity = Vector3::ScalarProduct(look, fDistance);

	m_xmf3Position = Vector3::Add(m_xmf3Position, velocity);
}

void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCurrentCameraMode = myCamera->GetMode();
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		myCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		myCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Update(float fTimeElapsed)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Gravity);

	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);

	float fMaxVelocityXZ = m_fMaxVelocityXZ;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}

	float fMaxVelocityY = m_fMaxVelocityY;

	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY)
	{
		m_xmf3Velocity.y *= (fMaxVelocityY / fLength);
	}

	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	Move(xmf3Velocity, false);

	if (m_pPlayerUpdatedContext)
	{
		OnPlayerUpdateCallback(fTimeElapsed);
	}

	DWORD nCurrentCameraMode = myCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA)
	{
		myCamera->Update(m_xmf3Position, fTimeElapsed);
	}

	if (m_pCameraUpdatedContext)
	{
		OnCameraUpdateCallback(fTimeElapsed);
	}

	if (nCurrentCameraMode == THIRD_PERSON_CAMERA)
	{
		myCamera->SetLookAt(m_xmf3Position);
	}

	myCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void CPlayer::CollideWith(GameObject* other)
{
	const auto& tag = other->GetTag();
	switch (tag)
	{
		case COLLISION_TAGS::NONE:
		{}
		break;

		case COLLISION_TAGS::CAR:
		{
			constexpr float ot_mass = 100.0f;
			constexpr float my_mass = 100.0f;
			constexpr auto ratio = (1.0f + ot_mass / my_mass) * 10.0f;

			const auto& ot_pos = other->GetPosition();
			const auto& my_pos = GetPosition();

			const auto reflection = Vector3::Subtract(my_pos, ot_pos);
			const auto reflection_dir = Vector3::Normalize(reflection);

			const auto& my_vel = GetVelocity();
			const auto my_inverseddir = Vector3::ScalarProduct(my_vel, -1.0f, false);

			const auto power = Vector3::Add(my_inverseddir, reflection);
			const auto my_dir = Vector3::ScalarProduct(power, ratio, true);
			Move(my_dir, true);
		}
		break;

		case COLLISION_TAGS::ROCK:
		{
			const auto ot_mass = 20;
			constexpr float my_mass = 100.0f;
			constexpr auto ratio = (1.0f + ot_mass / my_mass) * 10.0f;

			const auto& ot_pos = other->GetPosition();
			const auto& my_pos = GetPosition();

			const auto reflection = Vector3::Subtract(my_pos, ot_pos);
			const auto reflection_dir = Vector3::Normalize(reflection);

			const auto& my_vel = GetVelocity();
			const auto my_inverseddir = Vector3::ScalarProduct(my_vel, -1.0f, false);

			const auto power = Vector3::Add(my_inverseddir, reflection);
			const auto my_dir = Vector3::ScalarProduct(power, ratio, true);
			Move(my_dir, true);
		}
		break;

		case COLLISION_TAGS::TREE:
		{
			const auto ot_mass = 50;
			constexpr float my_mass = 100.0f;
			constexpr auto ratio = (1.0f + ot_mass / my_mass) * 10.0f;

			const auto& ot_pos = other->GetPosition();
			const auto& my_pos = GetPosition();

			const auto reflection = Vector3::Subtract(my_pos, ot_pos);
			const auto reflection_dir = Vector3::Normalize(reflection);

			const auto& my_vel = GetVelocity();
			const auto my_inverseddir = Vector3::ScalarProduct(my_vel, -1.0f, false);

			const auto power = Vector3::Add(my_inverseddir, reflection);
			const auto my_dir = Vector3::ScalarProduct(power, ratio, true);
			Move(my_dir, true);
		}
		break;
	}
}

GameCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	GameCamera* pNewCamera = NULL;
	switch (nNewCameraMode)
	{
		case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(myCamera);
		break;
		case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(myCamera);
		break;
		case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(myCamera);
		break;
	}
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && myCamera)
	{
		m_xmf3Right = myCamera->GetRightVector();
		m_xmf3Up = myCamera->GetUpVector();
		m_xmf3Look = myCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (myCamera) delete myCamera;

	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	localTransform._11 = m_xmf3Right.x; localTransform._12 = m_xmf3Right.y; localTransform._13 = m_xmf3Right.z;
	localTransform._21 = m_xmf3Up.x; localTransform._22 = m_xmf3Up.y; localTransform._23 = m_xmf3Up.z;
	localTransform._31 = m_xmf3Look.x; localTransform._32 = m_xmf3Look.y; localTransform._33 = m_xmf3Look.z;
	localTransform._41 = m_xmf3Position.x; localTransform._42 = m_xmf3Position.y; localTransform._43 = m_xmf3Position.z;

	UpdateTransform(NULL);
}

void CPlayer::Render(P3DGrpCommandList cmdlist, GameCamera* pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;

	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		GameObject::Render(cmdlist, pCamera);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAirplanePlayer

CAirplanePlayer::CAirplanePlayer(P3DDevice device, P3DGrpCommandList cmdlist, P3DSignature signature)
{
	myCamera = ChangeCamera(THIRD_PERSON_CAMERA, 10.0f);

	Awake();

	InitializeUniforms(device, cmdlist);
}

CAirplanePlayer::~CAirplanePlayer()
{}

void CAirplanePlayer::Awake()
{
//	m_pMainRotorFrame = FindFrame("rotor");
//	m_pTailRotorFrame = FindFrame("black_m_7");

	m_pMainRotorFrame = FindFrame("Rotor");
	m_pTailRotorFrame = FindFrame("Back_Rotor");
}

void CAirplanePlayer::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{

	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
		m_pMainRotorFrame->localTransform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->localTransform);
	}

	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->localTransform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->localTransform);
	}

	CPlayer::Animate(fTimeElapsed, pxmf4x4Parent);
	UpdateTransform(pxmf4x4Parent);
}

void CAirplanePlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();
}

GameCamera* CAirplanePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (myCamera) ? myCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(myCamera);
	switch (nNewCameraMode)
	{
		case FIRST_PERSON_CAMERA:
		SetFriction(2.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(2.5f);
		SetMaxVelocityY(40.0f);
		myCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		myCamera->SetTimeLag(0.0f);
		myCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		myCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		myCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		myCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;

		case SPACESHIP_CAMERA:
		SetFriction(10.5f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(40.0f);
		SetMaxVelocityY(40.0f);
		myCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		myCamera->SetTimeLag(0.0f);
		myCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		myCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		myCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		myCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;

		case THIRD_PERSON_CAMERA:
		SetFriction(90.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(150.0f);
		SetMaxVelocityY(30.0f);
		myCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		myCamera->SetTimeLag(0.25f);
		myCamera->SetOffset(XMFLOAT3(0.0f, 60.0f, -90.0f));
		myCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		myCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		myCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;

		default:
		break;
	}

	myCamera->SetPosition(Vector3::Add(m_xmf3Position, myCamera->GetOffset()));
	Update(fTimeElapsed);

	return(myCamera);
}

