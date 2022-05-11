#include "stdafx.hpp"
#include "GamePipeline.hpp"
#include "GameViewport.hpp"

XMFLOAT4X4 GamePipeline::matrixWorld = XMFLOAT4X4();
XMFLOAT4X4 GamePipeline::matrixProjection = XMFLOAT4X4();
XMFLOAT4X4 GamePipeline::matrixView = XMFLOAT4X4();
GameViewport GamePipeline::Viewport = GameViewport();

void GamePipeline::SetWorldTransform(const XMFLOAT4X4& world)
{
	matrixWorld = world;
	matrixView = Matrix4x4::Multiply(matrixWorld, matrixProjection);
}

void GamePipeline::SetProjection(const XMFLOAT4X4& projection)
{
	matrixProjection = projection;
}

void GamePipeline::SetViewport(const GameViewport& viewport)
{
	Viewport = viewport;
}

XMFLOAT3 GamePipeline::Transform(const XMFLOAT3& pos_model)
{
	auto xmf3Project = WorldProjectTransform(pos_model);
	auto f3Screen = ScreenTransform(xmf3Project);

	return (f3Screen);
}

XMFLOAT3 GamePipeline::WorldProjectTransform(const XMFLOAT3& pos_model)
{
	return Vector3::TransformCoord(pos_model, matrixView);
}

XMFLOAT3 GamePipeline::ProjectTransform(const XMFLOAT3& pos_model)
{
	return Vector3::TransformCoord(pos_model, matrixProjection);
}

XMFLOAT3 GamePipeline::ScreenTransform(const XMFLOAT3& pos_project)
{
	XMFLOAT3 f3Screen = pos_project;

	float fHalfWidth = Viewport.m_nWidth * 0.5f;
	float fHalfHeight = Viewport.m_nHeight * 0.5f;
	float sx = Viewport.m_nLeft + (pos_project.x * fHalfWidth) + fHalfWidth;
	float sy = Viewport.m_nTop + (-pos_project.y * fHalfHeight) + fHalfHeight;

	return XMFLOAT3(sx, sy, 0.0f);
}
