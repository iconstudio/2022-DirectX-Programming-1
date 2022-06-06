#include "pch.hpp"
#include "Transformer.hpp"

Transformer::Transformer()
	: World(Matrix4x4::Identity())
	, myRight(World._11, World._12, World._13)
	, myUp(World._21, World._22, World._23)
	, myLook(World._31, World._32, World._33)
	, myPosition(World._41, World._42, World._43)
	, Updated(false)
{
	myRight = Transformer::Right;
	myUp = Transformer::Up;
	myLook = Transformer::Forward;
}

Transformer::~Transformer()
{}

void Transformer::SetWorldMatrix(const XMFLOAT4X4& mat)
{
	World = mat;
}

void Transformer::SetPosition(float x, float y, float z)
{
	myPosition.x = x;
	myPosition.y = y;
	myPosition.z = z;
}

void Transformer::SetPosition(const XMFLOAT3& pos)
{
	SetPosition(std::move(XMFLOAT3(pos)));
}

void Transformer::SetPosition(XMFLOAT3&& pos)
{
	myPosition.x = pos.x;
	myPosition.y = pos.y;
	myPosition.z = pos.z;
}

void Transformer::SetRotation(const XMFLOAT4X4& tfrm)
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

void Transformer::Translate(float x, float y, float z)
{
	myPosition.x += x;
	myPosition.y += y;
	myPosition.z += z;
}

void Transformer::Translate(const XMFLOAT3& shift)
{
	myPosition.x += shift.x;
	myPosition.y += shift.y;
	myPosition.z += shift.z;
}

void Transformer::Translate(XMFLOAT3&& shift)
{
	myPosition.x += shift.x;
	myPosition.y += shift.y;
	myPosition.z += shift.z;
}

void Transformer::Move(const XMFLOAT3& dir, float distance)
{
	Translate(Vector3::ScalarProduct(dir, distance));
}

void Transformer::Move(XMFLOAT3&& dir, float distance)
{
	Translate(Vector3::ScalarProduct(std::forward<XMFLOAT3>(dir), distance));
}

void Transformer::MoveStrafe(float distance)
{
	Move(XMFLOAT3(GetRight()), distance);
}

void Transformer::MoveUp(float distance)
{
	Move(XMFLOAT3(GetUp()), distance);
}

void Transformer::MoveForward(float distance)
{
	Move(XMFLOAT3(GetLook()), distance);
}

void Transformer::Rotate(const XMFLOAT4X4& tfrm)
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

void Transformer::Rotate(float pitch, float yaw, float roll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(pitch, yaw, roll);

	World = Matrix4x4::Multiply(mtxRotate, World);

	myRight = Vector3::Normalize(XMFLOAT3(myRight));
	myUp = Vector3::Normalize(XMFLOAT3(myUp));
	myLook = Vector3::Normalize(XMFLOAT3(myLook));

}

void Transformer::Rotate(const XMFLOAT3& axis, float angle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(axis, angle);

	World = Matrix4x4::Multiply(mtxRotate, World);

	myRight = Vector3::Normalize(XMFLOAT3(myRight));
	myUp = Vector3::Normalize(XMFLOAT3(myUp));
	myLook = Vector3::Normalize(XMFLOAT3(myLook));
}

void Transformer::LookTo(const XMFLOAT3& look, const XMFLOAT3& up)
{
	const auto&& view = Matrix4x4::LookToLH(XMFLOAT3(GetPosition()), look, up);

	World._11 = view._11; World._12 = view._21; World._13 = view._31;
	World._21 = view._12; World._22 = view._22; World._23 = view._32;
	World._31 = view._13; World._32 = view._23; World._33 = view._33;

	myRight = Vector3::Normalize(XMFLOAT3(myRight));
	myUp = Vector3::Normalize(XMFLOAT3(myUp));
	myLook = Vector3::Normalize(XMFLOAT3(myLook));
}

void Transformer::LookAt(const XMFLOAT3& look, const XMFLOAT3& up)
{
	const auto&& view = Matrix4x4::LookAtLH(XMFLOAT3(GetPosition()), look, up);

	World._11 = view._11; World._12 = view._21; World._13 = view._31;
	World._21 = view._12; World._22 = view._22; World._23 = view._32;
	World._31 = view._13; World._32 = view._23; World._33 = view._33;

	myRight = Vector3::Normalize(XMFLOAT3(myRight));
	myUp = Vector3::Normalize(XMFLOAT3(myUp));
	myLook = Vector3::Normalize(XMFLOAT3(myLook));
}

XMFLOAT4X4& Transformer::GetWorldMatrix()
{
	return World;
}

const XMFLOAT4X4& Transformer::GetWorldMatrix() const
{
	return World;
}

XYZWrapper& Transformer::GetRight()
{
	return myRight;
}

XYZWrapper& Transformer::GetUp()
{
	return myUp;
}

XYZWrapper& Transformer::GetLook()
{
	return myLook;
}

XYZWrapper& Transformer::GetPosition()
{
	return myPosition;
}

const XYZWrapper& Transformer::GetRight() const
{
	return myRight;
}

const XYZWrapper& Transformer::GetUp() const
{
	return myUp;
}

const XYZWrapper& Transformer::GetLook() const
{
	return myLook;
}

const XYZWrapper& Transformer::GetPosition() const
{
	return myPosition;
}
