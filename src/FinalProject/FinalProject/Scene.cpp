#include "pch.hpp"
#include "Scene.hpp"
#include "GameObject.hpp"

Scene::Scene(Framework& framework, const char* name)
	: WeakSingleton(framework)
{

}

Scene::~Scene()
{}

void Scene::Awake()
{}

void Scene::Start()
{}

void Scene::Reset()
{}

void Scene::Update(float delta_time)
{}

void Scene::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{}

void Scene::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{}

void Scene::OnWindow(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}
