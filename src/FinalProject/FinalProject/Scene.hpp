#pragma once
#include "WeakSingleton.hpp"
#include "DelegateObjet.hpp"
#include "PausableObjet.hpp"

class Scene
	: WeakSingleton<Framework> // �����ӿ�ũ ����
	, public DelegateObjet // ������ �̺�Ʈ ����
	, public PausableObjet // ���� �̺�Ʈ ����
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
