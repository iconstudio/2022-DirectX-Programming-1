#include "pch.hpp"
#include "StageMain.hpp"

StageMain::StageMain(GameFramework& framework, HWND hwnd)
	: Scene(framework, hwnd, "Main")
{}

void StageMain::Awake(P3DDevice device, P3DGrpCommandList cmd_list)
{
	Scene::Awake(device, cmd_list);
}

void StageMain::Start()
{
	Scene::Start();
}

void StageMain::Reset()
{
	Scene::Reset();
}

void StageMain::Update(float elapsed_time)
{
	Scene::Update(elapsed_time);
}

void StageMain::Render()
{
	Scene::Render();
}

void StageMain::RenderUI(HDC surface)
{
	Scene::RenderUI(surface);
}

void StageMain::OnAwake()
{}

void StageMain::OnInialized()
{}

void StageMain::OnUpdate()
{}

void StageMain::OnRender()
{}

void StageMain::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageMain::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
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

void StageMain::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{}
