#include "stdafx.h"
#include "StageCredit.hpp"

StageCredit::StageCredit(GameFramework& framework, HWND hwnd)
	: Scene(framework, hwnd, "Credit")
{}

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

void StageCredit::RenderUI(HDC surface)
{
	Scene::RenderUI(surface);
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
{}
