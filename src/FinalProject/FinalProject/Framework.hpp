#pragma once
#include "WeakSingleton.hpp"
#include "DelegateObjet.hpp"
#include "PausableObjet.hpp"

class Framework
	: WeakSingleton<GraphicsCore> // DIrectX 렌더러 참조
	, public DelegateObjet // 윈도우 이벤트 실행
	, public PausableObjet // 게임 이벤트 실행
{
public:
	Framework(GraphicsCore& renderer, long width, long height);

	Framework& SetHWND(HWND window);
	Framework& SetHInstance(HINSTANCE instance);

	void Awake() final override;
	void Start() final override;
	void Reset() final override;
	void Update(float delta_time) final override;

	void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) final override;
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) final override;
	void OnWindow(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) final override;

private:
	HWND myWindow;
	HINSTANCE myClientInstance;
	const long frameWidth, frameHeight;

	std::unordered_map<std::string, shared_ptr<Scene>> myScenes;
	std::vector<shared_ptr<Scene>> myStages;
};
