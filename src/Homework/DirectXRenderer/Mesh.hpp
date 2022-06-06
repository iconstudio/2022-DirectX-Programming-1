#pragma once

class CMesh
{
public:
	CMesh();
	CMesh(const size_t number_polygons);
	~CMesh();

	void Set(const size_t index, const CPolygon& poly);
	void Set(const size_t index, CPolygon&& poly);
	void Push(const CPolygon& poly);
	void Push(CPolygon&& poly);

	std::size_t GetPolygonsNumber() const;
	const BoundingOrientedBox& GetCollider() const;
	BoundingOrientedBox& GetCollider();

	void PrepareRendering(GameScene& scene, COLORREF color) const;
	void PrepareRenderingUnchecked(GameScene& scene, COLORREF color) const;
	void Render(HDC surface) const;
	void RenderByFragments(HDC surface) const;

	friend GameEntity;

protected:
	void BuildLocalFragment(const CPolygon& poly);
	void BuildFragments();
	void TryAddFragment(const size_t vertex_id);

	BOOL RayIntersectionByTriangle(XMVECTOR& ray, XMVECTOR& ray_dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* out_distance) const;
	int Raycast(XMVECTOR& ray, XMVECTOR& ray_dir, float* out_distance) const;

	BoundingOrientedBox staticCollider;

	std::vector<CPolygon> myPolygons;

	// 색인된 인덱스 목록
	std::vector<XMFLOAT3> localIndex;
	size_t lastFound;
	std::unordered_map<XMFLOAT3, size_t> Dictionary;

	// 조각에 추가할 인덱스 큐
	std::queue<size_t> localIndexed;
	std::vector<CLocalFragment> localFragments;

	// 렌더링 용 조각
	std::vector<CFragment> myFragments;
};

class CPolygon
{
public:
	CPolygon();
	CPolygon(const size_t number_vertices);
	virtual ~CPolygon();

	void Set(const UINT index, const CVertex& vertex);
	void Set(const UINT index, CVertex&& vertex);
	void Push(const CVertex& vertex);
	void Push(CVertex&& vertex);

	std::vector<CVertex> Vertices;
};
