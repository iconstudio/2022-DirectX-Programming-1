#include "pch.hpp"
#include "Player.h"
#include "Pipeline.hpp"
#include "Model.hpp"
#include "Arithmetics.hpp"

CPlayer::CPlayer()
	: GameEntity()
	, myCamera(nullptr)
{
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	SetFriction(50.0f);
	SetGravity(XMFLOAT3());
	SetMaxXZVelocity(2.5f);
	SetMaxYVelocity(40.0f);

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	Release();
	ReleaseUploadBuffers();

	if (myCamera)
	{
		delete myCamera;
	}
}

void CPlayer::SetCamera(GameCamera* camera)
{
	myCamera = camera;
}

void CPlayer::Accelerate(DWORD direction, float dist)
{
	if (direction)
	{
		auto shift = XMFLOAT3(0, 0, 0);
		const auto right = GetRight();
		const auto up = GetUp();
		const auto look = GetLook();

		if (direction & DIR_FORWARD) shift = Vector3::Add(shift, look, dist);
		if (direction & DIR_BACKWARD) shift = Vector3::Add(shift, look, -dist);
		if (direction & DIR_RIGHT) shift = Vector3::Add(shift, right, dist);
		if (direction & DIR_LEFT) shift = Vector3::Add(shift, right, -dist);
		if (direction & DIR_UP) shift = Vector3::Add(shift, up, dist);
		if (direction & DIR_DOWN) shift = Vector3::Add(shift, up, -dist);

		GameEntity::Accelerate(shift);
	}
}

void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCurrentCameraMode = myCamera->GetMode();

	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f)
			{
				x -= (m_fPitch - 89.0f); m_fPitch = +89.0f;
			}
			if (m_fPitch < -89.0f)
			{
				x -= (m_fPitch + 89.0f); m_fPitch = -89.0f;
			}
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

		GameEntity::Rotate(0.0f, y, 0.0f);
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		myCamera->Rotate(x, y, z);

		GameEntity::Rotate(x, y, z);
	}
}

void CPlayer::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	GameEntity::Awake(device, cmdlist);

	if (myCamera)
	{
		myCamera->InitializeUniforms(device, cmdlist);
	}
}

void CPlayer::Update(float delta_time)
{
	const auto my_pos = GetPosition();

	GameEntity::Accelerate(myGravity);

	auto velocity = GetVelocity();

	auto xz_vel = Vector3::CrossProduct(velocity, { 0, 1, 0 }, false);

	auto& y_vel = velocity.y;
	auto y_speed = abs(velocity.y);
	if (m_fMaxVelocityY < y_speed)
	{
		y_speed *= (m_fMaxVelocityY / y_speed);
	}

	const auto movement = Vector3::ScalarProduct(velocity, delta_time, false);

	if (0.0f != mySpeed)
	{
		myCamera->Move(movement);
	}

	DWORD nCurrentCameraMode = myCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA)
	{
		myCamera->Update(my_pos, delta_time);

		myCamera->SetLookAt(my_pos);
	}

	myCamera->RegenerateViewMatrix();

	GameEntity::Update(delta_time);
}

void CPlayer::PrepareRendering(P3DGrpCommandList cmdlist) const
{
	GameEntity::PrepareRendering(cmdlist);
}

void CPlayer::Render(P3DGrpCommandList cmdlist, GameCamera* pCamera) const
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;

	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		GameEntity::Render(cmdlist, pCamera);
	}
}

void CPlayer::Release()
{
	if (myCamera)
	{
		myCamera->ReleaseUniforms();
	}
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
			const auto accel = Vector3::ScalarProduct(power, ratio, true);
			GameEntity::Accelerate(accel);
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
			const auto accel = Vector3::ScalarProduct(power, ratio, true);
			GameEntity::Accelerate(accel);
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
			const auto accel = Vector3::ScalarProduct(power, ratio, true);
			GameEntity::Accelerate(accel);
		}
		break;
	}
}

GameCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	GameCamera* pNewCamera = NULL;

	pNewCamera = new CThirdPersonCamera(myCamera);

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (myCamera) delete myCamera;

	return(pNewCamera);
}

constexpr COLLISION_TAGS CPlayer::GetTag() const noexcept
{
	return COLLISION_TAGS::PLAYER;
}

const GameCamera* CPlayer::GetCamera() const
{
	return myCamera;
}

GameCamera* CPlayer::GetCamera()
{
	return myCamera;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAirplanePlayer

void CAirplanePlayer::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	CPlayer::Awake(device, cmdlist);
}

CAirplanePlayer::CAirplanePlayer()
	: CPlayer()
{
	myCamera = ChangeCamera(THIRD_PERSON_CAMERA, 10.0f);

	m_pMainRotorFrame = FindFrame("Rotor");
	m_pTailRotorFrame = FindFrame("Back_Rotor");
}

CAirplanePlayer::~CAirplanePlayer()
{}

void CAirplanePlayer::Update(float delta_time)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * delta_time);
		m_pMainRotorFrame->localMatrix = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->localMatrix);
	}

	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * delta_time);
		m_pTailRotorFrame->localMatrix = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->localMatrix);
	}

	CPlayer::Update(delta_time);
}

GameCamera* CAirplanePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (myCamera) ? myCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(myCamera);
	switch (nNewCameraMode)
	{
		case FIRST_PERSON_CAMERA:
		myCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		myCamera->SetTimeLag(0.0f);
		myCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		myCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		myCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		myCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;

		case SPACESHIP_CAMERA:
		myCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		myCamera->SetTimeLag(0.0f);
		myCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		myCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		myCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		myCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;

		case THIRD_PERSON_CAMERA:
		myCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		myCamera->SetTimeLag(0.25f);
		myCamera->SetOffset(XMFLOAT3(0.0f, 60.0f, -80.0f));
		myCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		myCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		myCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;

		default:
		break;
	}

	myCamera->SetPosition(Vector3::Add(GetPosition(), myCamera->GetOffset()));
	Update(fTimeElapsed);

	return(myCamera);
}

