#include "stdafx.h"
#include "GraphicsPipeline.h"

void Draw2DLine(HDC hDCFrameBuffer, XMFLOAT3& f3PreviousProject, XMFLOAT3& f3CurrentProject)
{
	XMFLOAT3 f3Previous = CGraphicsPipeline::ScreenTransform(f3PreviousProject);
	XMFLOAT3 f3Current = CGraphicsPipeline::ScreenTransform(f3CurrentProject);
	::MoveToEx(hDCFrameBuffer, (long)f3Previous.x, (long)f3Previous.y, NULL);
	::LineTo(hDCFrameBuffer, (long)f3Current.x, (long)f3Current.y);
}

XMFLOAT3&& Vector3::XMVectorToFloat3(const XMVECTOR& vector)
{
	return XMVectorToFloat3(std::move(XMVECTOR(vector)));
}

XMFLOAT3&& Vector3::XMVectorToFloat3(XMVECTOR&& vector)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, vector);

	return std::move(xmf3Result);
}

XMFLOAT3&& Vector3::ScalarProduct(const XMFLOAT3& vector, float scalar, bool normalize)
{
	return ScalarProduct(std::move(XMFLOAT3(vector)), scalar, normalize);
}

XMFLOAT3&& Vector3::ScalarProduct(XMFLOAT3&& vector, float scalar, bool normalize)
{
	auto temp_vector = XMLoadFloat3(&vector);

	XMFLOAT3 xmf3Result;
	if (normalize)
		XMStoreFloat3(&xmf3Result, XMVector3Normalize(temp_vector) * scalar);
	else
		XMStoreFloat3(&xmf3Result, temp_vector * scalar);

	return std::move(xmf3Result);
}

XMFLOAT3&& Vector3::Add(const XMFLOAT3& vector1, const XMFLOAT3& vector2, float scalar)
{
	return Add(std::move(XMFLOAT3(vector1)), std::move(XMFLOAT3(vector2)), scalar);
}

XMFLOAT3&& Vector3::Add(XMFLOAT3&& vector1, XMFLOAT3&& vector2, float scalar)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&vector1) + XMLoadFloat3(&vector2) * scalar);

	return std::move(xmf3Result);
}

XMFLOAT3&& Vector3::Subtract(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
{
	return Subtract(std::move(XMFLOAT3(vector1)), std::move(XMFLOAT3(vector2)));
}

XMFLOAT3&& Vector3::Subtract(XMFLOAT3&& vector1, XMFLOAT3&& vector2)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&vector1) - XMLoadFloat3(&vector2));

	return std::move(xmf3Result);
}

FLOAT Vector3::DotProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
{
	return DotProduct(std::move(XMFLOAT3(vector1)), std::move(XMFLOAT3(vector2)));
}

FLOAT Vector3::DotProduct(XMFLOAT3&& vector1, XMFLOAT3&& vector2)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&vector1), XMLoadFloat3(&vector2)));

	return (xmf3Result.x);
}

XMFLOAT3&& Vector3::CrossProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2, bool normalize)
{
	return CrossProduct(std::move(XMFLOAT3(vector1)), std::move(XMFLOAT3(vector2)), normalize);
}

XMFLOAT3&& Vector3::CrossProduct(XMFLOAT3&& vector1, XMFLOAT3&& vector2, bool normalize)
{
	XMFLOAT3 xmf3Result;

	auto normal1 = XMVector3Normalize(XMLoadFloat3(&vector1));
	auto normal2 = XMVector3Normalize(XMLoadFloat3(&vector2));
	if (normalize)
	{
		auto cross = XMVector3Cross(normal1, normal2);
		XMStoreFloat3(&xmf3Result, XMVector3Normalize(cross));
	}
	else
	{
		XMStoreFloat3(&xmf3Result, XMVector3Cross(normal1, normal2));
	}

	return std::move(xmf3Result);
}

XMFLOAT3&& Vector3::Normalize(const XMFLOAT3& vector)
{
	return Normalize(std::move(XMFLOAT3(vector)));
}

XMFLOAT3&& Vector3::Normalize(XMFLOAT3&& vector)
{
	XMFLOAT3 m_xmf3Normal;
	XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&vector)));

	return std::move(m_xmf3Normal);
}

FLOAT Vector3::Length(XMFLOAT3 vector)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&vector)));

	return (xmf3Result.x);
}

FLOAT Vector3::Distance(XMFLOAT3 vector1, XMFLOAT3 vector2)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMVector3Length(XMVectorSubtract(XMLoadFloat3(&vector1), XMLoadFloat3(&vector2))));

	return (xmf3Result.x);
}

FLOAT Vector3::Angle(XMVECTOR vector1, XMVECTOR vector2)
{
	XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(vector1, vector2);
	return XMVectorGetX(XMVectorACos(xmvAngle));
}

FLOAT Vector3::Angle(XMFLOAT3 vector1, XMFLOAT3 vector2)
{
	auto xmvAngle1 = XMVector3Normalize(XMLoadFloat3(&vector1));
	auto xmvAngle2 = XMVector3Normalize(XMLoadFloat3(&vector2));
	return Angle(xmvAngle1, xmvAngle2);
}

XMFLOAT3&& Vector3::TransformNormal(const XMFLOAT3& vector, const XMMATRIX& transform)
{
	return TransformNormal(std::move(XMFLOAT3(vector)), std::move(XMMATRIX(transform)));
}

XMFLOAT3&& Vector3::TransformNormal(XMFLOAT3&& vector, XMMATRIX&& transform)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&vector), transform));

	return std::move(xmf3Result);
}

XMFLOAT3&& Vector3::TransformNormal(const XMFLOAT3& vector, const XMFLOAT4X4& transform)
{
	return TransformNormal(std::move(XMFLOAT3(vector)), std::move(XMFLOAT4X4(transform)));
}

XMFLOAT3&& Vector3::TransformNormal(XMFLOAT3&& vector, XMFLOAT4X4&& transform)
{
	XMMATRIX xmf4x4 = XMLoadFloat4x4(&transform);

	return TransformNormal(std::move(vector), std::move(xmf4x4));
}

XMFLOAT3&& Vector3::TransformCoord(const XMFLOAT3& vector, const XMMATRIX& transform)
{
	return TransformCoord(std::move(XMFLOAT3(vector)), std::move(XMMATRIX(transform)));
}

XMFLOAT3&& Vector3::TransformCoord(XMFLOAT3&& vector, XMMATRIX&& transform)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&vector), transform));

	return std::move(xmf3Result);
}

XMFLOAT3&& Vector3::TransformCoord(const XMFLOAT3& vector, const XMFLOAT4X4& transform)
{
	return TransformCoord(std::move(XMFLOAT3(vector)), std::move(XMFLOAT4X4(transform)));
}

XMFLOAT3&& Vector3::TransformCoord(XMFLOAT3&& vector, XMFLOAT4X4&& transform)
{
	auto xmf4x4 = XMLoadFloat4x4(&transform);

	return TransformCoord(std::move(vector), std::move(xmf4x4));
}

XMFLOAT4&& Vector4::Add(const XMFLOAT4& vector1, const XMFLOAT4& vector2)
{
	return Add(std::move(XMFLOAT4(vector1)), std::move(XMFLOAT4(vector2)));
}

XMFLOAT4&& Vector4::Add(XMFLOAT4&& vector1, XMFLOAT4&& vector2)
{
	XMFLOAT4 xmf4Result;
	XMStoreFloat4(&xmf4Result, XMLoadFloat4(&vector1) + XMLoadFloat4(&vector2));

	return std::move(xmf4Result);
}

XMFLOAT4X4&& Matrix4x4::Identity()
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::Translate(float x, float y, float z)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranslation(x, y, z));

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::Inverse(const XMFLOAT4X4& matrix)
{
	return Inverse(std::move(XMFLOAT4X4(matrix)));
}

XMFLOAT4X4&& Matrix4x4::Inverse(XMFLOAT4X4&& matrix)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&matrix)));

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::Transpose(const XMFLOAT4X4& matrix)
{
	return Transpose(std::move(XMFLOAT4X4(matrix)));
}

XMFLOAT4X4&& Matrix4x4::Transpose(XMFLOAT4X4&& matrix)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&matrix)));

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::Multiply(const XMFLOAT4X4& matrix1, const XMFLOAT4X4& matrix2)
{
	return Multiply(std::move(XMFLOAT4X4(matrix1)), std::move(XMFLOAT4X4(matrix2)));
}

XMFLOAT4X4&& Matrix4x4::Multiply(XMFLOAT4X4&& matrix1, XMFLOAT4X4&& matrix2)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&matrix1) * XMLoadFloat4x4(&matrix2));

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::Multiply(const XMFLOAT4X4& matrix1, const XMMATRIX& matrix2)
{
	return Multiply(std::move(XMFLOAT4X4(matrix1)), std::move(XMMATRIX(matrix2)));
}

XMFLOAT4X4&& Matrix4x4::Multiply(XMFLOAT4X4&& matrix1, XMMATRIX&& matrix2)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&matrix1) * matrix2);

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::Multiply(const XMMATRIX& matrix1, const XMFLOAT4X4& matrix2)
{
	return Multiply(std::move(XMMATRIX(matrix1)), std::move(XMFLOAT4X4(matrix2)));
}

XMFLOAT4X4&& Matrix4x4::Multiply(XMMATRIX&& matrix1, XMFLOAT4X4&& matrix2)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, matrix1 * XMLoadFloat4x4(&matrix2));

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::Multiply(const XMMATRIX& matrix1, const XMMATRIX& matrix2)
{
	return Multiply(std::move(XMMATRIX(matrix1)), std::move(XMMATRIX(matrix2)));
}

XMFLOAT4X4&& Matrix4x4::Multiply(XMMATRIX&& matrix1, XMMATRIX&& matrix2)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, matrix1 * matrix2);

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::RotationYawPitchRoll(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 xmmtx4x4Result;
	auto rad_pitch = XMConvertToRadians(fPitch);
	auto rad_yaw = XMConvertToRadians(fYaw);
	auto rad_roll = XMConvertToRadians(fRoll);
	auto rot = XMMatrixRotationRollPitchYaw(rad_pitch, rad_yaw, rad_roll);
	XMStoreFloat4x4(&xmmtx4x4Result, rot);

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::RotationAxis(XMFLOAT3 xmf3Axis, float fAngle)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixRotationAxis(XMLoadFloat3(&xmf3Axis), XMConvertToRadians(fAngle)));

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::PerspectiveFovLH(float fFovAngleY, float fAspectRatio, float fNearZ, float fFarZ)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(XMConvertToRadians(fFovAngleY), fAspectRatio, fNearZ, fFarZ));

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::LookAtLH(const XMFLOAT3 eye_pos, const XMFLOAT3 look_pos, const XMFLOAT3 up_dir)
{
	XMFLOAT4X4 xmmtx4x4Result;

	auto eye = XMLoadFloat3(&eye_pos);
	auto look = XMLoadFloat3(&look_pos);
	auto up = XMLoadFloat3(&up_dir);
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(eye, look, up));

	return std::move(xmmtx4x4Result);
}

XMFLOAT4X4&& Matrix4x4::LookToLH(const XMFLOAT3 eye_pos, const XMFLOAT3 look_pos, const XMFLOAT3 up_dir)
{
	XMFLOAT4X4 xmmtx4x4Result;
	auto eye = XMLoadFloat3(&eye_pos);
	auto look = XMLoadFloat3(&look_pos);
	auto up = XMLoadFloat3(&up_dir);
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookToLH(eye, look, up));

	return std::move(xmmtx4x4Result);
}

XMFLOAT4&& Plane::Normalize(const XMFLOAT4& xmf4Plane)
{
	return Normalize(std::move(XMFLOAT4(xmf4Plane)));
}

XMFLOAT4&& Plane::Normalize(XMFLOAT4&& xmf4Plane)
{
	XMFLOAT4 xmf4Result;
	XMStoreFloat4(&xmf4Result, XMPlaneNormalize(XMLoadFloat4(&xmf4Plane)));

	return std::move(xmf4Result);
}