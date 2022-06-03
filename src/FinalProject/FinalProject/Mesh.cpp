#include "pch.hpp"
#include "Mesh.hpp"

void Mesh::Render(P3DGrpCommandList cmd_list, int nSubSet)
{
	cmd_list->IASetPrimitiveTopology(typePrimitive);

	// ��� ������ �ѱ��.
	cmd_list->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);
	
	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		// ���� �޽��� ���ε� ������ �ѱ��.
		cmd_list->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		// ���� �޽��� �׸���. 
		cmd_list->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1
			, 0 // ���ε� ����
			, 0 // ���� ��ġ ������
			, 0); // �ν��Ͻ� ������ (ID)
	}
	else
	{
		// �׳� ���� ���ۿ� �ִ� ������ ���� �׸���.
		cmd_list->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}
