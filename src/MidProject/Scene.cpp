#include "pch.hpp"
#include "Scene.h"
#include "Shader.h"
#include "Player.h"
#include "Light.hpp"

Scene::Scene(GameFramework& framework, HWND hwnd, const char* name)
	: myName(name)
	, myFramework(framework), handleWindow(hwnd)
	, d3dDevice(nullptr), d3dTaskList(nullptr)
	, mySignature(nullptr)
	, myBackgroundColor()
	, posCursor{ 0, 0 }
	, lastDeltaTime(0.0f)
{}

Scene::~Scene()
{
	ReleaseUniforms();
}

void Scene::Awake(P3DDevice device, P3DGrpCommandList cmd_list)
{
	d3dDevice = device;
	d3dTaskList = cmd_list;

	//mySignature = CreateGraphicsRootSignature();

	OnAwake();

	InitializeUniforms();
}

void Scene::Start()
{}

void Scene::Reset()
{}

void Scene::Update(float elapsed_time)
{
	lastDeltaTime = elapsed_time;


	for (auto& instance : myInstances)
	{
		instance->ReleaseUploadBuffers();
	}

	OnUpdate();
}

void Scene::Render()
{
	OnRender();
}

void Scene::SetCamera(GameCamera* cam)
{
	myCamera = cam;
}

void Scene::SetRootSignature(P3DSignature signature)
{
	if (mySignature)
	{
		mySignature->Release();
	}

	mySignature = signature;
}

void Scene::SetBackgroundColor(const float colors[4])
{
	myBackgroundColor[0] = colors[0];
	myBackgroundColor[1] = colors[1];
	myBackgroundColor[2] = colors[2];
	myBackgroundColor[3] = colors[3];
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

const std::string& Scene::GetName() const noexcept
{
	return myName;
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

P3DSignature Scene::GetRootSignature()
{
	return mySignature;
}

P3DSignature const Scene::GetRootSignature() const
{
	return mySignature;
}

P3DSignature Scene::CreateGraphicsRootSignature()
{
	return nullptr;
}

void Scene::InitializeUniforms()
{}

void Scene::UpdateUniforms()
{}

void Scene::ReleaseUniforms()
{}

void Scene::ReleaseUploadBuffers()
{
	for (auto& instance : myInstances)
	{
		instance->ReleaseUploadBuffers();
	}
}
