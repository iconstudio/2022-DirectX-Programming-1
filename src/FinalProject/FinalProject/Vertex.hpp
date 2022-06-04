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

	XMFLOAT3 myPosition;
};

class DiffusedVertex : public Vertex
{
public:
	DiffusedVertex(COLORREF colour = 0);
	DiffusedVertex(float x, float y, float z, COLORREF colour);
	DiffusedVertex(float position[3], COLORREF colour);
	DiffusedVertex(const XMFLOAT3& position, COLORREF colour);
	DiffusedVertex(XMFLOAT3&& position, COLORREF colour);
	virtual ~DiffusedVertex();

	XMFLOAT3 myColour;
};

class LightenVertex : public Vertex
{
public:
	LightenVertex();
	LightenVertex(float x, float y, float z);
	LightenVertex(float position[3]);
	LightenVertex(const XMFLOAT3& position);
	LightenVertex(XMFLOAT3&& position);
	virtual ~LightenVertex();

	XMFLOAT3 myNormal;
};
