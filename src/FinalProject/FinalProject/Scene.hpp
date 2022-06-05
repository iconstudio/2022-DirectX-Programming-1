#pragma once
#include "WeakSingleton.hpp"
#include "DelegateObjet.hpp"
#include "PausableObjet.hpp"

class Scene
	: WeakSingleton<Framework> // 프레임워크 참조
	, public DelegateObjet // 윈도우 이벤트 실행
	, public PausableObjet // 게임 이벤트 실행
{
public:
	Scene(Framework& framework, const char* name);
	virtual ~Scene();

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Update(float delta_time) override;
	virtual	void PrepareRendering(P3DGrpCommandList cmdlist) const;
	virtual void Render(P3DGrpCommandList cmdlist) const;
	virtual void Release();

	void SetCamera(const shared_ptr<GameCamera>& cam);

	void AddInstance(const shared_ptr<GameObject>& ptr);
	void AddInstance(shared_ptr<GameObject>&& ptr);

	template<typename ObjectType>
		requires(std::is_base_of_v<GameEntity, ObjectType>)
	ObjectType* CreateInstance(float x, float y, float z);

	template<typename ObjectType>
		requires(std::is_base_of_v<GameEntity, ObjectType>)
	ObjectType* CreateInstance(float pos[3]);

	template<typename ObjectType>
		requires(std::is_base_of_v<GameEntity, ObjectType>)
	ObjectType* CreateInstance(const XMFLOAT3& position);

	template<typename ObjectType>
		requires(std::is_base_of_v<GameEntity, ObjectType>)
	ObjectType* CreateInstance(XMFLOAT3&& position);

	const std::string& GetName() const;

	virtual void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	virtual void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
	virtual void OnWindow(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override;

	const std::string myName;

	std::vector<shared_ptr<GameObject>> myInstances;
	shared_ptr<GameCamera> myCamera;

private:

};

template<typename ObjectType>
	requires(std::is_base_of_v<GameEntity, ObjectType>)
inline ObjectType* Scene::CreateInstance(float x, float y, float z)
{
	ObjectType* handle = new ObjectType();
	handle->SetPosition(x, y, z);

	AddInstance(shared_ptr<GameObject>(handle));

	return handle;
}

template<typename ObjectType>
	requires(std::is_base_of_v<GameEntity, ObjectType>)
inline ObjectType* Scene::CreateInstance(float pos[3])
{
	ObjectType* handle = new ObjectType();
	handle->SetPosition(pos);

	AddInstance(shared_ptr<GameObject>(handle));

	return handle;
}

template<typename ObjectType>
	requires(std::is_base_of_v<GameEntity, ObjectType>)
inline ObjectType* Scene::CreateInstance(const XMFLOAT3& position)
{
	ObjectType* handle = new ObjectType();
	handle->SetPosition(position);

	AddInstance(shared_ptr<GameObject>(handle));

	return handle;
}

template<typename ObjectType>
	requires(std::is_base_of_v<GameEntity, ObjectType>)
inline ObjectType* Scene::CreateInstance(XMFLOAT3&& position)
{
	ObjectType* handle = new ObjectType();
	handle->SetPosition(std::forward<XMFLOAT3>(position));

	AddInstance(shared_ptr<GameObject>(handle));

	return handle;
}
