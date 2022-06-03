#include "pch.hpp"
#include "Mesh.hpp"

void Mesh::Render(P3DGrpCommandList cmd_list, int nSubSet)
{
	cmd_list->IASetPrimitiveTopology(typePrimitive);

	// 모든 정점을 넘긴다.
	cmd_list->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);
	
	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		// 하위 메쉬의 색인된 정점을 넘긴다.
		cmd_list->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		// 하위 메쉬를 그린다. 
		cmd_list->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1
			, 0 // 색인된 정점
			, 0 // 정점 위치 시작점
			, 0); // 인스턴스 시작점 (ID)
	}
	else
	{
		// 그냥 정점 버퍼에 있는 정점을 따라 그린다.
		cmd_list->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}
