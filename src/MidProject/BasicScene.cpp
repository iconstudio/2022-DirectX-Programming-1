#include "pch.hpp"
#include "BasicScene.hpp"

BasicScene::BasicScene(GameFramework& framework, const char* name)
	: myName(name)
	, myFramework(framework)
	, d3dDevice(nullptr), d3dTaskList(nullptr)
	, myBackgroundColor()
	, posCursor{ 0, 0 }
	, lastDeltaTime(0.0f)
{}

BasicScene::~BasicScene()
{}

void BasicScene::SetBackgroundColor(const float colors[4])
{
	myBackgroundColor[0] = colors[0];
	myBackgroundColor[1] = colors[1];
	myBackgroundColor[2] = colors[2];
	myBackgroundColor[3] = colors[3];
}

void BasicScene::SetBackgroundColor(const XMFLOAT4& color)
{
	myBackgroundColor[0] = color.x;
	myBackgroundColor[1] = color.y;
	myBackgroundColor[2] = color.z;
	myBackgroundColor[3] = color.w;
}

void BasicScene::SetBackgroundColor(XMFLOAT4&& color)
{
	myBackgroundColor[0] = std::forward<float>(color.x);
	myBackgroundColor[1] = std::forward<float>(color.y);
	myBackgroundColor[2] = std::forward<float>(color.z);
	myBackgroundColor[3] = std::forward<float>(color.w);
}

const std::string& BasicScene::GetName() const noexcept
{
	return myName;
}
