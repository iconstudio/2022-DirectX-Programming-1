#pragma once

class Transformer
{
public:
	Transformer();
	~Transformer();

	void SetMatrix(const XMFLOAT4X4& mat);
	void SetMatrix(XMFLOAT4X4&& mat);
	void SetScale(float x, float y, float z);
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& pos);
	void SetPosition(XMFLOAT3&& pos);
	void SetRotation(const XMFLOAT4X4& tfrm);
	void SetRotation(XMFLOAT4X4&& tfrm);

	void Translate(float x, float y, float z);
	void Translate(const XMFLOAT3& shift);
	void Translate(XMFLOAT3&& shift);
	void Move(const XMFLOAT3& dir, float distance);
	void Move(XMFLOAT3&& dir, float distance);
	void MoveStrafe(float distance);
	void MoveForward(float distance);
	void MoveUp(float distance);

	void Rotate(const XMFLOAT4X4& tfrm);
	void Rotate(XMFLOAT4X4&& tfrm);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(const XMFLOAT3& axis, float angle);
	void Rotate(const XMFLOAT4& quaternion);
	void Rotate(XMFLOAT4&& quaternion);

	void LookTo(const XMFLOAT3& look, const XMFLOAT3& up);
	void LookAt(const XMFLOAT3& look, const XMFLOAT3& up);

	XMFLOAT4X4& GetMatrix();
	const XMFLOAT4X4& GetMatrix() const;
	XYZWrapper& GetRight();
	XYZWrapper& GetUp();
	XYZWrapper& GetLook();
	XYZWrapper& GetPosition();
	const XYZWrapper& GetRight() const;
	const XYZWrapper& GetUp() const;
	const XYZWrapper& GetLook() const;
	const XYZWrapper& GetPosition() const;

	XMFLOAT4X4 myMatrix;
	XYZWrapper myRight;
	XYZWrapper myUp;
	XYZWrapper myLook;
	XYZWrapper myPosition;

	static constexpr XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	static constexpr XMFLOAT3 Forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	static constexpr XMFLOAT3 Right = XMFLOAT3(1.0f, 0.0f, 0.0f);

private:
	bool Updated;
};


