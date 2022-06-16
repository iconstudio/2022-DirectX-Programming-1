#include "pch.hpp"
#include "Transformer.hpp"
#include "Arithmetics.hpp"

Transformer::Transformer()
	: myMatrix(Matrix4x4::Identity())
	, myRight(myMatrix._11, myMatrix._12, myMatrix._13)
	, myUp(myMatrix._21, myMatrix._22, myMatrix._23)
	, myLook(myMatrix._31, myMatrix._32, myMatrix._33)
	, myPosition(myMatrix._41, myMatrix._42, myMatrix._43)
	, Updated(false)
{
	myRight = Transformer::Right;
	myUp = Transformer::Up;
	myLook = Transformer::Forward;
}

Transformer::~Transformer()
{}

void Transformer::SetMatrix(const XMFLOAT4X4& mat)
{
	myMatrix = mat;
}

void Transformer::SetMatrix(XMFLOAT4X4&& mat)
{
	myMatrix = std::forward<XMFLOAT4X4>(mat);
}

void Transformer::SetScale(float x, float y, float z)
{
	auto mtxScale = XMMatrixScaling(x, y, z);
	myMatrix = Matrix4x4::Multiply(mtxScale, myMatrix);
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

void Transformer::SetRotation(XMFLOAT4X4&& tfrm)
{
	myRight.x = std::forward<float>(tfrm._11);
	myRight.y = std::forward<float>(tfrm._12);
	myRight.z = std::forward<float>(tfrm._13);

	myUp.x = std::forward<float>(tfrm._21);
	myUp.y = std::forward<float>(tfrm._22);
	myUp.z = std::forward<float>(tfrm._23);

	myLook.x = std::forward<float>(tfrm._31);
	myLook.y = std::forward<float>(tfrm._32);
	myLook.z = std::forward<float>(tfrm._33);
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
	myPosition.x += std::forward<float>(shift.x);
	myPosition.y += std::forward<float>(shift.y);
	myPosition.z += std::forward<float>(shift.z);
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

void Transformer::Rotate(XMFLOAT4X4&& tfrm)
{
	myRight.x += std::forward<float>(tfrm._11);
	myRight.y += std::forward<float>(tfrm._12);
	myRight.z += std::forward<float>(tfrm._13);

	myUp.x += std::forward<float>(tfrm._21);
	myUp.y += std::forward<float>(tfrm._22);
	myUp.z += std::forward<float>(tfrm._23);

	myLook.x += std::forward<float>(tfrm._31);
	myLook.y += std::forward<float>(tfrm._32);
	myLook.z += std::forward<float>(tfrm._33);
}

void Transformer::Rotate(float pitch, float yaw, float roll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(pitch, yaw, roll);

	myMatrix = Matrix4x4::Multiply(mtxRotate, myMatrix);

	myRight = Vector3::Normalize(XMFLOAT3(myRight));
	myUp = Vector3::Normalize(XMFLOAT3(myUp));
	myLook = Vector3::Normalize(XMFLOAT3(myLook));

}

void Transformer::Rotate(const XMFLOAT3& axis, float angle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(axis, angle);

	myMatrix = Matrix4x4::Multiply(mtxRotate, myMatrix);

	myRight = Vector3::Normalize(XMFLOAT3(myRight));
	myUp = Vector3::Normalize(XMFLOAT3(myUp));
	myLook = Vector3::Normalize(XMFLOAT3(myLook));
}

void Transformer::Rotate(const XMFLOAT4& quaternion)
{
	auto mid = XMLoadFloat4(&quaternion);
	auto mtxRotate = XMMatrixRotationQuaternion(mid);
	myMatrix = Matrix4x4::Multiply(mtxRotate, myMatrix);
}

void Transformer::Rotate(XMFLOAT4&& quaternion)
{
	auto mid = XMLoadFloat4(std::forward<XMFLOAT4*>(&quaternion));
	auto mtxRotate = XMMatrixRotationQuaternion(mid);
	myMatrix = Matrix4x4::Multiply(mtxRotate, myMatrix);
}

void Transformer::LookTo(const XMFLOAT3& look, const XMFLOAT3& up)
{
	const auto&& view = Matrix4x4::LookToLH(XMFLOAT3(GetPosition()), look, up);

	myMatrix._11 = view._11; myMatrix._12 = view._21; myMatrix._13 = view._31;
	myMatrix._21 = view._12; myMatrix._22 = view._22; myMatrix._23 = view._32;
	myMatrix._31 = view._13; myMatrix._32 = view._23; myMatrix._33 = view._33;

	myRight = Vector3::Normalize(XMFLOAT3(myRight));
	myUp = Vector3::Normalize(XMFLOAT3(myUp));
	myLook = Vector3::Normalize(XMFLOAT3(myLook));
}

void Transformer::LookAt(const XMFLOAT3& look, const XMFLOAT3& up)
{
	const auto&& view = Matrix4x4::LookAtLH(XMFLOAT3(GetPosition()), look, up);

	myMatrix._11 = view._11; myMatrix._12 = view._21; myMatrix._13 = view._31;
	myMatrix._21 = view._12; myMatrix._22 = view._22; myMatrix._23 = view._32;
	myMatrix._31 = view._13; myMatrix._32 = view._23; myMatrix._33 = view._33;

	myRight = Vector3::Normalize(XMFLOAT3(myRight));
	myUp = Vector3::Normalize(XMFLOAT3(myUp));
	myLook = Vector3::Normalize(XMFLOAT3(myLook));
}

XMFLOAT4X4& Transformer::GetMatrix()
{
	return myMatrix;
}

const XMFLOAT4X4& Transformer::GetMatrix() const
{
	return myMatrix;
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
