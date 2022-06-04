#include "pch.hpp"
#include "Scene.hpp"
#include "GameObject.hpp"
#include "GameCamera.hpp"

Scene::Scene(Framework& framework, const char* name)
	: WeakSingleton(framework)
	, myCamera(nullptr)
{}

Scene::~Scene()
{}

void Scene::Awake()
{
	for (auto& instance : myInstances)
	{
		instance->Awake();
	}

	myCamera->Awake();
}

void Scene::Start()
{
	myCamera->Start();
}

void Scene::Reset()
{
	myCamera->Reset();
}

void Scene::Update(float delta_time)
{
	for (auto& instance : myInstances)
	{
		instance->Update(delta_time);
	}

	myCamera->Update(delta_time);
}

void Scene::PrepareRendering(P3DGrpCommandList cmdlist)
{
	myCamera->PrepareRendering(cmdlist);
}

void Scene::Render(P3DGrpCommandList cmdlist)
{
	myCamera->Render(cmdlist);

	for (auto& instance : myInstances)
	{
		instance->Render(cmdlist);
	}
}

void Scene::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{}

void Scene::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{}

void Scene::OnWindow(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}
