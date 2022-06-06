#include "pch.hpp"
#include "Transformer.hpp"
#include "GameViewport.hpp"
#include "Arithmetics.hpp"

XMFLOAT4X4 Transformer::matrixWorld = XMFLOAT4X4();
XMFLOAT4X4 Transformer::matrixProjection = XMFLOAT4X4();
XMFLOAT4X4 Transformer::matrixView = XMFLOAT4X4();
GameViewport Transformer::Viewport = GameViewport();

void Transformer::SetWorldTransform(const XMFLOAT4X4& world)
{
	matrixWorld = world;
	matrixView = Matrix4x4::Multiply(matrixWorld, matrixProjection);
}

void Transformer::SetProjection(const XMFLOAT4X4& projection)
{
	matrixProjection = projection;
}

void Transformer::SetViewport(const GameViewport& viewport)
{
	Viewport = viewport;
}

XMFLOAT3 Transformer::Transform(const XMFLOAT3& pos_model)
{
	auto xmf3Project = WorldProjectTransform(pos_model);
	auto f3Screen = ScreenTransform(xmf3Project);

	return (f3Screen);
}

XMFLOAT3 Transformer::WorldProjectTransform(const XMFLOAT3& pos_model)
{
	return Vector3::TransformCoord(pos_model, matrixView);
}

XMFLOAT3 Transformer::ProjectTransform(const XMFLOAT3& pos_model)
{
	return Vector3::TransformCoord(pos_model, matrixProjection);
}

XMFLOAT3 Transformer::ScreenTransform(const XMFLOAT3& pos_project)
{
	XMFLOAT3 f3Screen = pos_project;

	float fHalfWidth = Viewport.m_nWidth * 0.5f;
	float fHalfHeight = Viewport.m_nHeight * 0.5f;
	float sx = Viewport.m_nLeft + (pos_project.x * fHalfWidth) + fHalfWidth;
	float sy = Viewport.m_nTop + (-pos_project.y * fHalfHeight) + fHalfHeight;

	return XMFLOAT3(sx, sy, 0.0f);
}
