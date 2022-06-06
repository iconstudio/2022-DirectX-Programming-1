#pragma once

class Scene
{
public:
	Scene(GameFramework& framework, HWND hwnd, const char* name);
	virtual ~Scene();

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) = 0;
	virtual void Start() = 0;
	virtual void Reset() = 0;
	virtual void Update(float elapsed_time) = 0;
	virtual void Render() = 0;

	virtual void OnAwake() = 0;
	virtual void OnInialized() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;

	virtual void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) = 0;
	virtual void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) = 0;
	virtual void OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) = 0;

	void SetCamera(GameCamera* cam);
	void SetRootSignature(P3DSignature signature);
	void SetBackgroundColor(const float colors[4]);

	template<typename ObjectType, typename GType = std::remove_cvref<ObjectType>>
		requires(std::is_base_of_v<GType, GameObject>)
	constexpr GType* CreateInstance(float x, float y, float z);

	template<typename ObjectType, typename GType = std::remove_cvref<ObjectType>>
		requires(std::is_base_of_v<GType, GameObject>)
	constexpr GType* CreateInstance(float pos[3]);

	template<typename ObjectType, typename GType = std::remove_cvref<ObjectType>>
		requires(std::is_base_of_v<GType, GameObject>)
	constexpr GType* CreateInstance(const XMFLOAT3& position);

	template<typename ObjectType, typename GType = std::remove_cvref<ObjectType>>
		requires(std::is_base_of_v<GType, GameObject>)
	constexpr GType* CreateInstance(XMFLOAT3&& position);

	GameObject* AddInstance(const shared_ptr<GameObject>& inst);
	GameObject* AddInstance(shared_ptr<GameObject>&& inst);

	const std::string& GetName() const noexcept;
	shared_ptr<GameObject> GetInstance(const UINT index) const;

	GameCamera* GetCamera() noexcept;
	const GameCamera* GetCamera() const noexcept;
	P3DSignature GetRootSignature();
	P3DSignature const GetRootSignature() const;

	float myBackgroundColor[4];

protected:
	virtual P3DSignature CreateGraphicsRootSignature();
	virtual void ReleaseUploadBuffers();

	virtual void InitializeUniforms();
	virtual void UpdateUniforms();
	virtual void ReleaseUniforms();

	const std::string myName;

	HWND handleWindow;
	GameFramework& myFramework;

	P3DDevice d3dDevice;
	P3DGrpCommandList d3dTaskList;

	P3DSignature mySignature;
	GameCamera* myCamera = nullptr;

	std::vector<shared_ptr<GameObject>> myInstances;

	POINT posCursor;
	float lastDeltaTime;
};

template<typename ObjectType, typename GType>
	requires(std::is_base_of_v<GType, GameObject>)
constexpr GType* Scene::CreateInstance(float x, float y, float z)
{
	return CreateInstance<ObjectType, GType>(XMFLOAT3(x, y, z));
}

template<typename ObjectType, typename GType>
	requires(std::is_base_of_v<GType, GameObject>)
constexpr GType* Scene::CreateInstance(float pos[3])
{
	return CreateInstance<ObjectType, GType>(XMFLOAT3(pos));
}

template<typename ObjectType, typename GType>
	requires(std::is_base_of_v<GType, GameObject>)
constexpr GType* Scene::CreateInstance(const XMFLOAT3& position)
{
	return CreateInstance<ObjectType, GType>(XMFLOAT3(position));
}

template<typename ObjectType, typename GType>
	requires(std::is_base_of_v<GType, GameObject>)
constexpr GType* Scene::CreateInstance(XMFLOAT3&& position)
{
	auto handle = new GType();
	handle->SetPosition(std::forward<XMFLOAT3>(position));

	AddInstance(shared_ptr<GameObject>(handle));

	return handle;
}
