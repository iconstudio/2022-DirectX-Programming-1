#include "pch.hpp"
#include "CubeMesh.hpp"

constexpr XMFLOAT3 CubePositions[] = {
	{-1.0f, -1.0f, -1.0f}, // 0
	{-1.0f, -1.0f, +1.0f}, // 1
	{+1.0f, -1.0f, -1.0f}, // 2
	{+1.0f, -1.0f, +1.0f}, // 3

	{-1.0f, +1.0f, -1.0f}, // 4
	{-1.0f, +1.0f, +1.0f}, // 5
	{+1.0f, +1.0f, -1.0f}, // 6
	{+1.0f, +1.0f, +1.0f}, // 7
};

constexpr UINT CubeFaceFront[] = { 4, 6, 2, 0 };
constexpr UINT CubeFaceTop[] = { 5, 7, 6, 4 };
constexpr UINT CubeFaceBack[] = { 1, 3, 7, 5 };
constexpr UINT CubeFaceBottom[] = { 0, 2, 3, 1 };
constexpr UINT CubeFaceLeft[] = { 5, 4, 0, 1 };
constexpr UINT CubeFaceRIght[] = { 6, 7, 3, 2 };

constexpr UINT const* CubeIndices[] = {
	CubeFaceFront, CubeFaceBack,
	CubeFaceTop,CubeFaceBottom,
	CubeFaceLeft, CubeFaceRIght
};

CubeMesh::CubeMesh()
	: Mesh()
{}

CubeMesh::~CubeMesh()
{}

void CubeMesh::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	numberVertices = 8;
	m_nType = VERTEXT_COLOR | VERTEXT_POSITION | VERTEXT_NORMAL;

	const auto peek = reinterpret_cast<const void*>(CubePositions);
	// sizeof(XMFLOAT3) * numberVertices
	constexpr auto sz = sizeof(CubePositions);
	mPositionBuffer = CreateBufferResource(device, cmdlist
		, peek, sz
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &myPositionUploadBuffer);

	myPositionBufferView.BufferLocation = mPositionBuffer->GetGPUVirtualAddress();
	myPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	myPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * numberVertices;

	numberSubMeshes = 6;

	// 렌더링 용 업로드 버퍼
	myIndexUploadBuffers = new ID3D12Resource * [numberSubMeshes];

	myIndexBuffers = new ID3D12Resource * [numberSubMeshes];
	myIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[numberSubMeshes];

	numberSubMeshesIndexes = new int[numberSubMeshes];

	for (int i = 0; i < numberSubMeshes; i++)
	{
		constexpr auto submeshes_count = 4;
		numberSubMeshesIndexes[i] = submeshes_count;

		auto& buffer = myIndexBuffers[i];
		auto& buffer_view = myIndexBufferViews[i];

		const auto& face_indices = CubeIndices[i];
		const auto& face_indices_count = numberSubMeshesIndexes[i]; // 4
		const auto face_indices_sz = sizeof(UINT) * submeshes_count;

		myIndexBuffers[i] = CreateBufferResource(device, cmdlist
			, face_indices, face_indices_sz
			, D3D12_HEAP_TYPE_DEFAULT
			, D3D12_RESOURCE_STATE_INDEX_BUFFER
			, &myIndexUploadBuffers[i]);

		buffer_view.BufferLocation = buffer->GetGPUVirtualAddress();
		buffer_view.Format = DXGI_FORMAT_R32_UINT;
		buffer_view.SizeInBytes = face_indices_sz;
	}
}
