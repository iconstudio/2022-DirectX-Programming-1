#pragma once
#include "stdafx.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"
#include "GameStaticObject.hpp"

using EntityPtr = shared_ptr<GameEntity>;
using ObjectPtr = shared_ptr<GameObject>;
using StaticPtr = shared_ptr<GameStaticObject>;

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

	void Kill(ObjectPtr& obj);
	Enemy* SpawnEnemy(ENEMY_TYPES type, const XMFLOAT3& pos);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

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

	// ������ �ڵ�
	HWND Window;

	// �����ӿ�ũ
	GameFramework& Framework;

	// �÷��̾��� �޽�
	shared_ptr<CMesh> meshPlayer;
	shared_ptr<CMesh> meshPlayerBullet;
	shared_ptr<CMesh> meshEnemyCube;
	shared_ptr<CMesh> meshEnemyManta;
	shared_ptr<CMesh> meshEnemyBullet;

	// ��� �޽�
	shared_ptr<CMesh> meshFloor, meshSide;
	// ���� 1���� 15������ ���� ���
	shared_ptr<CMesh> meshPillars[15];
	// ���� �޽�
	shared_ptr<CMesh> meshRail;

	// �� ���� ������ �ʴ� �ν��Ͻ�
	std::vector<StaticPtr> staticInstances;
	// �� ���� ������ �ν��Ͻ�
	std::vector<ObjectPtr> myInstances;

	// ���� �浹 ����
	size_t collisionAreaIndex;

	// �÷��̾ ��ġ�� ������ ��ȣ
	size_t worldPlayerPositionIndex;
	// �÷��̾��� ��ġ (0~������ ����)
	float playerPosition;
	// �÷��̾��� ����� ��ġ (0~1)
	float playerWorldRelativePosition;
	// �÷��̾��� �⺻ �̵� �ӵ� (�ʴ� �ȼ� �Ÿ�)
	float playerMoveSpeed = 3.0f;
	// ���� ���
	std::vector<Pillar*> Pillars;

	// ������ �� ����
	std::vector<CFragment> Fragments;

	// ������ ���� ��ȯ ���
	XMFLOAT4X4 globalMatrix;

	// ������ �� �� ����
	std::unordered_map<COLORREF, HPEN> Pens;

	// �÷��̾� ��ü
	shared_ptr<Player> myPlayer;

	// �����ӿ�ũ���� ���� ī�޶�
	shared_ptr<GameCamera> myCamera;

	// ���� ���
	RECT worldBoundary;

	// �÷��̾� ���� ��ġ
	XMFLOAT3 playerSpawnPoint = XMFLOAT3{ 0.0f, 0.0f, 1.0f };

	// �ѷ��ڽ��� ���� ��ġ
	XMFLOAT3 pillarStartPoint = XMFLOAT3{ 0.0f, 0.0f, 5.0f };
};
