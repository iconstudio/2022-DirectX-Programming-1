#pragma once

class BasicScene
{
public:
	BasicScene(GameFramework& framework, const char* name);
	virtual ~BasicScene();

	void SetBackgroundColor(const float colors[4]);
	void SetBackgroundColor(const XMFLOAT4& color);
	void SetBackgroundColor(XMFLOAT4&& color);
	void SetRootSignature(P3DSignature signature);

	const std::string& GetName() const noexcept;
	P3DSignature GetRootSignature();
	P3DSignature const GetRootSignature() const;

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) = 0;
	virtual void Start() = 0;
	virtual void Reset() = 0;
	virtual void Update(float delta_time) = 0;
	virtual void PrepareRendering() = 0;
	virtual void Render() = 0;

	virtual void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) = 0;
	virtual void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) = 0;
	virtual void OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) = 0;

	float myBackgroundColor[4];

protected:
	virtual P3DSignature CreateGraphicsRootSignature() = 0;

	const std::string myName;
	GameFramework& myFramework;

	P3DDevice d3dDevice;
	P3DGrpCommandList d3dTaskList;
	P3DSignature mySignature;

	POINT posCursor;
	float lastDeltaTime;
};

