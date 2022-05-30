#include "pch.hpp"
#include "StageGameEnd.hpp"
#include "GameFramework.h"

StageGameEnd::StageGameEnd(GameFramework& framework, HWND hwnd)
	: Scene(framework, hwnd, "Complete")
{
	constexpr float colors[] = { 0.0f, 0.5f, 0.5f, 1.0f };
	SetBackgroundColor(colors);
}

void StageGameEnd::Awake(P3DDevice device, P3DGrpCommandList cmd_list)
{
	Scene::Awake(device, cmd_list);
}

void StageGameEnd::Start()
{
	Scene::Start();
}

void StageGameEnd::Reset()
{
	Scene::Reset();
}

void StageGameEnd::Update(float elapsed_time)
{
	Scene::Update(elapsed_time);
}

void StageGameEnd::Render()
{
	Scene::Render();
}


void StageGameEnd::OnAwake()
{}

void StageGameEnd::OnInialized()
{}

void StageGameEnd::OnUpdate()
{}

void StageGameEnd::OnRender()
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
					myFramework.JumpToNextStage();
				}
				break;
			}
		}
		break;
	}
}
