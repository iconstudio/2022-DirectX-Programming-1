#include "pch.hpp"
#include "Scene.hpp"
#include "GameObject.hpp"
#include "GameCamera.hpp"

Scene::Scene(Framework& framework, const char* name)
	: WeakSingleton(framework)
	, myName(name)
	, myCamera(nullptr)
	, posCursor(), lastDeltaTime(0.0f)
{}

Scene::~Scene()
{}

void Scene::Awake()
{
	for (auto& instance : myInstances)
	{
		instance->Awake();
	}

	if (myCamera)
	{
		myCamera->Awake();
	}
}

void Scene::Start()
{
	if (myCamera)
	{
		myCamera->Start();
	}
	else
	{
		throw "카메라가 없음!";
	}
}

void Scene::Reset()
{
	if (myCamera)
	{
		myCamera->Reset();
	}
}

void Scene::Update(float delta_time)
{
	lastDeltaTime = delta_time;

	for (auto& instance : myInstances)
	{
		instance->Update(delta_time);
	}

	if (myCamera)
	{
		myCamera->Update(delta_time);
	}
}

void Scene::PrepareRendering(P3DGrpCommandList cmdlist) const
{
	if (myCamera)
	{
		myCamera->PrepareRendering(cmdlist);
	}
	else
	{
		throw "카메라가 없음!";
	}
}

void Scene::Render(P3DGrpCommandList cmdlist) const
{
	if (myCamera)
	{
		myCamera->Render(cmdlist);

		for (auto& instance : myInstances)
		{
			instance->Render(cmdlist);
		}
	}
}

void Scene::Release()
{
	if (myCamera)
	{
		myCamera->Release();
	}
}

void Scene::SetCamera(const shared_ptr<GameCamera>& cam)
{
	myCamera = cam;
}

void Scene::AddInstance(const shared_ptr<GameObject>& ptr)
{
	myInstances.push_back(ptr);
}

void Scene::AddInstance(shared_ptr<GameObject>&& ptr)
{
	myInstances.push_back(std::forward<shared_ptr<GameObject>>(ptr));
}

const std::string& Scene::GetName() const
{
	return myName;
}

void Scene::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		{}
		break;

		case WM_LBUTTONUP:
		{}
		break;

		default:
		{}
		break;
	}
}

void Scene::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{}
		break;

		case WM_KEYUP:
		{}
		break;

		default:
		{}
		break;
	}
}

void Scene::OnWindow(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		default:
		{}
		break;
	}
}
