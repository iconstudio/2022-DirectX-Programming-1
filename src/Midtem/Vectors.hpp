#pragma once

namespace Vector3
{
	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR&& vector)
	{
		XMFLOAT3 result{};

		XMStoreFloat3(&result, std::forward<XMVECTOR>(vector));

		return result;
	}

	inline XMFLOAT3 XMVectorToFloat3(const XMVECTOR& vector)
	{
		return XMVectorToFloat3(XMVECTOR(vector));
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
		return ScalarProduct(std::move(XMFLOAT3(vector)), scalar, normalize);
	}

	inline XMFLOAT3 Add(XMFLOAT3&& lhs, XMFLOAT3&& rhs)
	{
		XMFLOAT3 result{};

		const auto xmvector1 = XMLoadFloat3(std::forward<XMFLOAT3*>(&lhs));
		const auto xmvector2 = XMLoadFloat3(std::forward<XMFLOAT3*>(&rhs));
		XMStoreFloat3(&result, xmvector1 + xmvector2);

		return result;
	}

	inline XMFLOAT3 Add(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
	{
		return Add(std::move(XMFLOAT3(lhs)), std::move(XMFLOAT3(lhs)));
	}

	inline XMFLOAT3 Add(const XMFLOAT3& lhs, const XMFLOAT3& rhs, float scalar)
	{
		XMFLOAT3 result;
		XMStoreFloat3(&result, XMLoadFloat3(&lhs) + (XMLoadFloat3(&rhs) * scalar));
		return result;
	}

	inline XMFLOAT3 Subtract(XMFLOAT3&& lhs, XMFLOAT3&& rhs)
	{
		XMFLOAT3 result{};

		const auto xmvector1 = XMLoadFloat3(std::forward<XMFLOAT3*>(&lhs));
		const auto xmvector2 = XMLoadFloat3(std::forward<XMFLOAT3*>(&rhs));
		XMStoreFloat3(&result, xmvector1 - xmvector2);

		return result;
	}

	inline XMFLOAT3 Subtract(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
	{
		return Subtract(XMFLOAT3(lhs), XMFLOAT3(rhs));
	}

	inline float DotProduct(XMFLOAT3&& lhs, XMFLOAT3&& rhs)
	{
		XMFLOAT3 result{};

		const auto xmvector1 = XMLoadFloat3(std::forward<XMFLOAT3*>(&lhs));
		const auto xmvector2 = XMLoadFloat3(std::forward<XMFLOAT3*>(&rhs));
		XMStoreFloat3(&result, XMVector3Dot(xmvector1, xmvector2));

		return result.x;
	}

	inline float DotProduct(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
	{
		return DotProduct(XMFLOAT3(lhs), XMFLOAT3(rhs));
	}

	inline XMFLOAT3 CrossProduct(const XMFLOAT3& lhs, const XMFLOAT3& rhs, bool normalize = true)
	{
		XMFLOAT3 result;
		if (normalize)
			XMStoreFloat3(&result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&lhs), XMLoadFloat3(&rhs))));
		else
			XMStoreFloat3(&result, XMVector3Cross(XMLoadFloat3(&lhs), XMLoadFloat3(&rhs)));
		return result;
	}

	inline XMFLOAT3 Normalize(XMFLOAT3&& vector)
	{
		XMFLOAT3 m_xmf3Normal{};

		const auto mid = XMLoadFloat3(std::forward<XMFLOAT3*>(&vector));
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(mid));

		return(m_xmf3Normal);
	}

	inline XMFLOAT3 Normalize(const XMFLOAT3& vector)
	{
		return Normalize(std::move(XMFLOAT3(vector)));
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

	inline float Angle(XMVECTOR&& lhs, XMVECTOR&& rhs)
	{
		const auto&& pos1 = std::forward<XMVECTOR>(lhs);
		const auto&& pos2 = std::forward<XMVECTOR>(rhs);

		return XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(pos1, pos2)));
	}

	inline float Angle(const XMVECTOR& lhs, const XMVECTOR& rhs)
	{
		return Angle(XMVECTOR(lhs), XMVECTOR(rhs));
	}

	inline float Angle(const XMVECTOR& lhs, XMVECTOR&& rhs)
	{
		return Angle(XMVECTOR(lhs), std::forward<XMVECTOR>(rhs));
	}

	inline float Angle(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
	{
		return Angle(XMLoadFloat3(&lhs), XMLoadFloat3(&rhs));
	}

	inline float Angle(XMFLOAT3&& lhs, XMFLOAT3&& rhs)
	{
		const auto&& vec1 = std::forward<XMFLOAT3>(lhs);
		const auto&& vec2 = std::forward<XMFLOAT3>(rhs);

		return Angle(XMLoadFloat3(&vec1), XMLoadFloat3(&vec2));
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
		return TransformNormal(std::move(XMFLOAT3(vector)), std::move(XMMATRIX(transform)));
	}

	inline XMFLOAT3 TransformNormal(const XMFLOAT3& vector, XMMATRIX&& transform)
	{
		return TransformNormal(std::move(XMFLOAT3(vector)), std::forward<XMMATRIX>(transform));
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3&& vector, XMMATRIX&& transform)
	{
		XMFLOAT3 result{};

		const auto&& mid = std::forward<XMFLOAT3*>(&vector);
		XMStoreFloat3(&result
			, XMVector3TransformCoord(XMLoadFloat3(mid)
			, transform));

		return result;
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& vector, const XMMATRIX& transform)
	{
		return TransformCoord(std::move(XMFLOAT3(vector)), std::move(XMMATRIX(transform)));
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& vector, XMMATRIX&& transform)
	{
		return TransformCoord(std::move(XMFLOAT3(vector)), std::forward<XMMATRIX>(transform));
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& vector, const XMFLOAT4X4& matrix)
	{
		return TransformCoord(vector, XMLoadFloat4x4(&matrix));
	}
}

namespace Vector4
{
	inline XMFLOAT4 Add(XMFLOAT4& vector1, XMFLOAT4& vector2)
	{
		XMFLOAT4 result;
		XMStoreFloat4(&result, XMLoadFloat4(&vector1) + XMLoadFloat4(&vector2));
		
		return result;
	}
}

namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, XMMatrixIdentity());
		return result;
	}

	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& matrix1, const XMFLOAT4X4& matrix2)
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, XMLoadFloat4x4(&matrix1) * XMLoadFloat4x4(&matrix2));
		return result;
	}

	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& matrix1, const XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, XMLoadFloat4x4(&matrix1) * xmmtxMatrix2);
		return result;
	}

	inline XMFLOAT4X4 Multiply(const XMMATRIX& xmmtxMatrix1, const XMFLOAT4X4& matrix2)
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, xmmtxMatrix1 * XMLoadFloat4x4(&matrix2));
		return result;
	}

	inline XMFLOAT4X4 Inverse(const XMFLOAT4X4& matrix)
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, XMMatrixInverse(NULL, XMLoadFloat4x4(&matrix)));
		return result;
	}

	inline XMFLOAT4X4 Transpose(const XMFLOAT4X4& matrix)
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, XMMatrixTranspose(XMLoadFloat4x4(&matrix)));
		return result;
	}

	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
		return result;
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& eye
		, XMFLOAT3& look
		, XMFLOAT3&& up)
	{
		XMFLOAT4X4 result{};

		XMStoreFloat4x4(&result, XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&look), XMLoadFloat3(&up)));
		return result;
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& eye
		, XMFLOAT3& look
		, const XMFLOAT3& up)
	{
		return LookAtLH(eye, look, std::move(XMFLOAT3(up)));
	}
}

namespace Triangle
{
	inline bool Intersect(XMFLOAT3& ray_pos, XMFLOAT3& ray_dir, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2, float& dist)
	{
		return(TriangleTests::Intersects(XMLoadFloat3(&ray_pos), XMLoadFloat3(&ray_dir), XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), dist));
	}
}

namespace Plane
{
	inline XMFLOAT4 Normalize(XMFLOAT4& plane)
	{
		XMFLOAT4 result;
		XMStoreFloat4(&result, XMPlaneNormalize(XMLoadFloat4(&plane)));

		return result;
	}
}
