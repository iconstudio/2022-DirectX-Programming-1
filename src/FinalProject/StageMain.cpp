#include "pch.hpp"
#include "StageMain.hpp"
#include "GameFramework.h"
#include "Transformer.hpp"

Transformer titleTransform{};

StageMain::StageMain(GameFramework& framework)
	: Scene(framework, "Main")
	, textTitle("HelliAttack", 24, Colors::CadetBlue)
	, textAuthor("JinYoonSeong", 24, Colors::CadetBlue)
	, textDate("2022-06-13", 24, Colors::CadetBlue)
{
	constexpr float colors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetBackgroundColor(colors);
}

void StageMain::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	Scene::Awake(device, cmdlist);

	textTitle.Start(device, cmdlist);
	textAuthor.Start(device, cmdlist);
	textDate.Start(device, cmdlist);
}

void StageMain::Start()
{
	Scene::Start();
}

void StageMain::Reset()
{
	Scene::Reset();
}

void StageMain::Update(float delta_time)
{
	Scene::Update(delta_time);
}

void StageMain::PrepareRendering()
{
	auto& pipeline = Pipeline::plainShader;
	//auto& pipeline = Pipeline::diffusedShader;
	if (!pipeline)
	{
		throw "평면 색상 쉐이더가 존재하지 않음!";
	}

	pipeline->PrepareRendering(d3dTaskList);
	d3dTaskList->SetGraphicsRootSignature(pipeline->GetRootSignature());

	//XMFLOAT4X4 xmf4x4World{};

	//const auto& road_mat = titleTransform.GetMatrix();
	//const auto my_mat = XMLoadFloat4x4(&road_mat);
	//XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(my_mat));

	// 두번째 루트 매개인자에서 0번째 메모리에 float 16개 전달
	//d3dTaskList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

	constexpr auto frame_width = float(FRAME_BUFFER_WIDTH);
	constexpr auto frame_height = float(FRAME_BUFFER_HEIGHT);

	d3dTaskList->SetGraphicsRoot32BitConstants(0, 1, &frame_width, 0);
	d3dTaskList->SetGraphicsRoot32BitConstants(0, 1, &frame_height, 1);

	D3D12_VIEWPORT viewport{};
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 0.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = frame_width;
	viewport.Height = frame_height;

	D3D12_RECT scissor_rect{};
	scissor_rect.left = 0;
	scissor_rect.top = 0;
	scissor_rect.right = FRAME_BUFFER_WIDTH;
	scissor_rect.bottom = FRAME_BUFFER_HEIGHT;

	d3dTaskList->RSSetViewports(1, &viewport);
	d3dTaskList->RSSetScissorRects(1, &scissor_rect);
	textTitle.myMerger->PrepareRendering(d3dTaskList);
}

void StageMain::Render() const
{
	textTitle.myMerger->Render(d3dTaskList);
}

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
