#pragma once
#include "BasicScene.hpp"

class Scene : public BasicScene
{
public:
	Scene(GameFramework& framework, const char* name);
	virtual ~Scene();

	void SetCamera(GameCamera* cam);

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

	GameCamera* GetCamera() noexcept;
	const GameCamera* GetCamera() const noexcept;
	shared_ptr<GameObject> GetInstance(const UINT index) const;

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Update(float delta_time) override;

protected:
	GameCamera* myCamera = nullptr;

	std::vector<shared_ptr<GameObject>> myInstances;
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
