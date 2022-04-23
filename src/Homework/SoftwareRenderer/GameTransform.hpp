#pragma once
#include "stdafx.hpp"

class GameTransform
{
public:
	GameTransform();

	XYZWrapper& GetRight();
	XYZWrapper& GetUp();
	XYZWrapper& GetLook();
	XYZWrapper& GetPosition();

	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& pos);
	void SetPosition(XMFLOAT3&& pos);
	void SetRotation(const XMFLOAT4X4& tfrm);

	void Translate(float x, float y, float z);
	void Translate(XMFLOAT3&& pos);
	void Move(const XMFLOAT3& dir, float distance);
	void Move(XMFLOAT3&& dir, float distance);
	void MoveStrafe(float distance);
	void MoveForward(float distance);
	void MoveUp(float distance);

	void Rotate(const XMFLOAT4X4& tfrm);
	void Rotate(float pitch = 10.0f, float yaw = 10.0f, float roll = 10.0f);
	void Rotate(XMFLOAT3& axis, float angle);

	void LookTo(XMFLOAT3& look, XMFLOAT3& up);
	void LookAt(XMFLOAT3& look, XMFLOAT3& up);

	XMFLOAT4X4 World;
	XYZWrapper myRight;
	XYZWrapper myUp;
	XYZWrapper myLook;
	XYZWrapper myPosition;

	float Speed;
	XMFLOAT3 Direction;

	static constexpr XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	static constexpr XMFLOAT3 Forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	static constexpr XMFLOAT3 Right = XMFLOAT3(1.0f, 0.0f, 0.0f);

private:
	bool Updated;
};


