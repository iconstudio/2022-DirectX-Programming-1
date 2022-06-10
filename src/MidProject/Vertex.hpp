#pragma once

class CVertex
{
public:
	CVertex();
	CVertex(float x, float y, float z);
	CVertex(float position[3]);
	CVertex(const XMFLOAT3& position);
	CVertex(XMFLOAT3&& position);
	virtual ~CVertex();

	const XMFLOAT3 myPosition;
};

class CDiffusedVertex : public CVertex
{
public:
	CDiffusedVertex(XMFLOAT4 colour);
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 colour);
	CDiffusedVertex(float position[3], XMFLOAT4 colour);
	CDiffusedVertex(const XMFLOAT3& position, XMFLOAT4 colour);
	CDiffusedVertex(XMFLOAT3&& position, XMFLOAT4 colour);
	virtual ~CDiffusedVertex();

	const XMFLOAT4 myColour;
};

class CLightenVertex : public CVertex
{
public:
	CLightenVertex(const XMFLOAT3 normal);
	CLightenVertex(float x, float y, float z, const XMFLOAT3 normal);
	CLightenVertex(float position[3], const XMFLOAT3 normal);
	CLightenVertex(const XMFLOAT3& position, const XMFLOAT3 normal);
	CLightenVertex(XMFLOAT3&& position, const XMFLOAT3 normal);
	virtual ~CLightenVertex();

	XMFLOAT3 myNormal;
};

