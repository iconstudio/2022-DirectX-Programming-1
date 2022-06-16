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
	SetMaxXZVelocity(90.0f);
	SetMaxYVelocity(150.0f);

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

void CPlayer::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	GameEntity::Awake(device, cmdlist);

	myCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0);
	if (myCamera)
	{
		myCamera->Awake(device, cmdlist);
	}
}

void CPlayer::Update(float delta_time)
{
	GameEntity::Accelerate(myGravity);
	GameEntity::Update(delta_time);

	const auto movement = Vector3::ScalarProduct(GetVelocity(), delta_time, false);

	myCamera->Move(movement);

	DWORD nCurrentCameraMode = myCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA)
	{
		const auto my_pos = GetPosition();

		myCamera->Update(my_pos, delta_time);
		myCamera->SetLookAt(my_pos);
	}

	myCamera->RegenerateViewMatrix();
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
		myCamera->Release();
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

GameCamera* CPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (myCamera) ? myCamera->GetMode() : 0x00;

	if (nCurrentCameraMode == nNewCameraMode)
		return(myCamera);
	switch (nNewCameraMode)
	{
		case FIRST_PERSON_CAMERA:
		myCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		myCamera->SetTimeLag(0.0f);
		myCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		break;

		case THIRD_PERSON_CAMERA:
		myCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		myCamera->SetTimeLag(0.25f);
		myCamera->SetOffset(XMFLOAT3(0.0f, 60.0f, -80.0f));
		break;

		case SPACESHIP_CAMERA:
		myCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		myCamera->SetTimeLag(0.25f);
		myCamera->SetOffset(XMFLOAT3(0.0f, 60.0f, -60.0f));
		break;

		default:
		break;
	}

	myCamera->SetPosition(Vector3::Add(GetPosition(), myCamera->GetOffset()));
	myCamera->BuildProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
	myCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	myCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	Update(fTimeElapsed);

	return(myCamera);
}

GameCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	GameCamera* pNewCamera = nullptr;
	switch (nNewCameraMode)
	{
		case FIRST_PERSON_CAMERA:
		{
			if (myCamera)
				pNewCamera = new CFirstPersonCamera(*myCamera);
			else
				pNewCamera = new CFirstPersonCamera();
		}
		break;

		case THIRD_PERSON_CAMERA:
		{
			if (myCamera)
				pNewCamera = new CThirdPersonCamera(*myCamera);
			else
				pNewCamera = new CThirdPersonCamera();
		}
		break;

		case SPACESHIP_CAMERA:
		{
			if (myCamera)
				pNewCamera = new CSpaceShipCamera(*myCamera);
			else
				pNewCamera = new CSpaceShipCamera();
		}
		break;
	}

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

const GameCamera* CPlayer::GetCamera() const
{
	return myCamera;
}

GameCamera* CPlayer::GetCamera()
{
	return myCamera;
}
