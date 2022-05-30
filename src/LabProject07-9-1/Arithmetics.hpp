#pragma once

namespace Vector3
{
	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR&& xmvVector)
	{
		XMFLOAT3 result{};

		XMStoreFloat3(&result, std::forward<XMVECTOR>(xmvVector));

		return result;
	}

	inline XMFLOAT3 XMVectorToFloat3(const XMVECTOR& xmvVector)
	{
		return XMVectorToFloat3(XMVECTOR(xmvVector));
	}

	inline XMFLOAT3 ScalarProduct(XMFLOAT3&& vector, float scalar, bool normalize = true)
	{
		XMFLOAT3 result{};

		const auto xmvector = XMLoadFloat3(std::forward<XMFLOAT3*>(&vector));
		if (normalize)
		{
			XMStoreFloat3(&result, XMVector3Normalize(xmvector) * scalar);
		}
		else
		{
			XMStoreFloat3(&result, xmvector * scalar);
		}

		return result;
	}

	inline XMFLOAT3 ScalarProduct(const XMFLOAT3& vector, float scalar, bool normalize = true)
	{
		return ScalarProduct(XMFLOAT3(vector), scalar, normalize);
	}

	inline XMFLOAT3 Add(XMFLOAT3&& vector1, XMFLOAT3&& vector2)
	{
		XMFLOAT3 result{};

		const auto&& fvector1 = std::forward<XMFLOAT3>(vector1);
		const auto&& fvector2 = std::forward<XMFLOAT3>(vector2);

		const auto xmvector1 = XMLoadFloat3(&fvector1);
		const auto xmvector2 = XMLoadFloat3(&fvector2);
		XMStoreFloat3(&result, xmvector1 + xmvector2);

		return result;
	}

	inline XMFLOAT3 Add(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		return Add(XMFLOAT3(vector1), XMFLOAT3(vector2));
	}

	inline XMFLOAT3 Add(const XMFLOAT3& vector1, XMFLOAT3&& vector2)
	{
		return Add(XMFLOAT3(vector1), std::forward<XMFLOAT3>(vector2));
	}

	inline XMFLOAT3 Add(XMFLOAT3&& vector1, const XMFLOAT3& vector2)
	{
		return Add(std::forward<XMFLOAT3>(vector1), XMFLOAT3(vector2));
	}

	inline XMFLOAT3 Add(const XMFLOAT3& vector1, const XMFLOAT3& vector2, float scalar)
	{
		XMFLOAT3 result{};

		XMStoreFloat3(&result, XMLoadFloat3(&vector1) + (XMLoadFloat3(&vector2) * scalar));

		return result;
	}

	inline XMFLOAT3 Subtract(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		XMFLOAT3 result{};

		XMStoreFloat3(&result, XMLoadFloat3(&vector1) - XMLoadFloat3(&vector2));

		return result;
	}

	inline float DotProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		XMFLOAT3 result{};

		XMStoreFloat3(&result, XMVector3Dot(XMLoadFloat3(&vector1), XMLoadFloat3(&vector2)));

		return result.x;
	}

	inline XMFLOAT3 CrossProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2, bool normalize = true)
	{
		XMFLOAT3 result{};

		if (normalize)
		{
			XMStoreFloat3(&result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&vector1), XMLoadFloat3(&vector2))));
		}
		else
		{
			XMStoreFloat3(&result, XMVector3Cross(XMLoadFloat3(&vector1), XMLoadFloat3(&vector2)));
		}
		return result;
	}

	inline XMFLOAT3 Normalize(XMFLOAT3&& vector)
	{
		XMFLOAT3 normal{};

		const auto mid = XMLoadFloat3(std::forward<XMFLOAT3*>(&vector));
		XMStoreFloat3(&normal, XMVector3Normalize(mid));

		return normal;
	}

	inline XMFLOAT3 Normalize(const XMFLOAT3& vector)
	{
		return Normalize(XMFLOAT3(vector));
	}

	inline float Length(XMFLOAT3&& vector)
	{
		XMFLOAT3 result{};

		const auto mid = XMLoadFloat3(std::forward<XMFLOAT3*>(&vector));
		XMStoreFloat3(&result, XMVector3Length(mid));

		return(result.x);
	}

	inline float Length(const XMFLOAT3& vector)
	{
		return Length(XMFLOAT3(vector));
	}

	inline float Angle(XMVECTOR&& xvector1, XMVECTOR&& xvector2)
	{
		const auto&& pos1 = std::forward<XMVECTOR>(xvector1);
		const auto&& pos2 = std::forward<XMVECTOR>(xvector2);

		return XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(pos1, pos2)));
	}

	inline float Angle(const XMVECTOR& xvector1, const XMVECTOR& xvector2)
	{
		return Angle(XMVECTOR(xvector1), XMVECTOR(xvector2));
	}

	inline float Angle(const XMVECTOR& xvector1, XMVECTOR&& xvector2)
	{
		return Angle(XMVECTOR(xvector1), std::forward<XMVECTOR>(xvector2));
	}

	inline float Angle(XMVECTOR&& xvector1, const XMVECTOR& xvector2)
	{
		return Angle(std::forward<XMVECTOR>(xvector1), XMVECTOR(xvector2));
	}

	inline float Angle(XMFLOAT3&& vector1, XMFLOAT3&& vector2)
	{
		const auto&& vec1 = std::forward<XMFLOAT3>(vector1);
		const auto&& vec2 = std::forward<XMFLOAT3>(vector2);

		return Angle(XMLoadFloat3(&vec1), XMLoadFloat3(&vec2));
	}

	inline float Angle(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		return Angle(XMLoadFloat3(&vector1), XMLoadFloat3(&vector2));
	}

	inline XMFLOAT3 TransformNormal(XMFLOAT3&& vector, XMMATRIX&& transform)
	{
		XMFLOAT3 result{};

		XMStoreFloat3(&result
			, XMVector3TransformNormal(XMLoadFloat3(std::forward<XMFLOAT3*>(&vector))
			, std::forward<XMMATRIX>(transform)));

		return result;
	}

	inline XMFLOAT3 TransformNormal(const XMFLOAT3& vector, const XMMATRIX& transform)
	{
		return TransformNormal(XMFLOAT3(vector), XMMATRIX(transform));
	}

	inline XMFLOAT3 TransformNormal(const XMFLOAT3& vector, XMMATRIX&& transform)
	{
		return TransformNormal(XMFLOAT3(vector), std::forward<XMMATRIX>(transform));
	}

	inline XMFLOAT3 TransformNormal(XMFLOAT3&& vector, const XMMATRIX&& transform)
	{
		return TransformNormal(std::forward<XMFLOAT3>(vector), XMMATRIX(transform));
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3&& vector, XMMATRIX&& transform)
	{
		XMFLOAT3 result{};

		XMStoreFloat3(&result
			, XMVector3TransformCoord(XMLoadFloat3(std::forward<XMFLOAT3*>(&vector))
			, transform));

		return result;
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& vector, const XMMATRIX& transform)
	{
		return TransformCoord(XMFLOAT3(vector), XMMATRIX(transform));
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3&& vector, XMFLOAT4X4&& transform)
	{
		XMFLOAT3 result{};

		XMStoreFloat3(&result
			, XMVector3TransformCoord(XMLoadFloat3(std::forward<XMFLOAT3*>(&vector))
			, XMLoadFloat4x4(std::forward<XMFLOAT4X4*>(&transform))));

		return result;
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& vector, XMFLOAT4X4&& transform)
	{
		return TransformCoord(XMFLOAT3(vector), std::forward<XMFLOAT4X4>(transform));
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3&& vector, const XMMATRIX& transform)
	{
		return TransformCoord(std::forward<XMFLOAT3>(vector), XMMATRIX(transform));
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& vector, const XMFLOAT4X4& transform)
	{
		return TransformCoord(XMFLOAT3(vector), XMFLOAT4X4(transform));
	}
}

namespace Vector4
{
	inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 result{};

		XMStoreFloat4(&result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));

		return result;
	}
}

namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 result{};

		XMStoreFloat4x4(&result, DirectX::XMMatrixIdentity());

		return result;
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4&& lhs, XMFLOAT4X4&& rhs)
	{
		XMFLOAT4X4 result{};
		
		const auto&& mat1 = XMLoadFloat4x4(std::forward<XMFLOAT4X4*>(&lhs));
		const auto&& mat2 = XMLoadFloat4x4(std::forward<XMFLOAT4X4*>(&rhs));

		XMStoreFloat4x4(&result, mat1 * mat2);

		return result;
	}

	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& lhs, const XMFLOAT4X4& rhs)
	{
		XMFLOAT4X4 result{};

		XMStoreFloat4x4(&result, XMLoadFloat4x4(&lhs) * XMLoadFloat4x4(&rhs));

		return result;
	}
	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& lhs, const XMMATRIX& rhs)
	{
		XMFLOAT4X4 result{};

		XMStoreFloat4x4(&result, XMLoadFloat4x4(&lhs) * rhs);

		return result;
	}

	inline XMFLOAT4X4 Multiply(const XMMATRIX& xmmtxMatrix1, const XMFLOAT4X4& rhs)
	{
		XMFLOAT4X4 result{};

		XMStoreFloat4x4(&result, xmmtxMatrix1 * XMLoadFloat4x4(&rhs));

		return result;
	}

	inline XMFLOAT4X4 Inverse(const XMFLOAT4X4&& matrix)
	{
		XMFLOAT4X4 result{};

		const auto&& mat = std::forward<XMMATRIX>(DirectX::XMMatrixInverse(nullptr, XMLoadFloat4x4(&matrix)));
		XMStoreFloat4x4(&result, mat);

		return result;
	}

	inline XMFLOAT4X4 Inverse(const XMFLOAT4X4& matrix)
	{
		return Inverse(XMFLOAT4X4(matrix));
	}

	inline XMFLOAT4X4 Transpose(const XMFLOAT4X4& matrix)
	{
		XMFLOAT4X4 result{};

		const auto mat = DirectX::XMMatrixTranspose(XMLoadFloat4x4(&matrix));
		XMStoreFloat4x4(&result, mat);

		return result;
	}

	inline XMFLOAT4X4 RotationYawPitchRoll(float pitch, float yaw, float roll)
	{
		XMFLOAT4X4 result{};

		const auto rad_pitch = XMConvertToRadians(pitch);
		const auto rad_yaw = XMConvertToRadians(yaw);
		const auto rad_roll = XMConvertToRadians(roll);
		const auto rot = XMMatrixRotationRollPitchYaw(rad_pitch, rad_yaw, rad_roll);
		XMStoreFloat4x4(&result, rot);

		return result;
	}

	inline XMFLOAT4X4 RotationAxis(const XMFLOAT3 axis, float angle)
	{
		XMFLOAT4X4 result{};

		const auto rot = XMMatrixRotationAxis(XMLoadFloat3(&axis), XMConvertToRadians(angle));
		XMStoreFloat4x4(&result, rot);

		return result;
	}

	inline XMFLOAT4X4 PerspectiveFovLH(float fov_y, float aspect, float znear, float zfar)
	{
		XMFLOAT4X4 result{};

		const auto mat = DirectX::XMMatrixPerspectiveFovLH(fov_y, aspect, znear, zfar);
		XMStoreFloat4x4(&result, mat);

		return result;
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3&& eye_pos
		, XMFLOAT3&& lookat
		, XMFLOAT3&& up)
	{
		XMFLOAT4X4 result{};

		const auto&& eye = std::forward<XMVECTOR>(XMLoadFloat3(&eye_pos));
		const auto&& eye_look = std::forward<XMVECTOR>(XMLoadFloat3(&lookat));
		const auto&& eye_up = std::forward<XMVECTOR>(XMLoadFloat3(&up));
		const auto mat = XMMatrixLookAtLH(eye, eye_look, eye_up);
		XMStoreFloat4x4(&result, mat);

		return result;
	}

	inline XMFLOAT4X4 LookAtLH(const XMFLOAT3& eye_pos
		, const XMFLOAT3& lookat
		, const XMFLOAT3& up)
	{
		return LookAtLH(XMFLOAT3(eye_pos), XMFLOAT3(lookat), XMFLOAT3(up));
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3&& eye_pos
		, const XMFLOAT3& lookat
		, const XMFLOAT3& up)
	{
		return LookAtLH(std::forward<XMFLOAT3>(eye_pos), XMFLOAT3(lookat), XMFLOAT3(up));
	}

	inline XMFLOAT4X4 LookToLH(const XMFLOAT3 eye_pos
		, const XMFLOAT3 look_pos
		, const XMFLOAT3 up_dir)
	{
		XMFLOAT4X4 result{};

		const auto eye = XMLoadFloat3(&eye_pos);
		const auto look = XMLoadFloat3(&look_pos);
		const auto up = XMLoadFloat3(&up_dir);
		XMStoreFloat4x4(&result, XMMatrixLookToLH(eye, look, up));

		return result;
	}
}

namespace Triangle
{
	inline bool Intersect(XMFLOAT3& xmf3RayPosition, XMFLOAT3& xmf3RayDirection, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2, float& dist)
	{
		return TriangleTests::Intersects(XMLoadFloat3(&xmf3RayPosition)
			, XMLoadFloat3(&xmf3RayDirection)
			, XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2)
			, dist);
	}
}

namespace Plane
{
	inline XMFLOAT4 Normalize(XMFLOAT4& xmf4Plane)
	{
		XMFLOAT4 result;
		XMStoreFloat4(&result, XMPlaneNormalize(XMLoadFloat4(&xmf4Plane)));
		return result;
	}
}
