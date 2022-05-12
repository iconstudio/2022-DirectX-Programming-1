#pragma once
#include "stdafx.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"
#include "GameStaticObject.hpp"
#include "GameParticle.hpp"
#include "Mesh.hpp"

using ModelPtr = shared_ptr<CMesh>;
using EntityPtr = shared_ptr<GameEntity>;
using ObjectPtr = shared_ptr<GameObject>;
using StaticPtr = shared_ptr<GameStaticObject>;
using EffectPtr = shared_ptr<GameParticle>;

struct TerrainChunk
{
	XMFLOAT3 from, to;

	XMFLOAT3 vector;
	XMFLOAT3 normal;
	XMFLOAT3 axis;

	float myLength = 0.0f;
};

class ParticleBlob
{
public:
	std::vector<EffectPtr>::iterator particle;
	static const size_t count = 30;
	float lifetime = 0.0f;

	bool IsActivated() const
	{
		return 0 < lifetime;
	}

	void Update(float elapsed_time)
	{
		if (0 < lifetime) lifetime -= elapsed_time;
	}
};

class GameScene
{
public:
	GameScene(GameFramework& framework);
	~GameScene();

	void SetHwnd(HWND hwnd);
	void SetCamera(shared_ptr<GameCamera>& cam);

	void Start();
	void Update(float elapsed_time);
	void PrepareRendering();
	void Render(HDC surface);

	bool CheckCameraBounds(const GameEntity* obj) const;
	void AddFragment(const CFragment& fragment);
	HPEN ReadyPen(COLORREF color);
	void PrepareRenderingCollider(const BoundingOrientedBox& collider);
	void PrepareRenderingCollider(const BoundingFrustum& collider);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

private:
	void BuildMeshes();
	void BuildWorld();
	void BuildObjects();
	void BuildParticles();
	void CompleteBuilds();

	void PlayerJumpToRail(const shared_ptr<TerrainChunk>& node);
	void PlayerJumpToBefore();
	void PlayerJumpToNext();
	bool PlayerMoveOnRail(float value);

	ParticleBlob& PopParticleBlob();
	void CastParticles(ParticleBlob& blob);

	template<class Type>
	Type* CreateInstance();

	template<class Type>
	Type* CreateInstance(float x, float y, float z);

	template<class Type>
	Type* CreateInstance(const XMFLOAT3& position);

	template<class Type>
	Type* CreateInstance(XMFLOAT3&& position);

	Enemy* SpawnEnemy(ENEMY_TYPES type, const XMFLOAT3& pos);

	// 윈도우 핸들
	HWND Window;

	// 프레임워크
	GameFramework& Framework;

	// 선로 개수
	const int numberPillars;
	// 선로 목록
	std::vector<Pillar*> Pillars;
	// 지형 정보
	std::vector<shared_ptr<TerrainChunk>> Terrain;
	// 출입구
	RailBorder* boardFront, * boardBack;

	// 플레이어의 탑승 여부
	bool isPlayerRiding;
	// 플레이어가 위치한 선로의 번호
	int worldPlayerPositionIndex;
	// 플레이어가 위치한 지형
	shared_ptr<TerrainChunk> worldCurrentTerrain;
	// 플레이어의 위치 (0~선로의 길이)
	float playerPosition;
	// 플레이어의 속도 (초당 픽셀 거리)
	float playerSpeed;
	// 플레이어의 상대적 위치 (0~1)
	float playerWorldRelativePosition;
	// 플레이어의 기본 이동 속도 (초당 픽셀 거리)
	const float playerAccel = 2.0f;

	// 적의 수
	const size_t countEnemiesCube = 20, countEnemiesManta = 20;

	// 씬 내의 변하지 않는 인스턴스
	std::vector<StaticPtr> staticInstances;
	// 씬 내의 동적인 인스턴스
	std::vector<ObjectPtr> myInstances;
	// 씬 내의 파티클 인스턴스 모음
	std::vector<ParticleBlob> myParticleBlobs;
	// 씬 내의 파티클 인스턴스
	std::vector<EffectPtr> myParticles;
	// 플레이어 객체
	shared_ptr<Player> myPlayer;
	// 프레임워크에서 받은 카메라
	shared_ptr<GameCamera> myCamera;

	// 플레이어의 메쉬
	ModelPtr meshPlayer;
	ModelPtr meshPlayerBullet;
	ModelPtr meshEnemyCube;
	ModelPtr meshEnemyManta;
	ModelPtr meshEnemyBullet;

	// 경계 메쉬
	ModelPtr meshFloor, meshSide;
	// 출입구 메쉬
	ModelPtr meshEntrance;
	// 높이 1부터 15까지의 기둥의 메쉬
	ModelPtr meshPillars[15];
	// 선로 메쉬
	ModelPtr meshRail;
	// 입자 효과 메쉬
	ModelPtr meshParticle;

	// 렌더링 할 조각
	std::vector<CFragment> Fragments;

	// 월드의 월드 변환 행렬
	XMFLOAT4X4 globalMatrix;

	// 렌더링 용 펜 모음
	std::unordered_map<COLORREF, HPEN> Pens;

	// 월드 경계
	struct { float left; float right; float top; float bottom; } worldBoundary;

	// 플레이어 생성 위치
	XMFLOAT3 playerSpawnPoint = XMFLOAT3{ 0.0f, 2.5f, 1.0f };

	// 롤러코스터 시작 위치
	XMFLOAT3 pillarStartPoint = XMFLOAT3{ 0.0f, 0.0f, 5.0f };
};
