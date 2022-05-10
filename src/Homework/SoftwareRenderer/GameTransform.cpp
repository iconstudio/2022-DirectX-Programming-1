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

void GameTransform::SetWorldMatrix(const XMFLOAT4X4& mat)
{
	World = mat;
}

void GameTransform::SetPosition(float x, float y, float z)
{
	myPosition.x = x;
	myPosition.y = y;
	myPosition.z = z;
}

void GameTransform::SetPosition(const XMFLOAT3& pos)
{
	SetPosition(std::move(XMFLOAT3(pos)));
}

void GameTransform::SetPosition(XMFLOAT3&& pos)
{
	myPosition.x = pos.x;
	myPosition.y = pos.y;
	myPosition.z = pos.z;
}

void GameTransform::SetRotation(const XMFLOAT4X4& tfrm)
{
	myRight.x = tfrm._11;
	myRight.y = tfrm._12;
	myRight.z = tfrm._13;

	myUp.x = tfrm._21;
	myUp.y = tfrm._22;
	myUp.z = tfrm._23;

	myLook.x = tfrm._31;
	myLook.y = tfrm._32;
	myLook.z = tfrm._33;
}

void GameTransform::Translate(float x, float y, float z)
{
	myPosition.x += x;
	myPosition.y += y;
	myPosition.z += z;
}

void GameTransform::Translate(const XMFLOAT3& shift)
{
	myPosition.x += shift.x;
	myPosition.y += shift.y;
	myPosition.z += shift.z;
}

void GameTransform::Translate(XMFLOAT3&& shift)
{
	myPosition.x += shift.x;
	myPosition.y += shift.y;
	myPosition.z += shift.z;
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
	myRight.x += tfrm._11;
	myRight.y += tfrm._12;
	myRight.z += tfrm._13;

	myUp.x += tfrm._21;
	myUp.y += tfrm._22;
	myUp.z += tfrm._23;

	myLook.x += tfrm._31;
	myLook.y += tfrm._32;
	myLook.z += tfrm._33;
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

const XYZWrapper& GameTransform::GetRight() const
{
	return myRight;
}

const XYZWrapper& GameTransform::GetUp() const
{
	return myUp;
}

const XYZWrapper& GameTransform::GetLook() const
{
	return myLook;
}

const XYZWrapper& GameTransform::GetPosition() const
{
	return myPosition;
}
