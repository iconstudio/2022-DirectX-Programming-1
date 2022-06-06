#pragma once
#include "Mesh.h"

class CubeMesh : public Mesh
{
	CubeMesh(P3DDevice device, P3DGrpCommandList cmdlist, float w, float h, float d);

	virtual void ReleaseUploadBuffers();

	virtual void Render(P3DGrpCommandList cmdlist, int nSubSet);

protected:
	ID3D12Resource* m_pd3dPositionBuffer = NULL;
	ID3D12Resource* m_pd3dPositionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	int								numberMyMesh = 0;
	int* m_pnSubSetIndices = NULL;

	ID3D12Resource** m_ppd3dSubSetIndexBuffers = NULL;
	ID3D12Resource** m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW* m_pd3dSubSetIndexBufferViews = NULL;
};
