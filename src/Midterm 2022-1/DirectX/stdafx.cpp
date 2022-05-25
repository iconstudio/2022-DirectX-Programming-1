// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// LabProject03-1.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

UINT gnCbvSrvDescriptorIncrementSize = 32;

ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource **ppd3dUploadBuffer)
{
	ID3D12Resource *pd3dBuffer = NULL;

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = d3dHeapType;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, NULL, __uuidof(ID3D12Resource), (void **)&pd3dBuffer);

	if (pData)
	{
		switch (d3dHeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppd3dUploadBuffer)
			{
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void **)ppd3dUploadBuffer);

				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8 *pBufferDataBegin = NULL;
				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void **)&pBufferDataBegin);
				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppd3dUploadBuffer)->Unmap(0, NULL);

				pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);

				D3D12_RESOURCE_BARRIER d3dResourceBarrier;
				::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
				d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				d3dResourceBarrier.Transition.pResource = pd3dBuffer;
				d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
				d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8 *pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void **)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return(pd3dBuffer);
}

XMFLOAT3 Vector3::XMVectorToFloat3(const XMVECTOR& vector)
{
	return XMVectorToFloat3(std::move(XMVECTOR(vector)));
}

XMFLOAT3 Vector3::XMVectorToFloat3(XMVECTOR&& vector)
{
	XMFLOAT3 xmf3Result{};

	XMStoreFloat3(&xmf3Result, vector);

	return xmf3Result;
}

XMFLOAT3 Vector3::ScalarProduct(const XMFLOAT3& vector, float scalar, bool normalize)
{
	return ScalarProduct(std::move(XMFLOAT3(vector)), scalar, normalize);
}

XMFLOAT3 Vector3::ScalarProduct(XMFLOAT3&& vector, float scalar, bool normalize)
{
	auto temp_vector = XMLoadFloat3(&vector);

	XMFLOAT3 xmf3Result{};

	if (normalize)
		XMStoreFloat3(&xmf3Result, XMVector3Normalize(temp_vector) * scalar);
	else
		XMStoreFloat3(&xmf3Result, temp_vector * scalar);

	return xmf3Result;
}

XMFLOAT3 Vector3::Add(const XMFLOAT3& vector1, const XMFLOAT3& vector2, float scalar)
{
	return Add(std::move(XMFLOAT3(vector1)), std::move(XMFLOAT3(vector2)), scalar);
}

XMFLOAT3 Vector3::Add(XMFLOAT3&& vector1, XMFLOAT3&& vector2, float scalar)
{
	XMFLOAT3 xmf3Result{};

	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&vector1) + XMLoadFloat3(&vector2) * scalar);

	return xmf3Result;
}

XMFLOAT3 Vector3::Subtract(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
{
	return Subtract(std::move(XMFLOAT3(vector1)), std::move(XMFLOAT3(vector2)));
}

XMFLOAT3 Vector3::Subtract(XMFLOAT3&& vector1, XMFLOAT3&& vector2)
{
	XMFLOAT3 xmf3Result{};

	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&vector1) - XMLoadFloat3(&vector2));

	return xmf3Result;
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

XMFLOAT3 Vector3::CrossProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2, bool normalize)
{
	return CrossProduct(std::move(XMFLOAT3(vector1)), std::move(XMFLOAT3(vector2)), normalize);
}

XMFLOAT3 Vector3::CrossProduct(XMFLOAT3&& vector1, XMFLOAT3&& vector2, bool normalize)
{
	XMFLOAT3 xmf3Result{};

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

	return xmf3Result;
}

XMFLOAT3 Vector3::Normalize(const XMFLOAT3& vector)
{
	return Normalize(std::move(XMFLOAT3(vector)));
}

XMFLOAT3 Vector3::Normalize(XMFLOAT3&& vector)
{
	XMFLOAT3 m_xmf3Normal{};

	XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&vector)));

	return m_xmf3Normal;
}

FLOAT Vector3::Length(XMFLOAT3 vector)
{
	XMFLOAT3 xmf3Result{};

	XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&vector)));

	return (xmf3Result.x);
}

FLOAT Vector3::Distance(XMFLOAT3 vector1, XMFLOAT3 vector2)
{
	XMFLOAT3 xmf3Result{};

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

XMFLOAT3 Vector3::TransformNormal(const XMFLOAT3& vector, const XMMATRIX& transform)
{
	return TransformNormal(std::move(XMFLOAT3(vector)), std::move(XMMATRIX(transform)));
}

XMFLOAT3 Vector3::TransformNormal(XMFLOAT3&& vector, XMMATRIX&& transform)
{
	XMFLOAT3 xmf3Result{};

	XMStoreFloat3(&xmf3Result
		, XMVector3TransformNormal(XMLoadFloat3(&vector)
		, std::move(XMMATRIX(transform))));

	return xmf3Result;
}

XMFLOAT3 Vector3::TransformNormal(const XMFLOAT3& vector, const XMFLOAT4X4& transform)
{
	return TransformNormal(std::move(XMFLOAT3(vector)), std::move(XMFLOAT4X4(transform)));
}

XMFLOAT3 Vector3::TransformNormal(XMFLOAT3&& vector, XMFLOAT4X4&& transform)
{
	return TransformNormal(std::forward<XMFLOAT3>(vector)
		, XMLoadFloat4x4(&transform));
}

XMFLOAT3 Vector3::TransformCoord(const XMFLOAT3& vector, const XMMATRIX& transform)
{
	return TransformCoord(std::move(XMFLOAT3(vector))
		, std::move(XMMATRIX(transform)));
}

XMFLOAT3 Vector3::TransformCoord(XMFLOAT3&& vector, XMMATRIX&& transform)
{
	XMFLOAT3 xmf3Result{};

	XMStoreFloat3(&xmf3Result
		, XMVector3TransformCoord(XMLoadFloat3(&vector)
		, std::forward<XMMATRIX>(transform)));

	return xmf3Result;
}

XMFLOAT3 Vector3::TransformCoord(const XMFLOAT3& vector, const XMFLOAT4X4& transform)
{
	return TransformCoord(std::move(XMFLOAT3(vector)), std::move(XMFLOAT4X4(transform)));
}

XMFLOAT3 Vector3::TransformCoord(XMFLOAT3&& vector, XMFLOAT4X4&& transform)
{
	return TransformCoord(std::forward<XMFLOAT3>(vector), XMLoadFloat4x4(&transform));
}

XMFLOAT4 Vector4::Add(const XMFLOAT4& vector1, const XMFLOAT4& vector2)
{
	return Add(std::move(XMFLOAT4(vector1)), std::move(XMFLOAT4(vector2)));
}

XMFLOAT4 Vector4::Add(XMFLOAT4&& vector1, XMFLOAT4&& vector2)
{
	XMFLOAT4 xmf4Result{};

	XMStoreFloat4(&xmf4Result, XMLoadFloat4(&vector1) + XMLoadFloat4(&vector2));

	return xmf4Result;
}

XMFLOAT4X4 Matrix4x4::Identity()
{
	XMFLOAT4X4 xmmtx4x4Result{};

	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::Translate(float x, float y, float z)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranslation(x, y, z));

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::Inverse(const XMFLOAT4X4& matrix)
{
	return Inverse(std::move(XMFLOAT4X4(matrix)));
}

XMFLOAT4X4 Matrix4x4::Inverse(XMFLOAT4X4&& matrix)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&matrix)));

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::Transpose(const XMFLOAT4X4& matrix)
{
	return Transpose(std::move(XMFLOAT4X4(matrix)));
}

XMFLOAT4X4 Matrix4x4::Transpose(XMFLOAT4X4&& matrix)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&matrix)));

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::Multiply(const XMFLOAT4X4& matrix1, const XMFLOAT4X4& matrix2)
{
	return Multiply(std::move(XMFLOAT4X4(matrix1)), std::move(XMFLOAT4X4(matrix2)));
}

XMFLOAT4X4 Matrix4x4::Multiply(XMFLOAT4X4&& matrix1, XMFLOAT4X4&& matrix2)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&matrix1) * XMLoadFloat4x4(&matrix2));

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::Multiply(const XMFLOAT4X4& matrix1, const XMMATRIX& matrix2)
{
	return Multiply(std::move(XMFLOAT4X4(matrix1)), std::move(XMMATRIX(matrix2)));
}

XMFLOAT4X4 Matrix4x4::Multiply(XMFLOAT4X4&& matrix1, XMMATRIX&& matrix2)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&matrix1) * matrix2);

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::Multiply(const XMMATRIX& matrix1, const XMFLOAT4X4& matrix2)
{
	return Multiply(std::move(XMMATRIX(matrix1)), std::move(XMFLOAT4X4(matrix2)));
}

XMFLOAT4X4 Matrix4x4::Multiply(XMMATRIX&& matrix1, XMFLOAT4X4&& matrix2)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	XMStoreFloat4x4(&xmmtx4x4Result, matrix1 * XMLoadFloat4x4(&matrix2));

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::Multiply(const XMMATRIX& matrix1, const XMMATRIX& matrix2)
{
	return Multiply(std::move(XMMATRIX(matrix1)), std::move(XMMATRIX(matrix2)));
}

XMFLOAT4X4 Matrix4x4::Multiply(XMMATRIX&& matrix1, XMMATRIX&& matrix2)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	XMStoreFloat4x4(&xmmtx4x4Result, matrix1 * matrix2);

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::RotationYawPitchRoll(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	auto rad_pitch = XMConvertToRadians(fPitch);
	auto rad_yaw = XMConvertToRadians(fYaw);
	auto rad_roll = XMConvertToRadians(fRoll);
	auto rot = XMMatrixRotationRollPitchYaw(rad_pitch, rad_yaw, rad_roll);
	XMStoreFloat4x4(&xmmtx4x4Result, rot);

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::RotationAxis(XMFLOAT3 xmf3Axis, float fAngle)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixRotationAxis(XMLoadFloat3(&xmf3Axis), XMConvertToRadians(fAngle)));

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::PerspectiveFovLH(float fFovAngleY, float fAspectRatio, float fNearZ, float fFarZ)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(XMConvertToRadians(fFovAngleY), fAspectRatio, fNearZ, fFarZ));

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::LookAtLH(const XMFLOAT3 eye_pos, const XMFLOAT3 look_pos, const XMFLOAT3 up_dir)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	auto eye = XMLoadFloat3(&eye_pos);
	auto look = XMLoadFloat3(&look_pos);
	auto up = XMLoadFloat3(&up_dir);
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(eye, look, up));

	return xmmtx4x4Result;
}

XMFLOAT4X4 Matrix4x4::LookToLH(const XMFLOAT3 eye_pos, const XMFLOAT3 look_pos, const XMFLOAT3 up_dir)
{
	XMFLOAT4X4 xmmtx4x4Result{};

	auto eye = XMLoadFloat3(&eye_pos);
	auto look = XMLoadFloat3(&look_pos);
	auto up = XMLoadFloat3(&up_dir);
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookToLH(eye, look, up));

	return xmmtx4x4Result;
}

bool Triangle::Intersect(XMFLOAT3 ray_pos, XMFLOAT3 ray_dir, XMFLOAT3 v0, XMFLOAT3 v1, XMFLOAT3 v2, FLOAT& out_distance)
{
	auto ray = XMLoadFloat3(&ray_pos);
	auto dir = XMLoadFloat3(&ray_dir);
	auto p0 = XMLoadFloat3(&v0);
	auto p1 = XMLoadFloat3(&v1);
	auto p2 = XMLoadFloat3(&v2);

	return TriangleTests::Intersects(ray, dir, p0, p1, p2, out_distance);
}

XMFLOAT4 Plane::Normalize(const XMFLOAT4& xmf4Plane)
{
	return Normalize(std::move(XMFLOAT4(xmf4Plane)));
}

XMFLOAT4 Plane::Normalize(XMFLOAT4&& xmf4Plane)
{
	XMFLOAT4 xmf4Result{};

	XMStoreFloat4(&xmf4Result, XMPlaneNormalize(XMLoadFloat4(&xmf4Plane)));

	return xmf4Result;
}
