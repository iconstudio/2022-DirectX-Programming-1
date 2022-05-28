#pragma once

namespace Vector3
{
	inline XMFLOAT3 XMVectorToFloat3(const XMVECTOR& xmvVector)
	{
		return XMVectorToFloat3(XMVECTOR(xmvVector));
	}

	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR&& xmvVector)
	{
		XMFLOAT3 xmf3Result{};

		XMStoreFloat3(&xmf3Result, std::forward<XMVECTOR>(xmvVector));

		return xmf3Result;
	}

	inline XMFLOAT3 ScalarProduct(const XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		return ScalarProduct(XMFLOAT3(xmf3Vector), fScalar, bNormalize);
	}

	inline XMFLOAT3 ScalarProduct(XMFLOAT3&& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result{};

		const auto xmvector = XMLoadFloat3(std::forward<XMFLOAT3*>(&xmf3Vector));
		if (bNormalize)
		{
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(xmvector) * fScalar);
		}
		else 
		{
			XMStoreFloat3(&xmf3Result, xmvector * fScalar);
		}

		return xmf3Result;
	}

	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		return Add(XMFLOAT3(xmf3Vector1), XMFLOAT3(xmf3Vector1));
	}

	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, XMFLOAT3&& xmf3Vector2)
	{
		return Add(XMFLOAT3(xmf3Vector1), std::forward<XMFLOAT3>(xmf3Vector2));
	}

	inline XMFLOAT3 Add(XMFLOAT3&& xmf3Vector1, XMFLOAT3&& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result{};

		const auto xmvector1 = XMLoadFloat3(std::forward<XMFLOAT3*>(&xmf3Vector1));
		const auto xmvector2 = XMLoadFloat3(std::forward<XMFLOAT3*>(&xmf3Vector2));
		XMStoreFloat3(&xmf3Result, xmvector1 + xmvector2);

		return xmf3Result;
	}

	inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
		return xmf3Result;
	}

	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
		return xmf3Result;
	}

	inline float DotProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}

	inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return xmf3Result;
	}

	inline XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
	{
		return Normalize(xmf3Vector);
	}

	inline XMFLOAT3 Normalize(XMFLOAT3&& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal{};

		const auto mid = XMLoadFloat3(std::forward<XMFLOAT3*>(&xmf3Vector));
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(mid));

		return(m_xmf3Normal);
	}

	inline float Length(const XMFLOAT3& xmf3Vector)
	{
		return Length(XMFLOAT3(xmf3Vector));
	}

	inline float Length(XMFLOAT3&& xmf3Vector)
	{
		XMFLOAT3 xmf3Result{};

		const auto mid = XMLoadFloat3(std::forward<XMFLOAT3*>(&xmf3Vector));
		XMStoreFloat3(&xmf3Result, XMVector3Length(mid));

		return(xmf3Result.x);
	}

	inline float Angle(const XMVECTOR& xmvVector1, const XMVECTOR& xmvVector2)
	{
		return Angle(XMVECTOR(xmvVector1), XMVECTOR(xmvVector2));
	}

	inline float Angle(const XMVECTOR& xmvVector1, XMVECTOR&& xmvVector2)
	{
		return Angle(XMVECTOR(xmvVector1), std::forward<XMVECTOR>(xmvVector2));
	}

	inline float Angle(XMVECTOR&& xmvVector1, XMVECTOR&& xmvVector2)
	{
		const auto&& pos1 = std::forward<XMVECTOR>(xmvVector1);
		const auto&& pos2 = std::forward<XMVECTOR>(xmvVector2);

		return XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(pos1, pos2)));
	}

	inline float Angle(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		return Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2));
	}

	inline float Angle(XMFLOAT3&& xmf3Vector1, XMFLOAT3&& xmf3Vector2)
	{
		const auto&& vec1 = std::forward<XMFLOAT3>(xmf3Vector1);
		const auto&& vec2 = std::forward<XMFLOAT3>(xmf3Vector2);

		return Angle(XMLoadFloat3(&vec1), XMLoadFloat3(&vec2));
	}

	inline XMFLOAT3 TransformNormal(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
	{
		return TransformNormal(XMFLOAT3(xmf3Vector), XMMATRIX(xmmtxTransform));
	}

	inline XMFLOAT3 TransformNormal(const XMFLOAT3& xmf3Vector, XMMATRIX&& xmmtxTransform)
	{
		return TransformNormal(XMFLOAT3(xmf3Vector), std::forward<XMMATRIX>(xmmtxTransform));
	}

	inline XMFLOAT3 TransformNormal(XMFLOAT3&& xmf3Vector, XMMATRIX&& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result{};

		XMStoreFloat3(&xmf3Result
			, XMVector3TransformNormal(XMLoadFloat3(std::forward<XMFLOAT3*>(&xmf3Vector))
			, std::forward<XMMATRIX>(xmmtxTransform)));

		return xmf3Result;
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
	{
		return TransformCoord(XMFLOAT3(xmf3Vector), XMMATRIX(xmmtxTransform));
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, XMFLOAT4X4&& xmmtx4x4Matrix)
	{
		return TransformCoord(XMFLOAT3(xmf3Vector), std::forward<XMFLOAT4X4>(xmmtx4x4Matrix));
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3&& xmf3Vector, XMMATRIX&& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result{};

		XMStoreFloat3(&xmf3Result
			, XMVector3TransformCoord(XMLoadFloat3(std::forward<XMFLOAT3*>(&xmf3Vector))
			, xmmtxTransform));

		return xmf3Result;
	}
}

namespace Vector4
{
	inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}
}

namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}
}

namespace Triangle
{
	inline bool Intersect(XMFLOAT3& xmf3RayPosition, XMFLOAT3& xmf3RayDirection, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2, float& fHitDistance)
	{
		return(TriangleTests::Intersects(XMLoadFloat3(&xmf3RayPosition), XMLoadFloat3(&xmf3RayDirection), XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), fHitDistance));
	}
}

namespace Plane
{
	inline XMFLOAT4 Normalize(XMFLOAT4& xmf4Plane)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMPlaneNormalize(XMLoadFloat4(&xmf4Plane)));
		return(xmf4Result);
	}
}
