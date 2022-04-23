#include "stdafx.hpp"
#include "GamePipeline.hpp"
#include "GameCamera.hpp"

XMFLOAT4X4 GamePipeline::m_pxmf4x4World = XMFLOAT4X4();
XMFLOAT4X4 GamePipeline::m_pxmf4x4ViewProject = XMFLOAT4X4();
GameViewport GamePipeline::m_pViewport = GameViewport();

void GamePipeline::SetWorldTransform(const XMFLOAT4X4& pxmf4x4World)
{
	m_pxmf4x4World = pxmf4x4World;
}

void GamePipeline::SetViewPerspectiveProjectTransform(const XMFLOAT4X4& projection)
{
	m_pxmf4x4ViewProject = projection;
}

void GamePipeline::SetViewOrthographicProjectTransform(const XMFLOAT4X4& projection)
{
	m_pxmf4x4ViewProject = projection;
}

void GamePipeline::SetViewport(const GameViewport& pViewport)
{
	m_pViewport = pViewport;
}

XMFLOAT3 GamePipeline::Transform(const XMFLOAT3& xmf3Model)
{
	XMFLOAT3 xmf3Project = Project(xmf3Model);
	XMFLOAT3 f3Screen = ScreenTransform(xmf3Project);

	return (f3Screen);
}

XMFLOAT3 GamePipeline::Project(const XMFLOAT3& xmf3Model)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Multiply(m_pxmf4x4World, m_pxmf4x4ViewProject);
	XMFLOAT3 xmf3Project = Vector3::TransformCoord(xmf3Model, xmf4x4Transform);

	return (xmf3Project);
}

XMFLOAT3 GamePipeline::ScreenTransform(const XMFLOAT3& xmf3Project)
{
	XMFLOAT3 f3Screen = xmf3Project;

	float fHalfWidth = m_pViewport.m_nWidth * 0.5f;
	float fHalfHeight = m_pViewport.m_nHeight * 0.5f;
	f3Screen.x = m_pViewport.m_nLeft + (xmf3Project.x * fHalfWidth) + fHalfWidth;
	f3Screen.y = m_pViewport.m_nTop + (-xmf3Project.y * fHalfHeight) + fHalfHeight;

	return (f3Screen);
}
