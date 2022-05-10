#pragma once
#include "stdafx.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"
#include "GameMesh.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"
#include "Player.hpp"
#include "PlayerBullet.hpp"
#include "CubeMesh.hpp"
#include "Terrains.hpp"
#include <array>

using ObjectPtr = shared_ptr<GameObject>;

class GameScene
{
public:
	GameScene(GameFramework& framework, int sz_x, int height, int sz_y);
	~GameScene();

	void SetHwnd(HWND hwnd);
	void SetCamera(shared_ptr<GameCamera>& cam);

	void Start();
	void Update(float elapsed_time);
	void PrepareRendering();
	void Render(HDC surface);

	void AddFragment(const CFragment& fragment);
	HPEN ReadyPen(COLORREF color);
	void PrepareRenderingCollider(const BoundingOrientedBox& collider);
	void PrepareRenderingCollider(const BoundingFrustum& collider);

	void Kill(GameObject* obj);
	void Kill(ObjectPtr& obj);
	Enemy* SpawnEnemy(ENEMY_TYPES type, const XMFLOAT3& pos);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	friend class GameCollsionGroup;
	friend class Player;
	friend class GameObject;

	// 메쉬
	shared_ptr<CMesh> meshPlayer;
	shared_ptr<CMesh> meshPlayerBullet;
	shared_ptr<CMesh> meshEnemyCube;
	shared_ptr<CMesh> meshEnemyManta;
	shared_ptr<CMesh> meshEnemyBullet;

	// 경계 메쉬
	shared_ptr<CMesh> meshFloor, meshSide;
	// 높이 1부터 15까지의 선로 기둥
	shared_ptr<CMesh> meshPillars[15];
	// 선로 메쉬
	shared_ptr<CMesh> meshRail;

private:
	void BuildMeshes();
	void BuildWorld();
	void BuildObjects();
	void CompleteBuilds();

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
	// 변하지 않는 객체
	std::vector<ObjectPtr>::difference_type staticBound;

	// 기준 충돌 영역
	size_t collisionAreaIndex;

	// 플레이어가 위치한 선로의 번호
	size_t worldPlayerPositionIndex;
	// 플레이어의 위치 (0~선로의 길이)
	float playerPosition;
	// 플레이어의 상대적 위치 (0~1)
	float playerWorldRelativePosition;
	// 플레이어의 기본 이동 속도 (초당 픽셀 거리)
	float playerMoveSpeed = 3.0f;
	// 선로 목록
	std::vector<Pillar*> Pillars;

	// 렌더링 할 조각
	std::vector<CFragment> Fragments;

	// 월드의 월드 변환 행렬
	XMFLOAT4X4 globalMatrix;

	// 렌더링 용 펜 모음
	std::unordered_map<COLORREF, HPEN> Pens;

	// 플레이어 객체
	shared_ptr<Player> myPlayer;

	// 프레임워크에서 받은 카메라
	shared_ptr<GameCamera> myCamera;

	// 월드 경계
	RECT worldBoundary;

	// 플레이어 생성 위치
	XMFLOAT3 playerSpawnPoint = XMFLOAT3{ 0.0f, 0.0f, 1.0f };

	// 롤러코스터 시작 위치
	XMFLOAT3 pillarStartPoint = XMFLOAT3{ 0.0f, 0.0f, 5.0f };
};
