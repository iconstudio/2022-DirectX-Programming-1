#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"
#include "GameCamera.hpp"
#include "Fragment.hpp"
#include "Player.hpp"

using ObjectPtr = std::shared_ptr<GameObject>;
using CGroupPtr = std::shared_ptr<GameCollsionGroup>;

class GameScene
{
public:
	GameScene(GameFramework& framework, UINT sz_horizontal, UINT sz_vertical, UINT sz_up);
	~GameScene();

	void SetHwnd(HWND hwnd);
	void SetCamera(std::shared_ptr<GameCamera>& cam);

	void Start();
	void Update(float elapsed_time);
	void PrepareRendering();
	void Render(HDC surface);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	friend class GameCollsionGroup;

	// 게임 월드의 크기
	const INT worldSizeH, worldSizeV, worldSizeU;

	// 기준 충돌 영역
	UINT collisionAreaIndex;

private:
	void BuildCollisionGroups();
	void BuildWorld();
	void BuildObjects();
	void BuildEnemies();

	CGroupPtr CreateCollisionGroup();
	CGroupPtr FindProperGroup(const XMFLOAT3& position);

	template<class Type>
	Type* CreateInstance(float x, float y, float z);

	template<class Type>
	Type* CreateInstance(const XMFLOAT3& position);

	template<class Type>
	Type* CreateInstance(XMFLOAT3&& position);

	// 윈도우 핸들
	HWND Window;

	// 프레임워크
	GameFramework& Framework;

	// 씬 내의 모든 인스턴스
	std::vector<ObjectPtr> Instances;

	// 씬 내의 충돌 영역
	std::vector<CGroupPtr> collisionAreas;

	// 렌더링 할 충돌 영역
	std::vector<CGroupPtr> preparedCollisionAreas;

	// 플레이어 객체 (Instances 안에도 있음)
	std::shared_ptr<Player> myPlayer;

	// 프레임워크에서 받은 카메라
	std::shared_ptr<GameCamera> myCamera;

	// 플레이어 생성 위치
	XMFLOAT3 playerSpawnPoint = XMFLOAT3{ 5.0f, 0.0f, 1.0f };
};

class GameCollsionGroup
{
public:
	GameCollsionGroup(GameScene& scene, const size_t index, UINT sz_horizontal, UINT sz_vertical, UINT sz_up);

	void SetPosition(XMFLOAT3&& position);

	void AddInstance(ObjectPtr& ptr);

	void PrepareRendering();
	void AddFragment(const CFragment& fragment);
	void Render(HDC surface) const;

	bool Contains(const XMFLOAT3& point);

	friend class GameScene;

private:
	GameScene& Scene;
	const size_t Index;

	BoundingBox Collider;

	std::vector<ObjectPtr> Instances;
	std::vector<CFragment> Fragments;
};
