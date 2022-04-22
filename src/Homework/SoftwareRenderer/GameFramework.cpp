#include "stdafx.hpp"
#include "GameFramework.hpp"
#include "GameScene.hpp"
#include "GameCamera.hpp"

GameFramework::GameFramework()
{
}

GameFramework::~GameFramework()
{

}

void GameFramework::OnCreate(HINSTANCE instance, HWND hwnd)
{
	process = instance;
	window = hwnd;
}

void GameFramework::Start()
{
	myCamera = std::make_shared<GameCamera>();

	myScene = std::make_unique<GameScene>(WORLD_H, WORLD_V, WORLD_U);
	myScene->SetCamera(myCamera);

	myScene->Start();
}
