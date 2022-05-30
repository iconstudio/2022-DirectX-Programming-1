#include "pch.hpp"
#include "StageCredit.hpp"
#include "GameFramework.h"

StageCredit::StageCredit(GameFramework& framework, HWND hwnd)
	: Scene(framework, hwnd, "Credit")
{
	constexpr float colors[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	SetBackgroundColor(colors);
}

void StageCredit::Awake(P3DDevice device, P3DGrpCommandList cmd_list)
{
	Scene::Awake(device, cmd_list);
}

void StageCredit::Start()
{
	Scene::Start();
}

void StageCredit::Reset()
{
	Scene::Reset();
}

void StageCredit::Update(float elapsed_time)
{
	Scene::Update(elapsed_time);
}

void StageCredit::Render()
{
	Scene::Render();
}

void StageCredit::OnAwake()
{}

void StageCredit::OnInialized()
{}

void StageCredit::OnUpdate()
{}

void StageCredit::OnRender()
{}

void StageCredit::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageCredit::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
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

void StageCredit::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
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
