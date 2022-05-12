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

	// ������ �ڵ�
	HWND Window;

	// �����ӿ�ũ
	GameFramework& Framework;

	// ���� ����
	const int numberPillars;
	// ���� ���
	std::vector<Pillar*> Pillars;
	// ���� ����
	std::vector<shared_ptr<TerrainChunk>> Terrain;
	// ���Ա�
	RailBorder* boardFront, * boardBack;

	// �÷��̾��� ž�� ����
	bool isPlayerRiding;
	// �÷��̾ ��ġ�� ������ ��ȣ
	int worldPlayerPositionIndex;
	// �÷��̾ ��ġ�� ����
	shared_ptr<TerrainChunk> worldCurrentTerrain;
	// �÷��̾��� ��ġ (0~������ ����)
	float playerPosition;
	// �÷��̾��� �ӵ� (�ʴ� �ȼ� �Ÿ�)
	float playerSpeed;
	// �÷��̾��� ����� ��ġ (0~1)
	float playerWorldRelativePosition;
	// �÷��̾��� �⺻ �̵� �ӵ� (�ʴ� �ȼ� �Ÿ�)
	const float playerAccel = 2.0f;

	// ���� ��
	const size_t countEnemiesCube = 20, countEnemiesManta = 20;

	// �� ���� ������ �ʴ� �ν��Ͻ�
	std::vector<StaticPtr> staticInstances;
	// �� ���� ������ �ν��Ͻ�
	std::vector<ObjectPtr> myInstances;
	// �� ���� ��ƼŬ �ν��Ͻ� ����
	std::vector<ParticleBlob> myParticleBlobs;
	// �� ���� ��ƼŬ �ν��Ͻ�
	std::vector<EffectPtr> myParticles;
	// �÷��̾� ��ü
	shared_ptr<Player> myPlayer;
	// �����ӿ�ũ���� ���� ī�޶�
	shared_ptr<GameCamera> myCamera;

	// �÷��̾��� �޽�
	ModelPtr meshPlayer;
	ModelPtr meshPlayerBullet;
	ModelPtr meshEnemyCube;
	ModelPtr meshEnemyManta;
	ModelPtr meshEnemyBullet;

	// ��� �޽�
	ModelPtr meshFloor, meshSide;
	// ���Ա� �޽�
	ModelPtr meshEntrance;
	// ���� 1���� 15������ ����� �޽�
	ModelPtr meshPillars[15];
	// ���� �޽�
	ModelPtr meshRail;
	// ���� ȿ�� �޽�
	ModelPtr meshParticle;

	// ������ �� ����
	std::vector<CFragment> Fragments;

	// ������ ���� ��ȯ ���
	XMFLOAT4X4 globalMatrix;

	// ������ �� �� ����
	std::unordered_map<COLORREF, HPEN> Pens;

	// ���� ���
	struct { float left; float right; float top; float bottom; } worldBoundary;

	// �÷��̾� ���� ��ġ
	XMFLOAT3 playerSpawnPoint = XMFLOAT3{ 0.0f, 2.5f, 1.0f };

	// �ѷ��ڽ��� ���� ��ġ
	XMFLOAT3 pillarStartPoint = XMFLOAT3{ 0.0f, 0.0f, 5.0f };
};
