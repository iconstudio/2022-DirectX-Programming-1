#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"
#include "GameCamera.hpp"
#include "Fragment.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "CubeMesh.hpp"

using ObjectPtr = std::shared_ptr<GameObject>;
using CGroupPtr = std::shared_ptr<GameCollsionGroup>;

class GameScene
{
public:
	GameScene(GameFramework& framework, size_t sz_x, size_t height, size_t sz_y);
	~GameScene();

	void SetHwnd(HWND hwnd);
	void SetCamera(std::shared_ptr<GameCamera>& cam);

	void Start();
	void Update(float elapsed_time);
	void PrepareRendering();
	void AddFragment(const CFragment& fragment);
	void Render(HDC surface);

	Enemy* SpawnEnemy(ENEMY_TYPES type, const XMFLOAT3& pos);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	friend class GameCollsionGroup;
	friend class Player;

	// 기준 충돌 영역
	size_t collisionAreaIndex;

	// 플레이어가 위치한 선로의 번호
	size_t worldPlayerPositionIndex;

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

	// 렌더링 할 조각
	std::vector<CFragment> Fragments;

	// 렌더링 용 펜 모음
	std::unordered_map<COLORREF, HPEN> Pens;

	// 플레이어 객체 (Instances 안에도 있음)
	std::shared_ptr<Player> myPlayer;

	// 프레임워크에서 받은 카메라
	std::shared_ptr<GameCamera> myCamera;

	// 플레이어 생성 위치
	XMFLOAT3 playerSpawnPoint = XMFLOAT3{ 5.0f, 0.0f, 1.0f };

	std::shared_ptr<CMesh> meshPlayer;
	std::shared_ptr<CMesh> meshPillar;
	std::shared_ptr<CMesh> meshRail;
	std::shared_ptr<CMesh> meshEnemy[2];
	//std::vector<Pillar> Pillars;
};

class GameCollsionGroup
{
public:
	GameCollsionGroup(GameScene& scene, const size_t index, size_t sz_x, size_t height, size_t sz_z);

	void SetPosition(XMFLOAT3&& position);

	void AddInstance(ObjectPtr& ptr);

	void PrepareRendering();
	void Render(HDC surface) const;

	bool Contains(const XMFLOAT3& point);

	friend class GameScene;

private:
	GameScene& Scene;
	const size_t Index;

	BoundingBox Collider;
	CubeMesh myMesh;

	std::vector<ObjectPtr> Instances;
};
