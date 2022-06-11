#pragma once

class CPolygon
{
public:
	CPolygon();
	~CPolygon();

	void Reserve(const size_t count);
	void Add(const UINT index);
	template<std::size_t count>
	void Add(const UINT indices[count]);

	const UINT& At(const size_t index) const;
	UINT& At(const size_t index);

	void Render(P3DGrpCommandList cmdlist) const;

	const UINT* GetData() const;
	UINT* GetData();
	std::size_t GetSize() const;

private:
	std::vector<UINT> myIndices;
};
