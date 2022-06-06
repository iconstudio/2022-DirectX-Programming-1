#include "pch.hpp"
#include "stdafx.h"
#include "FinalProject.h"
#include "Timer.hpp"
#include "Framework.hpp"
#include "GraphicsCore.hpp"
#include "GraphicsPipeline.hpp"
#include "Scene.hpp"
#include "IlluminatedScene.hpp"
#include "Material.hpp"
#include "GameCamera.hpp"
#include "CubeMesh.hpp"
#include "GameEntity.hpp"
#include "Model.hpp"
#include "Shader.hpp"

constexpr int MAX_LOADSTRING = 100;
WCHAR captionTitle[MAX_LOADSTRING]{}; // 제목 표시줄 텍스트입니다.
WCHAR captionClass[MAX_LOADSTRING]{}; // 기본 창 클래스 이름입니다.
HINSTANCE gameClient; // 현재 인스턴스입니다.

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
void SetTitle(HWND hwnd, const wchar_t* caption);

Timer gameTimer{ 100.0f };
GraphicsCore gameRenderer{ FRAME_BUFFER_W, FRAME_BUFFER_H };
Framework gameFramework{ gameRenderer, FRAME_BUFFER_W, FRAME_BUFFER_H };

CubeMesh* test_cube1, * test_cube2;
Material* default_material;

auto vs_shader = gameRenderer.CreateEmptyShader("vs_5_1");
auto ps_shader = gameRenderer.CreateEmptyShader("ps_5_1");
auto pl_vs_shader = gameRenderer.CreateEmptyShader("vs_5_1");
auto pl_ps_shader = gameRenderer.CreateEmptyShader("ps_5_1");

void InitialzeGame(HWND hwnd)
{
	//
	gameRenderer.SetHWND(hwnd).Awake();

	vs_shader.Complile("shaders/VertexShader.hlsl", "main");
	ps_shader.Complile("shaders/PixelShader.hlsl", "main");
	pl_vs_shader.Complile("PlainVertexShader.hlsl", "main");
	pl_ps_shader.Complile("PlainPixelShader.hlsl", "main");

	UINT nInputElementDescs = 2;
	auto pl_attributes = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pl_attributes[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT
		, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pl_attributes[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	auto pl_io_layout = gameRenderer.CreateEmptyInputLayout();
	pl_io_layout.pInputElementDescs = pl_attributes;
	pl_io_layout.NumElements = nInputElementDescs;
	auto rs_dest = gameRenderer.CreateEmptyRasterizerState();
	auto blend_dest = gameRenderer.CreateEmptyBlendState();
	auto ds_dest = gameRenderer.CreateEmptyDepthStencilState();
	auto pl_pipeline = gameRenderer.CreateEmptyPipeline();
	pl_pipeline.AttachVertexShader(pl_vs_shader);
	pl_pipeline.AttachPixelShader(pl_ps_shader);
	pl_pipeline.Attach(pl_io_layout);
	pl_pipeline.Attach(rs_dest);
	pl_pipeline.Attach(blend_dest);
	pl_pipeline.Attach(ds_dest);
	gameRenderer.RegisterPipeline(pl_pipeline);

	auto attributes = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	attributes[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT
		, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	attributes[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	auto io_layout = gameRenderer.CreateEmptyInputLayout();
	io_layout.pInputElementDescs = attributes;
	io_layout.NumElements = nInputElementDescs;
	auto pipeline = gameRenderer.CreateEmptyPipeline();
	pipeline.AttachVertexShader(vs_shader);
	pipeline.AttachPixelShader(ps_shader);
	pipeline.Attach(io_layout);
	pipeline.Attach(rs_dest);
	pipeline.Attach(blend_dest);
	pipeline.Attach(ds_dest);
	gameRenderer.RegisterPipeline(pipeline);

	auto dxdevice = gameRenderer.GetDevice();
	auto dxcmdlist = gameRenderer.GetCommandList();

	//
	auto camera = make_shared<GameCamera>();
	camera->SetViewport(float(FRAME_BUFFER_W), float(FRAME_BUFFER_H));
	camera->SetFOVAngle(60.0f);
	camera->CreatePerspectiveProjectionMatrix(1.0f, 1000.0f);
	camera->CreateOrthographicProjectionMatrix(1.0f, 1000.0f, float(FRAME_BUFFER_W), float(FRAME_BUFFER_H));
	camera->GenerateViewMatrix();

	auto testbed = gameFramework.RegisterScene(IlluminatedScene(gameFramework, "Test Scene", dxdevice, dxcmdlist));
	gameFramework.AddStage(testbed);

	camera->Init(dxdevice, dxcmdlist);
	testbed->SetCamera(camera);

	auto temp_mat = new RawMaterial();
	temp_mat->m_xmf4AlbedoColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	default_material = new Material(temp_mat);
	delete temp_mat;

	test_cube1 = new CubeMesh;
	test_cube1->AddMaterial(default_material);
	test_cube2 = new CubeMesh;
	test_cube2->AddMaterial(default_material);

	test_cube1->Awake(dxdevice, dxcmdlist);
	test_cube2->Awake(dxdevice, dxcmdlist);

	auto test_model = Load(dxdevice, dxcmdlist, "Model/Tree.bin");

	auto test_inst1 = testbed->CreateInstance<GameEntity>(0.0f, 0.0f, 1.0f);
	test_inst1->SetMesh(test_cube1);
	test_inst1->Attach(test_model);
	auto test_inst2 = testbed->CreateInstance<GameEntity>(10.0f, 0.0f, 0.0f);
	test_inst2->SetMesh(test_cube2);
	test_inst2->Attach(test_model);
	auto test_inst3 = testbed->CreateInstance<GameEntity>(0.0f, 1.0f, -3.0f);
	test_inst3->SetMesh(test_cube1);
	test_inst3->Attach(test_model);

	gameFramework.SetHWND(hwnd).SetHInstance(gameClient).Awake();

	gameRenderer.Start();
	gameRenderer.SetPipeline(0);
	gameFramework.Start();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, captionTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_FINALPROJECT, captionClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;
	while (true)
	{
		gameTimer.Tick();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		const auto delta_time = gameTimer.GetDeltaTime();
		gameFramework.Update(delta_time);
		gameRenderer.Update(delta_time);
	}

	gameRenderer.Release();

	return static_cast<int>(msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FINALPROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL; // MAKEINTRESOURCEW(IDC_FINALPROJECT);
	wcex.lpszClassName = captionClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int cmd_show)
{
	gameClient = hInstance;

	HWND window = CreateWindowW(captionClass, captionTitle
		, WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT, 0
		, CW_USEDEFAULT, 0
		, nullptr, nullptr, hInstance
		, nullptr);

	if (!window)
	{
		return FALSE;
	}

	ShowWindow(window, cmd_show);
	UpdateWindow(window);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps{};

	switch (msg)
	{
		case WM_CREATE:
		{
			SetTimer(hwnd, 0, 10, NULL);
		}
		break;

		case WM_TIMER:
		{
			if (0 == wp)
			{
				KillTimer(hwnd, 0);

				DialogBox(gameClient, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);

				InitialzeGame(hwnd);

				SetTimer(hwnd, 1, 10, NULL);
			}
			else
			{
				gameRenderer.PrepareRendering();
				gameFramework.PrepareRendering();
				test_cube1->Render(gameRenderer.GetCommandList());
				test_cube2->Render(gameRenderer.GetCommandList());
				gameFramework.Render();
				gameRenderer.Render();
			}
		}
		break;

		case WM_PAINT:
		{
			HDC hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;

		default:
		{
			return DefWindowProc(hwnd, msg, wp, lp);
		}
	}

	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:
		return (INT_PTR)TRUE;

		case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void SetTitle(HWND hwnd, const wchar_t* caption)
{
	SetWindowText(hwnd, caption);
}
