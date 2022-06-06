#include "pch.hpp"
#include "Vertex.hpp"

Vertex::Vertex()
	: myPosition()
{}

Vertex::Vertex(float x, float y, float z)
	: myPosition(x, y, z)
{}

Vertex::Vertex(float position[3])
	: myPosition(position[0], position[1], position[2])
{}

Vertex::Vertex(const XMFLOAT3& position)
	: myPosition(position.x, position.y, position.z)
{}

Vertex::Vertex(XMFLOAT3&& position)
	: myPosition(std::forward<float>(position.x)
	, std::forward<float>(position.y)
	, std::forward<float>(position.z))
{}

Vertex::~Vertex()
{}

DiffusedVertex::DiffusedVertex(XMFLOAT4 colour)
	: Vertex(), myColour(colour)
{}

DiffusedVertex::DiffusedVertex(float x, float y, float z, XMFLOAT4 colour)
	: Vertex(x, y, z), myColour(colour)
{}

DiffusedVertex::DiffusedVertex(float position[3], XMFLOAT4 colour)
	: Vertex(position), myColour(colour)
{}

DiffusedVertex::DiffusedVertex(const XMFLOAT3& position, XMFLOAT4 colour)
	: Vertex(position), myColour(colour)
{}

DiffusedVertex::DiffusedVertex(XMFLOAT3&& position, XMFLOAT4 colour)
	: Vertex(std::forward<XMFLOAT3>(position)), myColour(colour)
{}

DiffusedVertex::~DiffusedVertex()
{}

LightenVertex::LightenVertex(const XMFLOAT3 normal)
	: Vertex(), myNormal(normal)
{}

LightenVertex::LightenVertex(float x, float y, float z, const XMFLOAT3 normal)
	: Vertex(x, y, z), myNormal(normal)
{}

LightenVertex::LightenVertex(float position[3], const XMFLOAT3 normal)
	: Vertex(position), myNormal(normal)
{}

LightenVertex::LightenVertex(const XMFLOAT3& position, const XMFLOAT3 normal)
	: Vertex(position), myNormal(normal)
{}

LightenVertex::LightenVertex(XMFLOAT3&& position, const XMFLOAT3 normal)
	: Vertex(std::forward<XMFLOAT3>(position)), myNormal(normal)
{}

LightenVertex::~LightenVertex()
{}
