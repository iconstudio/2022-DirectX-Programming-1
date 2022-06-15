#include "pch.hpp"
#include "StageHelliattack.hpp"
#include "GameFramework.h"
#include "Arithmetics.hpp"
#include "HelicopterPlayer.hpp"
#include "Terrains.hpp"

StageHelliattack::StageHelliattack(GameFramework& framework, HWND hwnd)
	: IlluminatedScene(framework, "Helliattack")
	, playerSpawnPoint()
	, myTerrain(256, 256)
	, handleWindow(hwnd)
{}

StageHelliattack::~StageHelliattack()
{}

void StageHelliattack::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	IlluminatedScene::Awake(device, cmdlist);

	constexpr float wscale = 10.0f;
	constexpr float hscale = 10.0f;

	// 지형 불러오기
	myTerrain.Awake("Resources/HeightMap.raw");
	myTerrain.Start(d3dDevice, d3dTaskList, { wscale, 1000.0f, hscale });

	worldWidth = myTerrain.myData.myMapWidth * wscale;
	worldHeight = myTerrain.myData.myMapHeight * hscale;

	XMFLOAT4 orientation{};
	XMStoreFloat4(&orientation, XMQuaternionIdentity());

	auto collider_copter = make_shared<BoundingOrientedBox>(XMFLOAT3()
		, XMFLOAT3(9.0f, 12.0f, 18.0f)
		, orientation);

	auto model_copter1 = myFramework.GetModel("Hellicoter1").lock();
	model_copter1->SetPosition(0.0f, 0.0f, 0.0f);
	model_copter1->SetScale(1.0f, 1.0f, 1.0f);

	playerSpawnPoint = XMFLOAT3(worldWidth * 0.5f, 350.0f, worldHeight * 0.5f);

	auto player = new HellicopterPlayer();
	player->Attach(model_copter1.get());
	player->Awake(d3dDevice, d3dTaskList);
	player->SetOriginalCollider(collider_copter);
	player->BuildCollider();
	player->SetPosition(playerSpawnPoint);
	player->LookTo(XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	auto cam = player->GetCamera();
	cam->SetPosition(playerSpawnPoint);
	cam->SetOffset(XMFLOAT3(0.0f, 500.0f, -900.0f));

	SetCamera(cam);
	myPlayer = player;

	//
	m_xmf4GlobalAmbient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	//
	numberLights = 1;
	myLights = new CLight[numberLights];
	ZeroMemory(myLights, sizeof(CLight) * numberLights);

	myLights[0].m_bEnable = true;
	myLights[0].m_nType = DIRECTIONAL_LIGHT;
	myLights[0].m_xmf4Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	myLights[0].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	myLights[0].m_xmf4Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	myLights[0].m_xmf3Direction = XMFLOAT3(0.3f, -0.5f, 0.5f);
}

void StageHelliattack::Start()
{
	IlluminatedScene::Start();
}

void StageHelliattack::Reset()
{
	IlluminatedScene::Reset();
}

void StageHelliattack::Update(float delta_time)
{
	IlluminatedScene::Update(delta_time);
}

void StageHelliattack::PrepareRendering()
{
	IlluminatedScene::PrepareRendering();
}

void StageHelliattack::Render() const
{
	IlluminatedScene::Render();

	XMFLOAT4X4 xmf4x4World{};

	ZeroMemory(&xmf4x4World, sizeof(xmf4x4World));
	const auto& terrain_mat = Matrix4x4::Identity();
	const auto tr_mat = XMLoadFloat4x4(&terrain_mat);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(tr_mat));

	// 두번째 루트 매개인자에서 0번째 메모리에 float 16개 전달
	d3dTaskList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

	myTerrain.Render(d3dTaskList);
}

void StageHelliattack::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{}

void StageHelliattack::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{}

void StageHelliattack::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageHelliattack::ProcessInput(UCHAR * pKeysBuffer)
{}
