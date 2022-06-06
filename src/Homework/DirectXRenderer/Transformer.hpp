#pragma once

class Transformer
{
public:
	static void SetWorldTransform(const XMFLOAT4X4& world);
	static void SetProjection(const XMFLOAT4X4& projection);
	static void SetViewport(const GameViewport& viewport);

	static XMFLOAT3 ScreenTransform(const XMFLOAT3& pos_project);
	static XMFLOAT3 ProjectTransform(const XMFLOAT3& pos_model);
	static XMFLOAT3 WorldProjectTransform(const XMFLOAT3& pos_model);
	static XMFLOAT3 Transform(const XMFLOAT3& pos_model);

private:
	static XMFLOAT4X4 matrixWorld;
	static XMFLOAT4X4 matrixProjection;
	static XMFLOAT4X4 matrixView;
	static GameViewport Viewport;
};

inline void DrawSide(HDC surface, const XMFLOAT3& pos1, const XMFLOAT3& pos2)
{
	auto from = Transformer::ScreenTransform(pos1);
	auto to = Transformer::ScreenTransform(pos2);
	DrawLine(surface, int(from.x), int(from.y), int(to.x), int(to.y));
}

inline void DrawSceenSide(HDC surface, const XMFLOAT3& from, const XMFLOAT3& to)
{
	DrawLine(surface, int(from.x), int(from.y), int(to.x), int(to.y));
}
