#include "stdafx.hpp"
#include "GameTransform.hpp"

GameTransform::GameTransform()
	: World(Matrix4x4::Identity())
	, myRight(World._11, World._12, World._13)
	, myUp(World._21, World._22, World._23)
	, myLook(World._31, World._32, World._33)
	, myPosition(World._41, World._42, World._43)
	, Updated(false)
{
	myRight = GameTransform::Right;
	myUp = GameTransform::Up;
	myLook = GameTransform::Forward;
}

void GameTransform::SetPosition(float x, float y, float z)
{
	myPosition.X = x;
	myPosition.Y = y;
	myPosition.Z = z;
}

void GameTransform::SetPosition(const XMFLOAT3& pos)
{
	SetPosition(std::move(XMFLOAT3(pos)));
}

void GameTransform::SetPosition(XMFLOAT3&& pos)
{
	myPosition.X = pos.x;
	myPosition.Y = pos.y;
	myPosition.Z = pos.z;
}

void GameTransform::SetRotation(const XMFLOAT4X4& tfrm)
{
	myRight.X = tfrm._11;
	myRight.Y = tfrm._12;
	myRight.Z = tfrm._13;

	myUp.X = tfrm._21;
	myUp.Y = tfrm._22;
	myUp.Z = tfrm._23;

	myLook.X = tfrm._31;
	myLook.Y = tfrm._32;
	myLook.Z = tfrm._33;
}

void GameTransform::Translate(float x, float y, float z)
{
	myPosition.X += x;
	myPosition.Y += y;
	myPosition.Z += z;
}

void GameTransform::Translate(const XMFLOAT3& shift)
{
	myPosition.X += shift.x;
	myPosition.Y += shift.y;
	myPosition.Z += shift.z;
}

void GameTransform::Translate(XMFLOAT3&& shift)
{
	myPosition.X += shift.x;
	myPosition.Y += shift.y;
	myPosition.Z += shift.z;
}

void GameTransform::Move(const XMFLOAT3& dir, float distance)
{
	Translate(Vector3::ScalarProduct(Vector3::Normalize(dir), distance));
}

void GameTransform::Move(XMFLOAT3&& dir, float distance)
{
	Translate(Vector3::ScalarProduct(Vector3::Normalize(std::forward<XMFLOAT3>(dir)), distance));
}

void GameTransform::MoveStrafe(float distance)
{
	Move(XMFLOAT3(GetRight()), distance);
}

void GameTransform::MoveUp(float distance)
{
	Move(XMFLOAT3(GetUp()), distance);
}

void GameTransform::MoveForward(float distance)
{
	Move(XMFLOAT3(GetLook()), distance);
}

void GameTransform::Rotate(const XMFLOAT4X4& tfrm)
{
	myRight.X += tfrm._11;
	myRight.Y += tfrm._12;
	myRight.Z += tfrm._13;

	myUp.X += tfrm._21;
	myUp.Y += tfrm._22;
	myUp.Z += tfrm._23;

	myLook.X += tfrm._31;
	myLook.Y += tfrm._32;
	myLook.Z += tfrm._33;
}

void GameTransform::Rotate(float pitch, float yaw, float roll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(pitch, yaw, roll);

	World = Matrix4x4::Multiply(mtxRotate, World);
}

void GameTransform::Rotate(const XMFLOAT3& axis, float angle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(axis, angle);

	World = Matrix4x4::Multiply(mtxRotate, World);
}

void GameTransform::LookTo(const XMFLOAT3& look, const XMFLOAT3& up)
{
	auto&& view = Matrix4x4::LookToLH(XMFLOAT3(GetPosition()), look, up);

	World._11 = view._11; World._12 = view._21; World._13 = view._31;
	World._21 = view._12; World._22 = view._22; World._23 = view._32;
	World._31 = view._13; World._32 = view._23; World._33 = view._33;
}

void GameTransform::LookAt(const XMFLOAT3& look, const XMFLOAT3& up)
{
	auto&& view = Matrix4x4::LookAtLH(XMFLOAT3(GetPosition()), look, up);

	World._11 = view._11; World._12 = view._21; World._13 = view._31;
	World._21 = view._12; World._22 = view._22; World._23 = view._32;
	World._31 = view._13; World._32 = view._23; World._33 = view._33;
}

XMFLOAT4X4& GameTransform::GetWorldMatrix()
{
	return World;
}

const XMFLOAT4X4& GameTransform::GetWorldMatrix() const
{
	return World;
}

XYZWrapper& GameTransform::GetRight()
{
	return myRight;
}

XYZWrapper& GameTransform::GetUp()
{
	return myUp;
}

XYZWrapper& GameTransform::GetLook()
{
	return myLook;
}

XYZWrapper& GameTransform::GetPosition()
{
	return myPosition;
}
