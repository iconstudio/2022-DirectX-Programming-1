#pragma once

class Vertex
{
public:
	Vertex();
	Vertex(float x, float y, float z);
	Vertex(float position[3]);
	Vertex(const XMFLOAT3& position);
	Vertex(XMFLOAT3&& position);
	virtual ~Vertex();

	const XMFLOAT3 myPosition;
};

class DiffusedVertex : public Vertex
{
public:
	DiffusedVertex(XMFLOAT4 colour);
	DiffusedVertex(float x, float y, float z, XMFLOAT4 colour);
	DiffusedVertex(float position[3], XMFLOAT4 colour);
	DiffusedVertex(const XMFLOAT3& position, XMFLOAT4 colour);
	DiffusedVertex(XMFLOAT3&& position, XMFLOAT4 colour);
	virtual ~DiffusedVertex();

	const XMFLOAT4 myColour;
};

class LightenVertex : public Vertex
{
public:
	LightenVertex(const XMFLOAT3 normal);
	LightenVertex(float x, float y, float z, const XMFLOAT3 normal);
	LightenVertex(float position[3], const XMFLOAT3 normal);
	LightenVertex(const XMFLOAT3& position, const XMFLOAT3 normal);
	LightenVertex(XMFLOAT3&& position, const XMFLOAT3 normal);
	virtual ~LightenVertex();

	XMFLOAT3 myNormal;
};
