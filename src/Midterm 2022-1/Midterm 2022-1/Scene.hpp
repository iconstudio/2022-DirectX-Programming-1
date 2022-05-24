#pragma once

class Scene
{
public:
	Scene(Framework& framework);
	~Scene();

	void SetHwnd(HWND hwnd);
	void SetCamera(const shared_ptr<Camera>& cam);

	virtual void Awake() = 0;
	virtual void Start() = 0;
	virtual void Update(float elapsed_time) = 0;
	virtual void PrepareRendering() = 0;
	virtual void Render() = 0;

	virtual void OnMouse(UINT msg, WPARAM wp, LPARAM lp);
	virtual void OnKeyboard(UINT msg, WPARAM wp, LPARAM lp);
	virtual void OnHWND(UINT msg, WPARAM wp, LPARAM lp);

private:
	template<class Type>
	Type* CreateInstance();

	template<class Type>
	Type* CreateInstance(float x, float y, float z);

	template<class Type>
	Type* CreateInstance(const XMFLOAT3& position);

	template<class Type>
	Type* CreateInstance(XMFLOAT3&& position);

	// 윈도우 핸들
	HWND Window;

	// 프레임워크
	Framework& Framework;

	// 적의 수
	const size_t countEnemiesCube = 20, countEnemiesManta = 20;

	// 씬 내의 변하지 않는 인스턴스
	std::vector<shared_ptr<GameObject>> staticInstances;
	// 씬 내의 동적인 인스턴스
	std::vector<shared_ptr<GameEntity>> myInstances;
	// 프레임워크에서 받은 카메라
	shared_ptr<Camera> myCamera;

	std::unordered_map<const char*, shared_ptr<Mesh>> myMeshes;

	// 월드의 월드 변환 행렬
	XMFLOAT4X4 globalMatrix;

	// 월드 경계
	struct { float left; float right; float top; float bottom; } worldBoundary;

	// 플레이어 생성 위치
	//XMFLOAT3 playerSpawnPoint = XMFLOAT3{ WORLD_H * 0.5, 2.5f, 1.0f };
};

