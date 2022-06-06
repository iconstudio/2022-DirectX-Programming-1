#include "pch.hpp"
#include "Framework.hpp"
#include "Scene.hpp"
#include "GraphicsCore.hpp"

Framework::Framework(GraphicsCore& renderer, long width, long height)
	: WeakSingleton(renderer)
	, myWindow(NULL), myClientInstance(NULL)
	, frameWidth(width), frameHeight(height)
	, myScenes(), myStages(), myStageIterator(), currentScene()
{
	myScenes.reserve(10);
	myStages.reserve(10);
}

Framework& Framework::SetHWND(HWND window)
{
	myWindow = window;
	return *this;
}

Framework& Framework::SetHInstance(HINSTANCE instance)
{
	myClientInstance = instance;
	return *this;
}

void Framework::Awake()
{
	for (auto& scene : myScenes)
	{
		scene.second->Awake();
	}
}

void Framework::Start()
{
	JumpToStage(0);
}

void Framework::Reset()
{
	if (currentScene)
	{
		currentScene->Reset();
	}
}

void Framework::Update(float delta_time)
{
	if (currentScene)
	{
		currentScene->Update(delta_time);
	}
}

void Framework::PrepareRendering()
{
	if (currentScene)
	{
		currentScene->PrepareRendering(mySystem.GetCommandList());
	}
}

void Framework::Render()
{
	if (currentScene)
	{
		currentScene->Render(mySystem.GetCommandList());
	}
}

void Framework::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{
	if (currentScene)
	{
		currentScene->OnMouse(hwnd, msg, btn, info);
	}
}

void Framework::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
	if (currentScene)
	{
		currentScene->OnKeyboard(hwnd, msg, key, state);
	}

	switch (msg)
	{
		case WM_KEYUP:
		{
			switch (key)
			{
				case VK_ESCAPE:
				{
					mySystem.WaitForGpuComplete();

					PostQuitMessage(0);
				}
				break;

				case VK_RETURN:
				{}
				break;

				case VK_F9:
				{
					mySystem.WaitForGpuComplete();

					mySystem.ToggleFullscreen();
				}
				break;

				case VK_F5:
				{}
				break;

				default:
				{}
				break;
			}
		}
		break;

		default:
		{}
		break;
	}
}

void Framework::OnWindow(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (currentScene)
	{
		currentScene->OnWindow(hwnd, msg, wp, lp);
	}

	switch (msg)
	{
		case WM_SIZE:
		{}
		break;
	}
}

void Framework::AddStage(const shared_ptr<Scene>& stage)
{
	myStages.push_back(stage);
}

bool Framework::JumpToStage(const size_t index)
{
	if (0 <= index && index < myStages.size())
	{
		myStageIterator = myStages.begin() + index;

		const auto& target = GetStage(index);
		if (currentScene)
		{
			currentScene->Reset();
		}

		currentScene = target.lock();
		currentScene->Start();

		return true;
	}

	return false;
}

bool Framework::JumpToStage(const std::vector<shared_ptr<Scene>>::iterator it)
{
	myStageIterator = it;

	if (currentScene)
	{
		currentScene->Reset();
	}
	currentScene = *myStageIterator;
	currentScene->Start();

	return nullptr != currentScene;
}

bool Framework::JumpToNextStage()
{
	if (myStages.end() != myStageIterator)
	{
		return JumpToStage(myStageIterator + 1);
	}

	return false;
}

weak_ptr<Scene> Framework::GetScene(const char* name) const
{
	return myScenes.find(name)->second;
}

weak_ptr<Scene> Framework::GetStage(const size_t index) const
{
	return myStages.at(index);
}

weak_ptr<Scene> Framework::GetNextStage() const
{
	return *(myStageIterator + 1);
}

weak_ptr<Scene> Framework::GetCurrentScene() const
{
	return currentScene;
}

