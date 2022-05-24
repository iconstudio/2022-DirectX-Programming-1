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

	// ������ �ڵ�
	HWND Window;

	// �����ӿ�ũ
	Framework& Framework;

	// ���� ��
	const size_t countEnemiesCube = 20, countEnemiesManta = 20;

	// �� ���� ������ �ʴ� �ν��Ͻ�
	std::vector<shared_ptr<GameObject>> staticInstances;
	// �� ���� ������ �ν��Ͻ�
	std::vector<shared_ptr<GameEntity>> myInstances;
	// �����ӿ�ũ���� ���� ī�޶�
	shared_ptr<Camera> myCamera;

	std::unordered_map<const char*, shared_ptr<Mesh>> myMeshes;

	// ������ ���� ��ȯ ���
	XMFLOAT4X4 globalMatrix;

	// ���� ���
	struct { float left; float right; float top; float bottom; } worldBoundary;

	// �÷��̾� ���� ��ġ
	//XMFLOAT3 playerSpawnPoint = XMFLOAT3{ WORLD_H * 0.5, 2.5f, 1.0f };
};

