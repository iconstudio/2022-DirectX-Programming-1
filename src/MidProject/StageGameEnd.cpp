#include "pch.hpp"
#include "StageGameEnd.hpp"
#include "GameFramework.h"

StageGameEnd::StageGameEnd(GameFramework& framework)
	: Scene(framework, "Complete")
{
	constexpr float colors[] = { 0.0f, 0.5f, 0.5f, 1.0f };
	SetBackgroundColor(colors);
}

void StageGameEnd::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	Scene::Awake(device, cmdlist);
}

void StageGameEnd::Start()
{
	Scene::Start();
}

void StageGameEnd::Reset()
{
	Scene::Reset();
}

void StageGameEnd::Update(float delta_time)
{
	Scene::Update(delta_time);
}

void StageGameEnd::PrepareRendering()
{}

void StageGameEnd::Render()
{}

void StageGameEnd::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageGameEnd::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		{
		}
		break;

		case WM_RBUTTONDOWN:
		{
		}
		break;

		case WM_LBUTTONUP:
		{
		}
		break;

		case WM_RBUTTONUP:
		{
		}
		break;

		case WM_MOUSEMOVE:
		{
		}
		break;

		default:
		{}
		break;
	}
}

void StageGameEnd::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			switch (key)
			{
				case VK_SPACE:
				case VK_RETURN:
				{
					myFramework.JumpToStage(0);
				}
				break;
			}
		}
		break;
	}
}
