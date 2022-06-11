#include "pch.hpp"
#include "Scene.h"
#include "Pipeline.hpp"
#include "Player.h"
#include "Light.hpp"

Scene::Scene(GameFramework& framework, const char* name)
	: BasicScene(framework, name)
{}

Scene::~Scene()
{}

void Scene::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	d3dDevice = device;
	d3dTaskList = cmdlist;
}

void Scene::Start()
{
	for (auto& instance : myInstances)
	{
		instance->ReleaseUploadBuffers();
	}
}

void Scene::Reset()
{}

void Scene::Update(float delta_time)
{
	lastDeltaTime = delta_time;

	for (auto& instance : myInstances)
	{
		instance->Animate(delta_time);
	}
}

void Scene::SetCamera(GameCamera* cam)
{
	myCamera = cam;
}

GameObject* Scene::AddInstance(const shared_ptr<GameObject>& inst)
{
	myInstances.push_back(inst);
	return inst.get();
}

GameObject* Scene::AddInstance(shared_ptr<GameObject>&& inst)
{
	auto ptr = std::forward<GameObject*>(inst.get());
	myInstances.push_back(std::forward<shared_ptr<GameObject>>(inst));
	return ptr;
}

shared_ptr<GameObject> Scene::GetInstance(const UINT index) const
{
	return myInstances.at(index);
}

GameCamera* Scene::GetCamera() noexcept
{
	return myCamera;
}

const GameCamera* Scene::GetCamera() const noexcept
{
	return myCamera;
}
