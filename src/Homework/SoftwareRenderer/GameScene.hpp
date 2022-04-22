#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"
#include "GameCamera.hpp"

using ObjectPtr = std::shared_ptr<GameObject>;
using CGroupPtr = std::shared_ptr<GameCollsionGroup>;

class GameScene
{
public:
	GameScene(UINT sz_horizontal, UINT sz_vertical, UINT sz_up);
	~GameScene();

	void SetCamera(std::shared_ptr<GameCamera> cam);

	void Start();
	void Animate(float elapsed_time);
	void Render(HDC surface);

	constexpr CGroupPtr&& CreateCollisionGroup();
	CGroupPtr FindProperGroup(const XMFLOAT3& position);

	template<class Type>
	ObjectPtr&& CreateInstance(float x, float y, float z);

	template<class Type>
	ObjectPtr&& CreateInstance(const XMFLOAT3& position);

	template<class Type>
	ObjectPtr&& CreateInstance(XMFLOAT3&& position);

	friend class GameCollsionGroup;

	// 게임 월드의 크기
	const UINT worldSizeH, worldSizeV, worldSizeU;

	// 기준 충돌 영역
	UINT collisionAreaIndex;

private:
	// 씬 내의 모든 인스턴스
	std::vector<ObjectPtr> Instances;

	// 씬 내의 충돌 영역
	std::vector<CGroupPtr> collisionAreas;
	
	// 프레임워크에서 받은 카메라
	std::shared_ptr<GameCamera> Camera;
};

class GameCollsionGroup
{
public:
	GameCollsionGroup(const UINT index, UINT sz_horizontal, UINT sz_vertical, UINT sz_up);

	void SetPosition(XMFLOAT3&& position);

	void AddInstance(ObjectPtr& ptr);

	bool Contains(const XMFLOAT3& point);

private:
	const UINT Index;
	BoundingBox Collider;

	std::vector<ObjectPtr> Instances;
};
