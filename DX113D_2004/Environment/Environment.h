#pragma once

class Cube;

class Environment : public Singleton<Environment>
{
	friend class Singleton;


public:
	void PostRender();

	void Set();

	void SetProjection();
	void SetViewport(UINT width = WIN_WIDTH, UINT height = WIN_HEIGHT);
	void SetTargetToCamera(Transform* target);


	Camera* GetTargetCamera() { return mTargetCamera; }
	Camera* GetWorldCamera() { return mWorldCamera; }
	Matrix GetProjection() { return mProjectionMatrix; }
	LightBuffer* GetLight() { return lightBuffer; }
	MatrixBuffer* GetProjectionBuffer() { return projectionBuffer; }
	Vector3 GetLightPosition();
	bool GetIsTargetCamera() { return mbIsTargetCamera; }

	bool GetIsEnabledTargetCamera() { return mbIsEnabledTargetCamera; }
	void SetIsEnabledTargetCamera(bool value) { mbIsEnabledTargetCamera = value; }

private:
	Environment();
	~Environment();

	void createPerspective();
	void showLightInformation();

private:

	Matrix mProjectionMatrix;

	MatrixBuffer* projectionBuffer;
	LightBuffer* lightBuffer;

	D3D11_VIEWPORT mViewPort;

	Camera* mTargetCamera;
	Camera* mWorldCamera;

	SamplerState* mSamplerState;
	bool mbIsTargetCamera;
	Transform* mTarget;

	Cube* mSun;

	bool mbIsEnabledTargetCamera;
	bool mbIsEnabledWorldCamera;
};