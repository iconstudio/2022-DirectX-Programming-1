#pragma once

class CVertex
{
public:
	constexpr CVertex()
		: myPosition()
	{}

	constexpr CVertex(float x, float y, float z)
		: myPosition(x, y, z)
	{}

	constexpr CVertex(const float (&position)[3])
		: myPosition(position[0], position[1], position[2])
	{}

	constexpr CVertex(const XMFLOAT3& position)
		: myPosition(position)
	{}

	constexpr CVertex(XMFLOAT3&& position)
		: myPosition(std::forward<XMFLOAT3>(position))
	{}

	constexpr virtual ~CVertex()
	{}

	XMFLOAT3 myPosition;
};

class CDiffusedVertex : public CVertex
{
public:
	constexpr CDiffusedVertex(const CDiffusedVertex&) = default;
	constexpr CDiffusedVertex(CDiffusedVertex&&) = default;

	constexpr CDiffusedVertex(const CVertex& position)
		: CVertex(position)
		, myColour()
	{}

	constexpr CDiffusedVertex(CVertex&& position)
		: CVertex(std::forward<CVertex>(position))
		, myColour()
	{}

	constexpr CDiffusedVertex(const XMFLOAT4& colour)
		: CVertex()
		, myColour(colour)
	{}

	constexpr CDiffusedVertex(XMFLOAT4&& colour)
		: CVertex()
		, myColour(std::forward<XMFLOAT4>(colour))
	{}

	constexpr CDiffusedVertex(const CVertex& position, const XMFLOAT4& colour)
		: CVertex(position)
		, myColour(colour)
	{}

	constexpr CDiffusedVertex(CVertex&& position, const XMFLOAT4& colour)
		: CVertex(std::forward<CVertex>(position))
		, myColour(colour)
	{}

	constexpr CDiffusedVertex(const CVertex& position, XMFLOAT4&& colour)
		: CVertex(position)
		, myColour(std::forward<XMFLOAT4>(colour))
	{}

	constexpr CDiffusedVertex(CVertex&& position, XMFLOAT4&& colour)
		: CVertex(std::forward<CVertex>(position))
		, myColour(std::forward<XMFLOAT4>(colour))
	{}

	constexpr virtual ~CDiffusedVertex()
	{}

	XMFLOAT4 myColour;
};

class CLightenVertex : public CVertex
{
public:
	constexpr CLightenVertex(const CLightenVertex&) = default;
	constexpr CLightenVertex(CLightenVertex&&) = default;

	constexpr CLightenVertex(const CVertex& position)
		: CVertex(position)
		, myNormal()
	{}

	constexpr CLightenVertex(CVertex&& position)
		: CVertex(std::forward<CVertex>(position))
		, myNormal()
	{}

	constexpr CLightenVertex(const CVertex& position, const XMFLOAT3& normal)
		: CVertex(position)
		, myNormal(normal)
	{}

	constexpr CLightenVertex(CVertex&& position, const XMFLOAT3& normal)
		: CVertex(std::forward<CVertex>(position))
		, myNormal(normal)
	{}

	constexpr CLightenVertex(const CVertex& position, XMFLOAT3&& normal)
		: CVertex(position)
		, myNormal(std::forward<XMFLOAT3>(normal))
	{}

	constexpr CLightenVertex(CVertex&& position, XMFLOAT3&& normal)
		: CVertex(std::forward<CVertex>(position))
		, myNormal(std::forward<XMFLOAT3>(normal))
	{}

	constexpr virtual ~CLightenVertex()
	{}

	XMFLOAT3 myNormal;
};
