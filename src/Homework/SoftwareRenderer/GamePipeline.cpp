#include "stdafx.hpp"
#include "GamePipeline.hpp"
#include "GameCamera.hpp"

XMFLOAT4X4 GamePipeline::matrixWorld = XMFLOAT4X4();
XMFLOAT4X4 GamePipeline::matrixProjection = XMFLOAT4X4();
XMFLOAT4X4 GamePipeline::matrixView = XMFLOAT4X4();
GameViewport GamePipeline::Viewport = GameViewport();

void GamePipeline::SetWorldTransform(const XMFLOAT4X4& pxmf4x4World)
{
	matrixWorld = pxmf4x4World;
	matrixView = Matrix4x4::Multiply(matrixWorld, matrixProjection);
}

void GamePipeline::SetProjection(const XMFLOAT4X4& projection)
{
	matrixProjection = projection;
}

void GamePipeline::SetViewport(const GameViewport& pViewport)
{
	Viewport = pViewport;
}

XMFLOAT3 GamePipeline::Transform(const XMFLOAT3& xmf3Model)
{
	auto xmf3Project = ProjectTransform(xmf3Model);
	auto f3Screen = ScreenTransform(xmf3Project);

	return (f3Screen);
}

XMFLOAT3 GamePipeline::ProjectTransform(const XMFLOAT3& xmf3Model)
{
	return Vector3::TransformCoord(xmf3Model, matrixView);
}

XMFLOAT3 GamePipeline::ScreenTransform(const XMFLOAT3& xmf3Project)
{
	XMFLOAT3 f3Screen = xmf3Project;

	float fHalfWidth = Viewport.m_nWidth * 0.5f;
	float fHalfHeight = Viewport.m_nHeight * 0.5f;
	float sx = Viewport.m_nLeft + (xmf3Project.x * fHalfWidth) + fHalfWidth;
	float sy = Viewport.m_nTop + (-xmf3Project.y * fHalfHeight) + fHalfHeight;

	return XMFLOAT3(sx, sy, 0.0f);
}
