#include "pch.hpp"
#include "CubeMesh.hpp"

constexpr XMFLOAT3 CubeNormFront = { 0.0f, 0.0f, 1.0f };
constexpr XMFLOAT3 CubeNormTop = { 0.0f, 1.0f, 0.0f };
constexpr XMFLOAT3 CubeNormBack = { 0.0f, 0.0f, -1.0f };
constexpr XMFLOAT3 CubeNormBottom = { 0.0f, -1.0f, 0.0f };
constexpr XMFLOAT3 CubeNormLeft = { -1.0f, 0.0f, 0.0f };
constexpr XMFLOAT3 CubeNormRight = { 1.0f, 0.0f, 0.0f };

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

CubeMesh::CubeMesh(P3DDevice device, P3DGrpCommandList cmdlist, float w, float h, float d)
	: Mesh()
{
	numberMyVertex = 8;
	m_nType = VERTEXT_COLOR | VERTEXT_POSITION | VERTEXT_NORMAL;

	auto half_w = w * 0.5f;
	auto half_h = h * 0.5f;
	auto half_d = d * 0.5f;

	XMFLOAT3 my_cube[std::size(CubePositions)]{};
	CopyMemory(my_cube, CubePositions, sizeof(CubePositions));
	for (int i = 0; i < 8; i++)
	{
		my_cube[i].x *= half_w;
		my_cube[i].y *= half_h;
		my_cube[i].z *= half_d;
	}

	// 서술자 & 서술자 힙이 필요없다.
	m_pd3dPositionBuffer = CreateBufferResource(device, cmdlist
		, my_cube
		, sizeof(XMFLOAT3) * numberMyVertex
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * numberMyVertex;

	numberMyMesh = 6;
	if (numberMyMesh > 0)
	{
		m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [numberMyMesh];
		m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [numberMyMesh];
		m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[numberMyMesh];

		m_pnSubSetIndices = new int[numberMyMesh];

		for (int i = 0; i < numberMyMesh; i++)
		{
			m_pnSubSetIndices[i] = 4;

			m_ppd3dSubSetIndexBuffers[i] = CreateBufferResource(device, cmdlist
				, CubeIndices[i]
				, sizeof(UINT) * m_pnSubSetIndices[i]
				, D3D12_HEAP_TYPE_DEFAULT
				, D3D12_RESOURCE_STATE_INDEX_BUFFER
				, &m_ppd3dSubSetIndexUploadBuffers[i]);

			m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
			m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
			m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
		}
	}
}

void CubeMesh::ReleaseUploadBuffers()
{}

void CubeMesh::Render(P3DGrpCommandList cmdlist, int nSubSet)
{}
