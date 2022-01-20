#pragma once

class Cube;

class Environment : public Singleton<Environment>
{
	friend class Singleton;


public:
	void PostRender();

	void Set();

	void SetPerspectiveProjectionBuffer();
	void SetOrthographicProjectionBuffer();
	void SetViewport(UINT width = WIN_WIDTH, UINT height = WIN_HEIGHT);
	void SetTargetToCamera(Transform* target);


	Camera* GetTargetCamera() { return mTargetCamera; }
	Camera* GetWorldCamera() { return mWorldCamera; }
	Matrix GetProjection() { return mPerspectiveProjectionMatrix; }
	LightBuffer* GetLight() { return mLightBuffer; }
	ProjectionBuffer* GetPerspectiveProjectionBuffer() { return mPerspectiveProjectionBuffer; }
	ProjectionBuffer* GetOrthographicProjectionBuffer() { return mOrthographicProjectionBuffer; }
	Vector3 GetLightPosition();

	bool GetIsTargetCamera() { return mbIsTargetCamera; }
	bool GetIsEnabledTargetCamera() { return mbIsEnabledTargetCamera; }
	void SetIsEnabledTargetCamera(bool value) { mbIsEnabledTargetCamera = value; }

private:
	Environment();
	~Environment();

	void createPerspectiveBuffer();
	void createOrthographicBuffer();
	void showLightInformation();

private:

	Matrix mPerspectiveProjectionMatrix;
	Matrix mOrthographicProjectionMatrix;

	ProjectionBuffer* mPerspectiveProjectionBuffer;
	ProjectionBuffer* mOrthographicProjectionBuffer;
	LightBuffer* mLightBuffer;

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