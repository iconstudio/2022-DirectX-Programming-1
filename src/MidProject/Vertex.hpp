#pragma once

class CVertex
{
public:
	constexpr CVertex();
	constexpr CVertex(float x, float y, float z);
	constexpr CVertex(float position[3]);
	constexpr CVertex(const XMFLOAT3& position);
	constexpr CVertex(XMFLOAT3&& position);
	virtual ~CVertex();

	XMFLOAT3 myPosition;
};

class CDiffusedVertex : public CVertex
{
public:
	constexpr CDiffusedVertex(const CDiffusedVertex&) = default;
	constexpr CDiffusedVertex(CDiffusedVertex&&) = default;
	constexpr CDiffusedVertex(const CVertex& position);
	constexpr CDiffusedVertex(CVertex&& position);
	constexpr CDiffusedVertex(const XMFLOAT4& colour);
	constexpr CDiffusedVertex(XMFLOAT4&& colour);
	constexpr CDiffusedVertex(const CVertex& position, const XMFLOAT4& colour);
	constexpr CDiffusedVertex(CVertex&& position, const XMFLOAT4& colour);
	constexpr CDiffusedVertex(const CVertex& position, XMFLOAT4&& colour);
	constexpr CDiffusedVertex(CVertex&& position, XMFLOAT4&& colour);
	virtual ~CDiffusedVertex();

	XMFLOAT4 myColour;
};

class CLightenVertex : public CVertex
{
public:
	constexpr CLightenVertex(const CVertex& position);
	constexpr CLightenVertex(CVertex&& position);
	constexpr CLightenVertex(const CVertex& position, const XMFLOAT3& normal);
	constexpr CLightenVertex(CVertex&& position, const XMFLOAT3& normal);
	constexpr CLightenVertex(const CVertex& position, XMFLOAT3&& normal);
	constexpr CLightenVertex(CVertex&& position, XMFLOAT3&& normal);
	virtual ~CLightenVertex();

	XMFLOAT3 myNormal;
};

constexpr CVertex::CVertex()
	: myPosition()
{}

constexpr CVertex::CVertex(float x, float y, float z)
	: myPosition(x, y, z)
{}

constexpr CVertex::CVertex(float position[3])
	: myPosition(position)
{}

constexpr CVertex::CVertex(const XMFLOAT3& position)
	: myPosition(position)
{}

constexpr CVertex::CVertex(XMFLOAT3&& position)
	: myPosition(std::forward<XMFLOAT3>(position))
{}

CVertex::~CVertex()
{}

constexpr CDiffusedVertex::CDiffusedVertex(const CVertex& position)
	: CVertex(position)
	, myColour()
{}

constexpr CDiffusedVertex::CDiffusedVertex(CVertex&& position)
	: CVertex(std::forward<CVertex>(position))
	, myColour()
{}

constexpr CDiffusedVertex::CDiffusedVertex(const XMFLOAT4& colour)
	: CVertex()
	, myColour(colour)
{}

constexpr CDiffusedVertex::CDiffusedVertex(XMFLOAT4&& colour)
	: CVertex()
	, myColour(std::forward<XMFLOAT4>(colour))
{}

constexpr CDiffusedVertex::CDiffusedVertex(const CVertex& position, const XMFLOAT4& colour)
	: CVertex(position)
	, myColour(colour)
{}

constexpr CDiffusedVertex::CDiffusedVertex(CVertex&& position, const XMFLOAT4& colour)
	: CVertex(std::forward<CVertex>(position))
	, myColour(colour)
{}

constexpr CDiffusedVertex::CDiffusedVertex(const CVertex& position, XMFLOAT4&& colour)
	: CVertex(position)
	, myColour(std::forward<XMFLOAT4>(colour))
{}

constexpr CDiffusedVertex::CDiffusedVertex(CVertex&& position, XMFLOAT4&& colour)
	: CVertex(std::forward<CVertex>(position))
	, myColour(std::forward<XMFLOAT4>(colour))
{}

CDiffusedVertex::~CDiffusedVertex()
{}

constexpr CLightenVertex::CLightenVertex(const CVertex& position)
	: CVertex(position)
	, myNormal()

{}

constexpr CLightenVertex::CLightenVertex(CVertex&& position)
	: CVertex(std::forward<CVertex>(position))
	, myNormal()
{}

constexpr CLightenVertex::CLightenVertex(const CVertex& position, const XMFLOAT3& normal)
	: CVertex(position)
	, myNormal(normal)
{}

constexpr CLightenVertex::CLightenVertex(CVertex&& position, const XMFLOAT3& normal)
	: CVertex(std::forward<CVertex>(position))
	, myNormal(normal)
{}

constexpr CLightenVertex::CLightenVertex(const CVertex& position, XMFLOAT3&& normal)
	: CVertex(position)
	, myNormal(std::forward<XMFLOAT3>(normal))
{}

constexpr CLightenVertex::CLightenVertex(CVertex&& position, XMFLOAT3&& normal)
	: CVertex(std::forward<CVertex>(position))
	, myNormal(std::forward<XMFLOAT3>(normal))
{}

CLightenVertex::~CLightenVertex()
{}
