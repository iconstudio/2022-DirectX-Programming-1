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

	virtual void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	virtual void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
	virtual void OnWindow(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override;

	std::vector<shared_ptr<GameObject>> myInstances;

private:

};
