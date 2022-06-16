#pragma once

class BasicScene
{
public:
	BasicScene(GameFramework& framework, const char* name);
	virtual ~BasicScene();

	void SetBackgroundColor(const float colors[4]);
	void SetBackgroundColor(const XMFLOAT4& color);
	void SetBackgroundColor(XMFLOAT4&& color);

	const std::string& GetName() const noexcept;

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) = 0;
	virtual void Start() = 0;
	virtual void Reset() = 0;
	virtual void Update(float delta_time) = 0;
	virtual void PrepareRendering() = 0;
	virtual void Render() const = 0;

	virtual void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) = 0;
	virtual void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) = 0;
	virtual void OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) = 0;

	float myBackgroundColor[4];

protected:
	const std::string myName;
	GameFramework& myFramework;

	P3DDevice d3dDevice;
	P3DGrpCommandList d3dTaskList;

	POINT posCursor;
	float lastDeltaTime;
};

